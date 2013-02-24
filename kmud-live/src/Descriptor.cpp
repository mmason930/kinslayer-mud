
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "accounts.h"
#include "olc.h"
#include "comm.h"
#include "kuDescriptor.h"

#include "Descriptor.h"
#include "UserMacro.h"
#include "StringUtil.h"
#include "CharacterUtil.h"

extern kuDescriptor *gatewayConnection;
extern Descriptor *descriptor_list;
extern sql::Connection gameDatabase;

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
		this->snoop_by->Send( "Your snoop target is no longer among us.\r\n" );
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

void Descriptor::sendInstant( const std::string &str )
{
	if(this->getGatewayDescriptorType() == GatewayDescriptorType::websocket)
	{
		Character *loggingCharacter = this->character;

		if(this->original)
			loggingCharacter = this->original;
		if(loggingCharacter)
			loggingCharacter->LogOutput(str);

		socketWriteInstant(encodeWebSocketOutputCommand(str.c_str()), false);
	}
	else
		socketWriteInstant(str, true);
}

void Descriptor::socketWriteInstant( const std::string &str, bool recordToUserLog )
{
	this->descriptor->socketWriteInstant( str );

	Character *loggingCharacter = this->character;

	if(this->original)
		loggingCharacter = this->original;

	if(loggingCharacter && recordToUserLog)
	{
		loggingCharacter->LogOutput(str);
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
	if(pulse % (PASSES_PER_SEC * 1) == 0)
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

void Descriptor::sendWebSocketUsernameCommand(const std::string &username, const std::list<UserMacro *> &userMacros)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Username";
	commandObject["username"] = username;
	commandObject["macros"] = Json::Value();

	for(auto userMacroIter = userMacros.begin();userMacroIter != userMacros.end();++userMacroIter)
	{
		Json::Value userMacroObject;
		userMacroObject["keyCode"] = (*userMacroIter)->getKeyCode();
		userMacroObject["replacement"] = (*userMacroIter)->getReplacement();
		userMacroObject["id"] = (*userMacroIter)->getId();
		
		commandObject["macros"].append(userMacroObject);
	}

	sendWebSocketCommand(writer.write(commandObject));
}

void Descriptor::sendWebSocketPlayersOnlineCommand(const int playersOnline)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Players Online";
	commandObject["numberOfPlayers"] = playersOnline;

	if(character)
	{
		commandObject["level"] = GET_LEVEL(character);
		commandObject["expToLevel"] = character->ExperienceToLevel();
		commandObject["weavePoints"] = character->points.weave;
		commandObject["hitPoints"] = GET_HIT(character);
		commandObject["maxHitPoints"] = GET_MAX_HIT(character);
		commandObject["spellPoints"] = GET_MANA(character);
		commandObject["maxSpellPoints"] = GET_MAX_MANA(character);
		commandObject["movePoints"] = GET_MOVE(character);
		commandObject["maxMovePoints"] = GET_MAX_MOVE(character);
		commandObject["title"] = GET_TITLE(character);
		commandObject["offensive"] = character->Offense();
		commandObject["dodge"] = character->Dodge();
		commandObject["parry"] = character->Parry();
		commandObject["asbsorb"] = character->Absorb();
	}

	sendWebSocketCommand(writer.write(commandObject));
}

std::string Descriptor::encodeWebSocketOutputCommand(const char *output)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Output";
	commandObject["data"] = output;

	return encodeWebSocketCommand(writer.write(commandObject));
}

std::string Descriptor::encodeWebSocketCommand(const std::string &command)
{
	return command + (char)0x06;
}

void Descriptor::sendWebSocketCommand(const std::string &command)
{
		this->socketWriteInstant(encodeWebSocketCommand(command), false);
}

void Descriptor::processWebSocketSaveUserMacroCommand(Json::Value &commandObject)
{
	if(this->character)
	{
		UserMacro *userMacro = NULL;
		unsigned short keyCode;
		std::string replacement;

		if(commandObject["keyCode"].isNull() || !commandObject["keyCode"].isInt())
		{
			MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro with invalid keyCode.", GET_NAME(character));
			return;
		}
		if(commandObject["replacement"].isNull() || !commandObject["replacement"].isString())
		{
			MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro with invalid replacement.", GET_NAME(character));
			return;
		}

		keyCode = (unsigned short)commandObject["keyCode"].asInt();
		replacement = commandObject["replacement"].asString();

		if(!commandObject["id"].isNull() && commandObject["id"].isInt())
		{
			int userMacroId = commandObject["id"].asInt();
			userMacro = CharacterUtil::getUserMacro(gameDatabase, userMacroId);

			if(userMacro == NULL)
			{
				MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save user macro that does not exist. ID: %d. Key: %d, Replacement: %s", GET_NAME(character), userMacroId, keyCode, StringUtil::vaEscape(replacement).c_str());
				return;
			}

			if(userMacro->getUserId() != character->getUserId())
			{
				MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro that belongs to another user. ID: %d.", GET_NAME(character), userMacroId);
				delete userMacro;
				return;
			}
		}
		else
		{
			CharacterUtil::deleteUserMacro(gameDatabase, character->getUserId(), keyCode);

			userMacro = new UserMacro();
			userMacro->setUserId(character->getUserId());
			userMacro->setCreatedDatetime(DateTime());
		}

		userMacro->setKeyCode(keyCode);
		userMacro->setReplacement(replacement);

		CharacterUtil::putUserMacro(gameDatabase, userMacro);

		Json::Value commandResponse;
		commandResponse["method"] = "Save User Macro";
		commandResponse["wasSuccessful"] = true;
		commandResponse["userMacroId"] = userMacro->getId();
		commandResponse["keyCode"] = userMacro->getKeyCode();
		commandResponse["replacement"] = userMacro->getReplacement();

		Json::FastWriter writer;

		this->sendWebSocketCommand(writer.write(commandResponse));

		delete userMacro;
	}
}

void Descriptor::processWebSocketDeleteUserMacroCommand(Json::Value &commandObject)
{
	if(!character)
		return;

	if(commandObject["keyCode"].isNull() || !commandObject["keyCode"].isInt())
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to delete user macro with invalid ID.", GET_NAME(character));
		return;
	}

	unsigned short keyCode = (unsigned short)commandObject["keyCode"].asInt();
	CharacterUtil::deleteUserMacro(gameDatabase, character->getUserId(), keyCode);
}

void Descriptor::processWebSocketCommands()
{
	while(true)
	{
		const char *inputDataBuffer = this->descriptor->getInputDataBuffer();
		const char *endOfCommandPointer = strchr(inputDataBuffer, 0x06);

		if(endOfCommandPointer != NULL)
		{
			std::string jsonCommand = std::string(inputDataBuffer, endOfCommandPointer - inputDataBuffer);
			Json::Value commandObject;
			Json::Reader reader;

			if(!reader.parse(jsonCommand, commandObject, false))
			{
				MudLog(BRF, LVL_APPR, TRUE, "Could not process websocket command. Input: %s", StringUtil::vaEscape(jsonCommand).c_str());
				break;
			}

			this->descriptor->eraseInput(0, (endOfCommandPointer - inputDataBuffer) + 1);
			
			std::string method = commandObject["method"].asString();
			if(method == "Input")
			{
				this->commandQueue.push_back(commandObject["data"].asString());
			}
			else if(method == "Save User Macro")
			{
				processWebSocketSaveUserMacroCommand(commandObject);
			}
			else if(method == "Delete User Macro")
			{
				processWebSocketDeleteUserMacroCommand(commandObject);
			}
		}
		else
			break;
	}
}
