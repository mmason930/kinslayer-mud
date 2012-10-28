
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "accounts.h"
#include "olc.h"
#include "comm.h"
#include "kuDescriptor.h"

#include <flusspferd.hpp>

#include "Descriptor.h"

flusspferd::object JS_parseJson(std::string jsonText);
std::string JS_stringifyJson(flusspferd::object obj);

extern kuDescriptor *gatewayConnection;
extern Descriptor *descriptor_list;

Descriptor::Descriptor()
{
	descriptor = 0;
	memset( &host, 0, sizeof( host ) );
	bad_pws = 0;
	idle_tics = 0;
	connected = 0;
	wait = 0;
	desc_num = 0;
	showstr_head = 0;
	showstr_vector= 0;
	showstr_count = 0;
	showstr_page = 0;
	str = 0;
	max_str = 0;
	backstr = 0;
	character = 0;
	original = 0;
	snooping = 0;
	snoop_by = 0;
	next = 0;
	olc = 0;
	loggedIn = false;
}

Descriptor::~Descriptor()
{
}

void Descriptor::closeSocketClean()
{
	gatewayConnection->send("Close " + session + "\n");

	this->descriptor->socketClose();
}

//Calling this should disconnect the player from the game, clean up the descriptor,
//and also send a message to the gateway to close the client connection.
void Descriptor::disconnect()
{
	if(gatewayConnection != NULL && !session.empty()) {

		gatewayConnection->send("Close " + session + "\n");
	}

	cleanup();
}

//Calling this should disconnect the player from the game, clean up the descriptor,
//but send a message to the gateway telling it to hold on to the connection.
void Descriptor::persistentDisconnect()
{
	if(gatewayConnection != NULL) {

		gatewayConnection->send("PersistentClose " + session + "\n");
	}

	cleanup();
}

void Descriptor::cleanup()
{
	Descriptor* temp;

	REMOVE_FROM_LIST( this, descriptor_list, next );

	Character *ch;
	if( ((ch = character) || (ch = original)) && loggedIn ) {
		Account account = SwitchManager::GetManager().GetAccountByPlayerID( ch->player.idnum );
		//We do not want to put the mob's idnum or their name in the switch manager if this descriptor is switched.
		SwitchManager::GetManager().AddSwitch( this->host, ch->player.name, time(0), account.IsValid() ? account.GetID() : (-1) );
	}

	/* Forget snooping */
	if ( this->snooping )
		this->snooping->snoop_by = NULL;

	if ( this->snoop_by )
	{
		this->snoop_by->Send( "Your victim is no longer among us.\r\n" );
		this->snoop_by->snooping = NULL;
	}

	/*. Kill any OLC stuff .*/
	switch ( this->connected )
	{
	case CON_OEDIT:
	case CON_REDIT:
	case CON_ZEDIT:
	case CON_MEDIT:
	case CON_SEDIT:
	case CON_AEDIT:
	case CON_SGEDIT:
	case CON_CEDIT:
	case CON_CLEDIT:
	case CON_KEDIT:
	case CON_WEDIT:
	case CON_AUCTION:
#ifdef KINSLAYER_JAVASCRIPT
	case CON_JEDIT:
#endif
	cleanup_olc( this, CLEANUP_ALL );

	default:
		break;
	}

	if ( this->character )
	{
		/*
		 Plug memory leak, from Eric Green.
		*/

		if ( PLR_FLAGGED( this->character, PLR_MAILING ) && this->str )
		{
			if ( *( this->str ) )
				delete[] ( *( this->str ) );

			delete[] ( this->str );
		}

		if ( STATE( this ) == CON_PLAYING || STATE( this ) == CON_DISCONNECT )
		{
			Act( "$n has lost $s link.", TRUE, this->character, 0, 0, TO_ROOM );
			MudLog( NRM, MAX( LVL_IMMORT,
				GET_INVIS_LEV( this->character ) ), TRUE, "Closing link to: %s.", GET_NAME( this->character ) );
			this->character->desc = NULL;
		}

		else
		{
			MudLog( CMP, MAX( GET_INVIS_LEV( this->character ), LVL_APPR ), TRUE, "Losing player: %s.",
			        !this->character->player.name.empty() ? GET_NAME( this->character ) : "<null>" );
		}
	}

	else
		MudLog( CMP, LVL_IMMORT, TRUE, "Losing descriptor without char." );

	/* JE 2/22/95 -- part of my unending quest to make switch stable */

	if ( this->original && this->original->desc )
		this->original->desc = NULL;

	if ( this->showstr_head )
		delete[] ( this->showstr_head );

	if ( this->showstr_count )
		delete[] ( this->showstr_vector);
	if ( this->character )
		this->character->desc = 0;

	delete ( this );
}

void Descriptor::socketWriteInstant( const std::string &str )
{
	this->descriptor->socketWriteInstant( str );
	if(this->character && PLR_FLAGGED(this->character, PLR_LOGGER))
	{
		this->character->LogOutput(str);
	}
}

GatewayDescriptorType *Descriptor::getGatewayDescriptorType() const
{
	return gatewayDescriptorType;
}

void Descriptor::setGatewayDescriptorType(GatewayDescriptorType *gatewayDescriptorType)
{
	this->gatewayDescriptorType = gatewayDescriptorType;
}

void Descriptor::sendWebSocketCommands(const int pulse)
{
	if(pulse % (PASSES_PER_SEC * 5) == 0)
	{
		int playersOnline = 0;

		for(auto descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
		{
			if(descriptor->character)
				++playersOnline;
		}

		for(auto descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
		{
			if(descriptor->getGatewayDescriptorType() != GatewayDescriptorType::websocket)
				continue;
			if(!descriptor->loggedIn)
				continue;

			descriptor->sendWebSocketPlayersOnlineCommand(playersOnline);
		}
	}
}

void Descriptor::sendWebSocketUsernameCommand(const std::string &username)
{
	flusspferd::object commandObject = flusspferd::create_object(flusspferd::object());

	commandObject.set_property("method", "Username");
	commandObject.set_property("username", username);

	sendWebSocketCommand(JS_stringifyJson(commandObject));
}

void Descriptor::sendWebSocketPlayersOnlineCommand(const int playersOnline)
{
	flusspferd::object commandObject = flusspferd::create_object(flusspferd::object());

	commandObject.set_property("method", "Players Online");
	commandObject.set_property("numberOfPlayers", playersOnline);

	sendWebSocketCommand(JS_stringifyJson(commandObject));
}

void Descriptor::sendWebSocketCommand(const std::string &command)
{
	std::stringstream buffer;

	buffer << (unsigned char)0x1B
		   << (unsigned char)0x06
		   << (unsigned char)0x1B
		   << command
		   << (unsigned char)0x06
		   << (unsigned char)0x1B
		   << (unsigned char)0x06;

	this->socketWriteInstant(buffer.str().c_str());
}
