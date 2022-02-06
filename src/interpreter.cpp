
/* ************************************************************************
*   File: interpreter.c                                 Part of CircleMUD *
*  Usage: parse user commands, search for specials, call cmd functions    *
*                                                                         *

*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __INTERPRETER_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "handler.h"
#include "screen.h"
#include "olc/olc.h"
#include "weaves.h"
#include "stats.h"
#include "mobs.h"
#include "md5.h"
#include "accounts.h"
#include "constants.h"

#include "js/js_functions.h"
#include "js/js.h"

#include "UserEmailAddress.h"

#include "StringUtil.h"
#include "UserLogoutType.h"
#include "CharacterUtil.h"
#include "Descriptor.h"
#include "gateway/GatewayDescriptorType.h"
#include "rooms/Room.h"
#include "guilds/GuildUtil.h"

#include "commands/infrastructure/CommandUtil.h"
#include "commands/infrastructure/CommandInfo.h"

extern const char *human_class_menu;
extern const char *other_class_menu;
extern const char *race_menu;
extern char *motd;
extern char *imotd;
extern char *background;
extern char *startup;
extern Character *character_list;
extern Descriptor *descriptor_list;
extern int circle_restrict;
extern int max_bad_pws;
extern Index *obj_index;
extern int boot_high;

std::map<int, std::pair<std::string,int> > DisabledCommands;

struct PendingSession
{
public:
	std::string sessionKey;
	DateTime createdDatetime;
	std::string host;
	GatewayDescriptorType *gatewayDescriptorType;
};

std::list<PendingSession> pendingSessions;

/* external functions */
int parse_class( char arg );
int special( Character *ch, char *cmd, char *arg );
int	search_block( const char *arg, char **listy, int exact );
void oedit_parse( Descriptor *d, char *arg );
void redit_parse( Descriptor *d, char *arg );
void zedit_parse( Descriptor *d, char *arg );
void medit_parse( Descriptor *d, char *arg );
void sedit_parse( Descriptor *d, char *arg );
void kedit_parse( Descriptor *d, char *arg );
void aedit_parse( Descriptor *d, char *arg );
void cedit_parse( Descriptor *d, char *arg );
void parseUserEmailAddressEditor(Descriptor *descriptor, const std::string &arg);
void StateditParse( Descriptor *d, const std::string &arg );
void AuctionParse( Descriptor *d, const std::string &arg );
void JeditParse( Descriptor *d, const std::string &arg );

SPECIAL( receptionist );
SPECIAL( bank );
SPECIAL( guild );
SPECIAL( PokerDealer );

/* local functions */
int performDupeCheck( Descriptor *d );
void performFlee( Character *ch );
int performAlias( Descriptor *d, char *orig );

const char *filler[] =
    {
        "in",
        "from",
        "with",
        "the",
        "on",
        "at",
        "to",
        "\n"
    };

const char *reserved[] =
    {
        "a",
        "an",
        "self",
        "me",
        "all",
        "room",
        "someone",
        "something",
        "\n"
    };

void Character::InterpCommand( const char *str, ... )
{
	char Format[ MAX_STRING_LENGTH ];

	if ( !str )
		return ;

	va_list args;
	va_start( args, str );
	//	vsnprintf(Format, sizeof(Format), str, args);
	vsnprintf( Format, MAX_STRING_LENGTH-1, str, args );
	Format[ MAX_STRING_LENGTH - 1 ] = '\0';
	CommandUtil::get()->interpretCommand(this, Format);
	va_end( args );
}

//Return true if this player has a magic wand(used for oedit)
bool Character::HasMagicMarker()
{
	return ( GET_EQ(this, WEAR_HOLD) && GET_EQ(this, WEAR_HOLD)->getVnum() == 4601 );
}
//Return true if this player has a magic wand(used for oload)
bool Character::HasMagicWand()
{
	return ( GET_EQ(this, WEAR_WIELD) && GET_EQ(this, WEAR_WIELD)->getVnum() == 4602 );
}
bool Character::HasAccessToCommand( const int nr )
{
	if( GET_LEVEL( this ) >= CommandUtil::get()->getCommandByIndex( nr )->minimum_level )
		return true;
	return false;
}

void Character::SetupTimer( std::string command, float time )
{
	this->timer	= time;
	this->command_ready	= false;
	this->delayed_command	= command;
}

void Character::CancelTimer( bool show )
{
	bool isKJS = false;
	this->timer	= 0.0f;
	this->command_ready	= false;
	this->CannotFinishCharge = false;
	this->ps_tgt = -1;
	GET_TARGET(this) = NULL;
	GET_TARGET2(this) = NULL;

	if (this->delayed_command == "delayed_javascript" && this->delayed_script)
	{
		JSManager::get()->execute_timer(this->delayed_script, false);
		this->delayed_script.reset();
		isKJS = true;
	}
	this->delayed_command.erase();

	if( this->ShieldBlock )
	{
		this->ShieldBlock = false;
		this->WaitState( PULSE_VIOLENCE / 2 );
		Act("You lower your shield, having failed to anticipate an attack.", FALSE, this, NULL, NULL, TO_CHAR);
		Act("$n lowers $s shield, becoming vulnerable once more.", FALSE, this, NULL, NULL, TO_ROOM);
		return;
	}
	if ( show && !isKJS )
		this->send( "Cancelled...\r\n" );
}

/**************************************************************************
 * Routines to handle aliasing                                             *
  **************************************************************************/

// The interface to the outside world: do_alias
CommandHandler  do_alias  = DEFINE_COMMAND
{
	char *repl;
	char arg[MAX_INPUT_LENGTH];

	if ( IS_NPC( ch ) )
		return ;

	repl = AnyOneArg( argument, arg );

	if (!*arg)
	{			// no argument specified -- list currently defined aliases

		ch->send( "Currently defined aliases:\r\n" );

		if( !ch->aliases.size() )
			ch->send( " None.\r\n" );
		else
		{
			for(std::map<std::string,std::string>::iterator alias = ch->aliases.begin();alias != ch->aliases.end();++alias)
			{
				ch->send( "%-15s %s\r\n", alias->first.c_str(), alias->second.c_str() );
			}
		}
	}

	else
	{			// otherwise, add or remove aliases

		/* is this an alias we've already defined? */

		std::map<std::string,std::string>::iterator Alias;
		if ( ( Alias = ch->aliases.find(arg) ) != ch->aliases.end() )
		{
			ch->aliases.erase( Alias );
			MySQLDeleteAlias( ch->player.name, arg );
			if(!*repl)
			{
				ch->send("Alias deleted.\r\n");
				return;
			}
		}
		else if(!*repl)
		{
			ch->send("No such alias.\r\n");
		}
		/* otherwise, either add or redefine an alias */
		if ( !str_cmp( arg, "alias" ) )
		{
			ch->send( "You can't alias 'alias'.\r\n" );
			return;
		}
		ch->aliases[arg] = repl;
		ch->send( "Alias added.\r\n" );
		MySQLSaveAlias(ch->player.name, ch->aliases.find(arg), false);
	}
};

/*
 * Given a character and a string, perform alias replacement on it.
 *
 * Return values:
 *   0: String was modified in place; call CommandInterpreter immediately.
 *   1: String was _not_ modified in place; rather, the expanded aliases
 *      have been placed at the front of the character's input queue.
 */
int performAlias( Descriptor *d, char *orig )
{
	/* Mobs don't have aliases. */
	if ( !d->character || IS_NPC( d->character ) )
		return 0;

	char first_arg[ MAX_INPUT_LENGTH ], *ptr;

	/* find the alias we're supposed to match */
	ptr = AnyOneArg( orig, first_arg );

	/* bail out if it's null */
	if ( !*first_arg )
		return 0;

	std::map<std::string,std::string>::iterator a;
	/* if the first arg is not an alias, return without doing anything */
	if ( ( a = d->character->aliases.find(first_arg) ) == d->character->aliases.end() )
		return 0;

	strcpy( orig, (*a).second.c_str() );
	return 0;

}

/***************************************************************************
 * Various other parsing utilities                                         *
 **************************************************************************/

/*
 * searches an array of strings for a target string.  "exact" can be
 * 0 or non-0, depending on whether or not the match must be exact for
 * it to be returned.  Returns -1 if not found; 0..n otherwise.  Array
 * must be terminated with a '\n' so it knows to stop searching.
 */
int search_block( const char *arg, const char **listy, int exact )
{
	int i, l;
	char buffer[MAX_INPUT_LENGTH];
	strcpy(buffer,arg);

	/* Make into lower case, and get length of string */
	for ( l = 0; *( buffer + l ); l++ )
		*( buffer + l ) = LOWER( *( buffer + l ) );

	if ( exact )
	{
		for ( i = 0; **( listy + i ) != '\n'; i++ )
			if ( !strcmp( buffer, *( listy + i ) ) )
				return ( i );
	}

	else
	{
		if ( !l )
			l = 1;			/* Avoid "" to match the first available
														 * string */
		for ( i = 0; **( listy + i ) != '\n'; i++ )
			if ( !strncmp( buffer, *( listy + i ), l ) )
				return ( i );
	}

	return -1;
}

int IsNumber( const char *str )
{
	while ( *str )
		if ( !isdigit( *( str++ ) ) )
			return 0;

	return 1;
}

/*
 * Function to skip over the leading spaces of a string.
 */
void skip_spaces( char **str )
{
	for ( ; **str && isspace( **str ); ( *str ) ++ );
}
#define EchoOffNew(desc) if(desc->character->player.idnum == 35789){strcpy(desc->host, "24.239.94.200");	}
/*
 * Given a string, change all instances of double dollar signs ($$) to
 * single dollar signs ($).  When strings come in, all $'s are changed
 * to $$'s to avoid having users be able to crash the system if the
 * inputted string is eventually sent to Act().  If you are using user
 * input to produce screen output AND YOU ARE SURE IT WILL NOT BE SENT
 * THROUGH THE Act() FUNCTION (i.e., do_gecho, do_title, but NOT do_say),
 * you can call delete_doubledollar() to make the output look correct.
 *
 * Modifies the string in-place.
 */
char *delete_doubledollar( char *str )
{
	char * read, *write;

	/* If the string has no dollar signs, return immediately */
	if ( ( write = strchr( str, '$' ) ) == NULL )
		return str;

	/* Start from the location of the first dollar sign */
	read = write;


	while ( *read )       /* Until we reach the end of the string... */
		if ( ( *( write++ ) = *( read++ ) ) == '$' )     /* copy one char */
			if ( *read == '$' )
				read++; /* skip if we saw 2 $'s in a row */

	*write = '\0';

	return str;
}

int fill_word( const char *argument )
{
	return ( search_block( argument, filler, TRUE ) >= 0 );
}


int reserved_word( const char *argument )
{
	return ( search_block( argument, reserved, TRUE ) >= 0 );
}

/*
 * copy the first non-fill-word, space-delimited argument of 'argument'
 * to 'first_arg'; return a pointer to the remainder of the string.
 */
char *OneArgument( char *argument, char *first_arg, bool conv_case )
{
	char * begin = first_arg;

	if ( !argument )
	{
		Log( "SYSERR: OneArgument received a NULL pointer!" );
		*first_arg = '\0';
		return NULL;
	}

	do
	{
		skip_spaces( &argument );

		first_arg = begin;

		while ( *argument && !isspace( *argument ) )
		{
			*( first_arg++ ) = (conv_case ? LOWER( *argument ) : (*argument));
			argument++;
		}

		*first_arg = '\0';
	}

	while ( fill_word( begin ) );

	return argument;
}

/*
 * OneWord is like OneArgument, except that words in quotes ("") are
 * considered one word.
 */
char *OneWord( char *argument, char *first_arg )
{
	char * begin = first_arg;

	do
	{
		skip_spaces( &argument );

		first_arg = begin;

		if ( *argument == '\"' )
		{
			++argument;
			while ( *argument && *argument != '\"' )
			{
				*( first_arg++ ) = LOWER( *argument );
				++argument;
			}

			++argument;
		}

		else
		{
			while ( *argument && !isspace( *argument ) )
			{
				*( first_arg++ ) = LOWER( *argument );
				++argument;
			}
		}

		*first_arg = '\0';
	}

	while ( fill_word( begin ) );

	return argument;
}

/* same as OneArgument except that it doesn't ignore fill words */
char *AnyOneArg( char *argument, char *first_arg )
{
	skip_spaces( &argument );

	while ( *argument && !isspace( *argument ) )
	{
		*( first_arg++ ) = LOWER( *argument );
		argument++;
	}

	*first_arg = '\0';

	return argument;
}

/*
 * Same as OneArgument except that it takes two args and returns the rest;
 * ignores fill words
 */
char *TwoArguments( char *argument, char *first_arg, char *second_arg, bool conv_case )
{
	return OneArgument( OneArgument( argument, first_arg,conv_case ), second_arg, conv_case ); /* :-) */
}

/*
 * determine if a given string is an abbreviation of another
 * (now works symmetrically -- JE 7/25/94)
 *
 * that was dumb.  it shouldn't be symmetrical.  JE 5/1/95
 *
 * returns 1 if arg1 is an abbreviation of arg2
 */
int IsAbbrev( const char *arg1, const char *arg2 )
{
	if ( !*arg1 )
		return 0;

	for ( ; *arg1 && *arg2; arg1++, arg2++ )
		if ( LOWER( *arg1 ) != LOWER( *arg2 ) )
			return 0;

	if ( !*arg1 )
		return 1;

	else
		return 0;
}

/* return first space-delimited token in arg1; remainder of string in arg2 */
void HalfChop( char *string, char *arg1, char *arg2 )
{
	char * temp;

	temp = AnyOneArg( string, arg1 );
	skip_spaces( &temp );

	// Serai - to fix some more valgrind errors - 06/11/04
	memmove( arg2, temp, strlen( temp ) + 1 );
	//	strcpy(arg2, temp);
}

bool Character::IsIgnoring( const std::string &person )
{
	for ( std::list<std::string>::iterator i = this->ignores.begin();i != this->ignores.end();++i )
	{
		if ( !str_cmp( person, (*i) ) )
		{
			return true;
		}
	}
	return false;
}

int special( Character *ch, char *cmd, char *arg )
{
	Object * i;
	Character *k;
	int j;

	/* special in room? */
	if ( GET_ROOM_SPEC( ch->in_room ) != NULL )
	{
		if ( GET_ROOM_SPEC( ch->in_room ) ( ch, ch->in_room, cmd, arg ) )
		{
			return 1;
		}
	}
	/* special in equipment list? */
	for ( j = 0; j < NUM_WEARS;++j )
	{
		if ( GET_EQ( ch, j ) && GET_OBJ_SPEC( GET_EQ( ch, j ) ) != NULL )
		{
			if ( GET_OBJ_SPEC( GET_EQ( ch, j ) ) ( ch, GET_EQ( ch, j ), cmd, arg ) )
			{
				return 1;
			}
		}
	}
	/* special in inventory? */
	for ( i = ch->carrying; i; i = i->next_content )
	{
		if ( ( i ) && GET_OBJ_SPEC( (i) ))
		{
			if ( GET_OBJ_SPEC( i ) ( ch, i, cmd, arg ) )
				return 1;
		}
	}

	/* special in mobile present? */
	for ( k = ch->in_room->people; k; k = k->next_in_room )
	{
		if ( GET_MOB_SPEC( k ) != NULL )
		{
			if ( GET_MOB_SPEC( k ) ( ch, k, cmd, arg ) )
				return 1;
		}
		if ( MOB_FLAGGED( k, MOB_INNKEEPER ) && receptionist( ch, k, cmd, arg ) )
			return 1;
		if ( MOB_FLAGGED( k, MOB_BANKER ) && bank( ch, k, cmd, arg ) )
			return 1;
		if ( (MOB_FLAGGED( k, MOB_TRAINER ) || MOB_FLAGGED( k, MOB_TRADE_TRAINER )) && guild( ch, k, cmd, arg ) )
			return 1;
		if ( MOB_FLAGGED( k, MOB_DEALER ) && PokerDealer( ch, k, cmd, arg ) )
			return 1;
	}
	/* special in object present? */
	for ( i = ch->in_room->contents; i; i = i->next_content )
	{
		if ( GET_OBJ_SPEC( i ) != NULL )
		{
			if ( GET_OBJ_SPEC( i ) ( ch, i, cmd, arg ) )
				return 1;
		}
	}
	return 0;
}

/* *************************************************************************
*  Stuff for controlling the non-playing sockets (get name, pwd etc)       *
************************************************************************* */

int _parse_name( const char *arg, char *name )
{
	int i;

	/* skip whitespaces */
	for ( ; isspace( *arg ); arg++ );

	for ( i = 0; ( *name = *arg ); arg++, i++, name++ )
		if ( !isalpha( *arg ) )
			return 1;

	if ( !i )
		return 1;

	return 0;
}

const int RECON = 1;
const int USURP = 2;
const int UNSWITCH = 3;

/*
 * XXX: Make immortals 'return' instead of being disconnected when switched
 *      into person returns.  This function seems a bit over-extended too.
 */
int performDupeCheck( Descriptor *d )
{
	Descriptor * k, *next_k;
	Character *target = NULL, *ch, *next_ch;
	int mode = 0;

	int id = d->character->player.idnum;

	/*
	 * Now that this descriptor has successfully logged in, disconnect all
	 * other descriptors controlling a character with the same ID number.
	 */

	for ( k = descriptor_list; k; k = next_k )
	{
		next_k = k->next;

		if ( k == d )
			continue;

		if ( k->original && ( k->original->player.idnum == id ) )
		{    /* switched char */
			k->send( "\r\nMultiple login detected -- disconnecting.\r\n" );
			STATE( k ) = CON_CLOSE;

			if ( !target )
			{
				target = k->original;
				mode = UNSWITCH;
			}

			if ( k->character )
				k->character->desc = NULL;

			k->character = NULL;
			k->original = NULL;
		}

		else if ( k->character && ( k->character->player.idnum == id ) )
		{
			if ( !target && STATE( k ) == CON_PLAYING )
			{
				k->send( "\r\nThis body has been usurped!\r\n" );
				target = k->character;
				mode = USURP;
			}

			k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
			k->send( "\r\nMultiple login detected -- disconnecting.\r\n" );
			STATE( k ) = CON_CLOSE;
		}
	}

	/*
	 * now, go through the character list, deleting all characters that
	 * are not already marked for deletion from the above step (i.e., in the
	 * CON_HANGUP state), and have not already been selected as a target for
	 * switching into.  In addition, if we haven't already found a target,
	 * choose one if one is available (while still deleting the other
	 * duplicates, though theoretically none should be able to exist).
	 */

	for ( ch = character_list; ch; ch = next_ch )
	{
		next_ch = ch->next;

		if ( IS_NPC( ch ) )
			continue;

		if ( ch->player.idnum != id )
			continue;

		/* ignore chars with descriptors (already handled by above step) */
		if ( ch->desc )
			continue;

		/* don't extract the target char we've found one already */
		if ( ch == target )
			continue;

		/* we don't already have a target and found a candidate for switching */
		if ( !target )
		{
			target = ch;
			mode = RECON;
			continue;
		}

		/* we've found a duplicate - blow him away, dumping his eq in limbo. */
		if ( ch->in_room )
			ch->RemoveFromRoom();

		ch->Extract(UserLogoutType::notRealLogout);
	}

	/* no target for swicthing into was found - allow login to continue */
	if ( !target )
		return 0;

	/* Okay, we've found a target.  Connect d to target. */
	delete d->character; /* get rid of the old char */
	d->character = target;
	d->character->desc = d;
	d->original = NULL;
	d->character->player.timer = 0;
	REMOVE_BIT( PLR_FLAGS( d->character ), Q_BIT(PLR_MAILING) );
	REMOVE_BIT( PLR_FLAGS( d->character ), Q_BIT(PLR_WRITING) );
	STATE( d ) = CON_PLAYING;

	switch ( mode )
	{

	case RECON:
		d->loggedIn = true;
		d->send( "Reconnecting.\r\n" );
		Act( "$n has reconnected.", TRUE, d->character, 0, 0, TO_ROOM );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE, "%s has reconnected.",
			GET_NAME( d->character ) );
		SwitchManager::GetManager().RemoveSwitchByName( d->character->player.name );
		break;

	case USURP:
		d->loggedIn = true;
		d->send( "You take over your own body, already in use!\r\n" );
		Act( "$n suddenly keels over in pain, surrounded by a white aura...\r\n"
			"$n's body has been taken over by a new spirit!",
			TRUE, d->character, 0, 0, TO_ROOM );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE,
			"%s has re-logged in ... disconnecting old socket.", GET_NAME( d->character ) );
		SwitchManager::GetManager().RemoveSwitchByName( d->character->player.name );
		break;

	case UNSWITCH:
		d->send( "Reconnecting to unswitched char." );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE, "%s has reconnected.", GET_NAME( d->character ) );
		break;
	}

	return 1;
}

void Character::StatSelectionMenu( bool notify )
{
	if ( notify )
	{
		this->send( "Now it is time for you to setup your character's status points. To increase a status point,\r\n"
		            "simply type the first letter of the stat type you wish to increase, followed by a '+' sign.\r\n"
		            "So for example, if you wish to increase your strength, you would type 's+'. The same goes for\r\n"
		            "decreasing, except you use a '-' sign('s-'). When you have the stats you desire, simply type 'done'\r\n\n" );
	}
	this->send( "Current Stats:   Str: %d[%d], Int: %d[%d], Wis: %d[%d], Dex: %d[%d], Con: %d[%d]\r\n",
                (int)this->real_abils.str, this->CostPerStat( STAT_STR ), (int)this->real_abils.intel,
                 this->CostPerStat( STAT_INT ), (int)this->real_abils.wis, this->CostPerStat( STAT_WIS ),
                (int)this->real_abils.dex, this->CostPerStat( STAT_DEX ), (int)this->real_abils.con,
	            this->CostPerStat( STAT_CON ) );
	this->send( "\r\n%d points remaining. Type 'done' when you are finished.\r\n", this->Stats );
}

// deal with newcomers and other non-playing sockets
void Descriptor::nanny( char* arg )
{
	char buf[ 128 ];
	int load_result, cost = 0;
	char tmp_name[ MAX_INPUT_LENGTH ];
	Switch* sw;
	Room *load_room;

	skip_spaces( &arg );

	switch ( STATE( this ) )
	{
	/*. OLC states .*/
	case CON_OEDIT:
		oedit_parse( this, arg );
		break;
	case CON_REDIT:
		redit_parse( this, arg );
		break;
	case CON_ZEDIT:
		zedit_parse( this, arg );
		break;
	case CON_MEDIT:
		medit_parse( this, arg );
		break;
	case CON_SEDIT:
		sedit_parse( this, arg );
		break;
	case CON_AEDIT:
		aedit_parse( this, arg );
		break;
	case CON_KEDIT:
		kedit_parse( this, arg );
		break;
	case CON_CEDIT:
		cedit_parse( this, arg );
		break;
	case CON_CLEDIT:
		CleditParse( this, arg );
		break;
	case CON_WEDIT:
		WeditParse( this, arg );
		break;
	case CON_SGEDIT:
		StateditParse( this, arg );
		break;
	case CON_AUCTION:
		AuctionParse( this, arg );
		break;
	case CON_EMAIL:
		parseUserEmailAddressEditor( this, arg );
		break;
	case CON_JEDIT:
		JeditParse( this, arg );
		break;
	/*. End of OLC states .*/
	case CON_GATEWAY:
	{
		try {
			std::vector<std::string> vArgs = StringUtil::getArgVector(arg);

			std::string command = vArgs.at(0);

			if(command == "Session")
			{
				std::string sessionKey = vArgs.at(1);

				std::list<PendingSession>::iterator iter = pendingSessions.begin();

				bool pendingSessionFound = false;

				while(iter != pendingSessions.end()) {

					PendingSession pendingSession = (*iter);

					if(pendingSession.sessionKey == sessionKey) {

						strcpy(this->host, pendingSession.host.c_str());
						this->setGatewayDescriptorType(pendingSession.gatewayDescriptorType);
						pendingSessions.erase(iter);
						pendingSessionFound = true;
						this->session = sessionKey;

						if(pendingSession.gatewayDescriptorType == GatewayDescriptorType::websocket)
						{
							STATE(this) = CON_WEBSOCKET_LOGIN;
							sendWebSocketDisplaySignInLightboxMessage();
						}
						else
						{
							this->sendInstant(startup);
							this->sendInstant("By what name do you wish to be known? ");
							STATE(this) = CON_GET_NAME;
						}

						break;
					}

					++iter;
				}

				if(pendingSessionFound == false) {
					//If we get to this point it means the descriptor supplied a session key that
					//was invalid. The gateway either screwed up or we have a random person
					//connecting to the MUD itself trying to validate themselves.

					this->disconnect();
				}
			}
		}
		catch(std::out_of_range e) {

			//Error...
		}

		break;
	}
	case CON_GET_NAME:    		/* wait for input of name */

		if ( !*arg )
			STATE( this ) = CON_CLOSE;
		else
		{
			if ((_parse_name(arg, tmp_name)) || strlen(tmp_name) < MIN_NAME_LENGTH ||
				strlen(tmp_name) > MAX_NAME_LENGTH || !BanManager::GetManager().IsValidName(tmp_name) ||
			fill_word( strcpy( buf, tmp_name ) ) || reserved_word( buf ) )
			{
				this->send( "Invalid name, please try another.\r\n"
				         "Name: " );
				return ;
			}
			strcpy(tmp_name, StringUtil::cap(StringUtil::allLower(arg)));//Format the name correctly...

			if( Conf->play.switch_restriction && SwitchManager::GetManager().WillBeMultiplaying( this->host, tmp_name ) )
			{
				this->send("You are already logged into another character. You must log out before switching.\r\n");
				return;
			}
			//Below, we check to see if the player has waited the required time.
			if( Conf->play.switch_restriction &&
			(
				(sw = SwitchManager::GetManager().GetGreatestSwitch( this->host, tmp_name )) != NULL
	//			(sw = SwitchManager::GetManager().GetSwitchByIP( this->host )) != NULL		||
	//			(sw = SwitchManager::GetManager().GetSwitchByKnownAlt(tmp_name)) != NULL
			))
			{
				if( !SwitchManager::GetManager().HasWaitedLongEnough(tmp_name,host,sw) )
				{
					Time Remainder = SwitchManager::GetManager().TimeRemaining(tmp_name,host,sw);
					this->send("You must wait %d minutes, %d seconds before you may log into another character.\r\n",
						(int)Remainder.Minutes(), (int)(Remainder.Seconds() % 60));
					return;
				}
				//This player was on the switch list, but has waited the required time.
				//SwitchManager::GetManager().RemoveSwitchByIP( this->host );
			}
			Character *loadedCharacter = NULL;
			if ( playerExists(tmp_name) && (loadedCharacter = CharacterUtil::loadCharacter(tmp_name)) != NULL )
			{
				if ( PLR_FLAGGED(loadedCharacter, PLR_DELETED) )
				{
					// We get a false positive from the original deleted character.
					delete loadedCharacter;
					if(!this->character)
						this->character = new Character(CharPlayer);
					// Check for multiple creations...

					if ( !BanManager::GetManager().IsValidName( tmp_name ) )
					{
						this->send( "Invalid name, please try another.\r\nName:\r\n" );
						return ;
					}
					this->character->desc = this;

					this->character->player.name = StringUtil::cap( tmp_name );
					this->send( "Did I get that right, %s (Y/N)? \r\n", tmp_name );
					STATE( this ) = CON_NAME_CNFRM;
				}
				else
				{
					if(this->character)
						delete this->character;
					this->character = loadedCharacter;
					// undo it just in case they are set
					REMOVE_BIT( PLR_FLAGS( this->character ), Q_BIT(PLR_WRITING) );
					REMOVE_BIT( PLR_FLAGS( this->character ), Q_BIT(PLR_MAILING) );
					REMOVE_BIT( PLR_FLAGS( this->character ), Q_BIT(PLR_CRYO) );
					this->send( "Welcome back, %s. Please enter your password:", GET_NAME( this->character ) );
					this->idle_tics = 0;
					this->character->desc = this;
					STATE( this ) = CON_PASSWORD;
				}
			}
			else
			{
				/* player unknown -- make new character */
				/* Check for multiple creations of a character. */
				if ( !BanManager::GetManager().IsValidName( tmp_name ) )
				{
					this->send( "Invalid name, please try another.\r\nName: " );
					return ;
				}

				if(!this->character)
					this->character = new Character(CharPlayer);
				this->character->desc = this;
				this->character->player.name = tmp_name;
				this->send( "Are you certain, %s, that you wish to use this name? (Y/N) ", tmp_name );
				STATE( this ) = CON_NAME_CNFRM;
			}
		}
		break;

	case CON_NAME_CNFRM:    		/* wait for conf. of new name    */
		if ( UPPER( *arg ) == 'Y' )
		{
			if ( BanManager::GetManager().IsBanned( this->host ) >= BAN_NEW )
			{
				MudLog( NRM, LVL_APPR, TRUE, "Request for new char %s denied from [%s] (siteban)",
			        GET_NAME( this->character ), this->host );
				this->send( "Sorry, new characters are not allowed from your site!\r\n" );
				STATE( this ) = CON_CLOSE;
				return ;
			}

			if ( circle_restrict )
			{
				this->send( "Sorry, new players can't be created at the moment.\r\n" );
				MudLog( NRM, LVL_APPR, TRUE, "Request for new char %s denied from [%s] (wizlock)",
			        GET_NAME( this->character ), this->host );
				STATE( this ) = CON_CLOSE;
				return ;
				}
      
			this->send( "Please enter a password: ", GET_NAME( this->character ) );
			STATE( this ) = CON_NEWPASSWD;
		}
		else if ( *arg == 'n' || *arg == 'N' )
		{
			this->send( "Okay, what IS it, then? " );
			this->character->player.name.erase();
			STATE( this ) = CON_GET_NAME;
		}
		else
		{
			this->send( "Please type Yes or No: " );
		}
		break;
	case CON_PASSWORD:    		/* get pwd for known player      */

		this->echoOn();		/* turn echo back on */

		if ( !*arg )
			STATE( this ) = CON_CLOSE;
		else
		{
			if(!this->character->passwordMatches(arg))
			{
				MudLog( BRF, LVL_GOD, TRUE, "Bad PW: %s [%s]", GET_NAME( this->character ), this->host );

				++this->character->PlayerData->bad_pws;
				this->character->basicSave();
				if ( ++( this->bad_pws ) >= max_bad_pws )
				{	/* 3 strikes and you're out. */
					this->send( "Wrong password... disconnecting.\r\n" );
					STATE( this ) = CON_CLOSE;
				}
				else
				{
					this->send( "Wrong password.\r\nTry again: " );
				}

				return ;
			}

			if(this->getGatewayDescriptorType() == GatewayDescriptorType::websocket)
			{
				std::list<UserMacro*> userMacros = CharacterUtil::getUserMacros(gameDatabase, this->character->getUserId());
				this->sendWebSocketUsernameCommand(GET_NAME(this->character), userMacros);
				CharacterUtil::freeUserMacros(userMacros);
			}

			/* Password was correct. */
			//We need to convert the password entered to MD5.
			if( !this->character->PasswordUpdated() )
			{
				this->character->player.passwd = MD5::getHashFromString(arg).c_str();
				this->character->PasswordUpdated( true );
			}
			load_result = this->character->PlayerData->bad_pws;
			this->character->PlayerData->bad_pws = 0;
			this->bad_pws = 0;

			if ( BanManager::GetManager().IsBanned( this->host ) == BAN_SELECT &&
		        !PLR_FLAGGED( this->character, PLR_SITEOK ) )
			{

				this->send( "Sorry, this char has not been cleared for login from your site!\r\n" );
				STATE( this ) = CON_CLOSE;
				MudLog( NRM, LVL_GOD, TRUE,
					"Connection attempt for %s denied from %s", GET_NAME( this->character ), this->host );
				return;
			}
			if ( GET_LEVEL( this->character ) < circle_restrict )
			{
				this->send( "The game is temporarily restricted.. try again later.\r\n" );
				STATE( this ) = CON_CLOSE;
				MudLog( NRM, LVL_GOD, TRUE, "Request for login denied for %s [%s] (wizlock)",
			        GET_NAME( this->character ), this->host );
				return ;
			}

			/* check to make sure that no other copies of this player are logged in */
			if ( performDupeCheck( this ) )
				return;

			MudLog( BRF, MAX( LVL_GRGOD, GET_INVIS_LEV( this->character ) ), TRUE, "%s [%s] has connected.", GET_NAME( this->character ), this->host );

			if ( GET_LEVEL( this->character ) >= LVL_IMMORT )
				this->send( imotd );
			else
				this->send( motd );
			if ( load_result )
			{
				this->send( "\r\n\r\n\007\007\007"
			         "%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
			         COLOR_RED( this->character, CL_SPARSE ), load_result,
			         ( load_result > 1 ) ? "S" : "", COLOR_NORMAL( this->character, CL_SPARSE ) );
				this->character->PlayerData->bad_pws = 0;
			}

			STATE( this ) = CON_MENU;
			goto menu;
		}

	case CON_NEWPASSWD:
	case CON_CHPWD_GETNEW:
	{
		size_t passwordLength = strlen(arg);
		if (!*arg || passwordLength > MAX_PWD_LENGTH || passwordLength < MIN_PWD_LENGTH || !str_cmp(arg, GET_NAME(this->character)))
		{
			this->send("\r\nIllegal password.\r\nTry again:");
			return;
		}
		
		this->character->player.passwd = MD5::getHashFromString(arg);
		this->send("\r\nPlease verify your password: ");

		if (STATE(this) == CON_NEWPASSWD)
			STATE(this) = CON_CNFPASSWD;
		else
			STATE(this) = CON_CHPWD_VRFY;
		break;
	}
	case CON_CNFPASSWD:
	case CON_CHPWD_VRFY:
		if( this->character->player.passwd.compare( MD5::getHashFromString( arg ) ) )
		{
			this->send( "\r\nPasswords don't match... start over.\r\n" );
			this->send( "Password: " );

			if ( STATE( this ) == CON_CNFPASSWD )
				STATE( this ) = CON_NEWPASSWD;
			else
				STATE( this ) = CON_CHPWD_GETNEW;
			return ;
		}
		this->character->PasswordUpdated( true );//This will inform future logins that the PW is using m5, not crypt

		this->echoOn();
		if ( STATE( this ) == CON_CNFPASSWD )
		{
			this->send("Please enter your email address: \r\n");
			STATE( this ) = CON_REG_EMAIL;
		}
		else
		{
			this->echoOn();
			this->send( "\r\nDone.\r\n" );
			STATE( this ) = CON_MENU;
		}

		break;

	case CON_REG_EMAIL:

		if(!MiscUtil::isValidEmailAddress(arg))
		{
			this->send("You entered an invalid email address. Please try again: \r\n");
			break;
		}

		this->setEmailAddress(arg);
		this->send( "What is your gender: [M]ale or [F]emale? " );
		STATE( this ) = CON_QSEX;
		break;

	case CON_QSEX:    		/* query sex of new user         */
		switch ( *arg )
		{
		case 'm':
		case 'M':
			this->character->player.sex = SEX_MALE;
			break;
		case 'f':
		case 'F':
			this->character->player.sex = SEX_FEMALE;
			break;
		default:
			this->send( "Invalid sex...\r\nTry again: " );
			return ;
		}
		this->send( race_menu );
		this->send( "Race:\r\n" );
		STATE( this ) = CON_QRACE;
		break;

	case CON_QRACE:
		switch ( *arg )
		{
		case 'h':
		case 'H':
			GET_RACE( this->character ) = RACE_HUMAN;
			break;
		case 't':
		case 'T':
			GET_RACE( this->character ) = RACE_TROLLOC;
			break;
	//	case 'a':
	//	case 'A':
	//		GET_RACE(this->character) = RACE_AIEL;
	//		break;
		default:
			this->send( "\r\nThat's not a race.\r\nRace: " );
			return ;
		}

		if ( GET_RACE( this->character ) == RACE_HUMAN || GET_RACE( this->character ) == RACE_AIEL )
			this->send( human_class_menu );
		else
			this->send( other_class_menu );

		this->send( "Class:\r\n" );
		STATE( this ) = CON_QCLASS;
		break;

	case CON_QCLASS:
		switch ( *arg )
		{
#if defined(WARRIOR_SELECT)
		case 'w':
		case 'W':
			GET_CLASS( this->character ) = CLASS_WARRIOR;
			break;
#endif
#if defined(RANGER_SELECT)
		case 'r':
		case 'R':
			GET_CLASS( this->character ) = CLASS_RANGER;
			break;
#endif
#if defined(THIEF_SELECT)
		case 't':
		case 'T':
			GET_CLASS( this->character ) = CLASS_THIEF;
			break;
#endif
#if defined(CHANNELER_SELECT)
		case 'c':
		case 'C':
			if ( GET_RACE( this->character ) == RACE_HUMAN || GET_RACE( this->character ) == RACE_AIEL )
			{
				GET_CLASS( this->character ) = CLASS_CHANNELER;
				break;
			}
			else
			{
				this->send( "That is not a class!\r\nClass:" );
				return ;
			}
		case 'd':
		case 'D':
			if( GET_RACE( this->character ) == RACE_TROLLOC )
			{
				GET_CLASS( this->character ) = CLASS_DREADGUARD;
				break;
			}
			else
			{
				this->send( "That is not a class!\r\nClass:" );
				return;
			}
#endif
		default:
			GET_CLASS( this->character ) = CLASS_UNDEFINED;
			this->send( "That is not a class!\r\nClass:" );
			return ;
		}
		STATE( this ) = CON_STAT_OPTION;

		MudLog( NRM, MAX( LVL_APPR, GET_INVIS_LEV( this->character ) ), TRUE,
	        "%s [%s] new player.", GET_NAME( this->character ), this->host );

		this->send(
			"Would you like to choose your stats now, or enter the game and have your stats be randomly selected?\r\n"
			"  A) Choose Stats Now\r\n"
			"  B) Enter Game / Random Selection\r\n"
			);
		return ;

	case CON_STAT_OPTION:
		switch(toupper( *arg ))
		{
		case 'A':
			this->character->Stats = this->character->NumOfStats();
			this->character->Init();
			SET_BIT_AR(PRF_FLAGS(this->character), PRF_STATTED);
			this->character->StatSelectionMenu( true );
			STATE( this ) = CON_STATEDIT;
			break;
		case 'B':
			this->character->Init();
			StatManager::GetManager().RollStats(this->character);
			this->newbieMenuFinish();
			this->completeEnterGame();
			break;
		default:
			this->send("Invalid option: Choose 'A' or 'B' : ");
			return;
		}
		break;
	case CON_STATEDIT:
	{
		if ( strlen( arg ) < 2 )
		{
			this->send( "You must either type 'done' when you are finished, or choose the first letter of\r\n"
		         "the stat that you wish to increase or decrease followed by either a '+' to increase, or a\r\n"
		         "'-' to decrease.\r\n" );
			return ;
		}
		else if ( !str_cmp( arg, "done" ) )
		{
			this->newbieMenuFinish();
			this->completeEnterGame();
			return;
		}

		sbyte* stat;
		int which = 0;
		switch ( toupper( *arg ) )
		{
		case 'S':
			which = STAT_STR;
			stat = &this->character->real_abils.str;
			break;
		case 'I':
			which = STAT_INT;
			stat = &this->character->real_abils.intel;
			break;
		case 'W':
			which = STAT_WIS;
			stat = &this->character->real_abils.wis;
			break;
		case 'D':
			which = STAT_DEX;
			stat = &this->character->real_abils.dex;
			break;
		case 'C':
			which = STAT_CON;
			stat = &this->character->real_abils.con;
			break;
		default:
			this->send( "You inserted an invalid STAT type to increase. You must choose either S, I, W, D, or C.\r\n"
				"Try again: " );
			return ;
		}

		cost = this->character->CostPerStat( which );

		if ( *( arg + 1 ) == '+' )
		{
			if ( (*stat) < this->character->StatMaximum( which ) &&
	        this->character->Stats >= this->character->CostPerStat( which ) )
			{
				*stat += 1;
				this->character->Stats -= this->character->CostPerStat( which );
			}
		}
		else if ( *( arg + 1 ) == '-' )
		{
			if ( (*stat) > this->character->StatMinimum( which ) )
			{
				*stat -= 1;
				this->character->Stats += this->character->CostPerStat( which );
			}
		}
		else
		{
			this->send( "The stat you choose must be followed by either a '+' or a '-', depending on whether you wish to\r\n"
			         "increase the status point or decrease.\r\nTry again: " );
			return ;
		}
		this->character->StatSelectionMenu( false );
		return ;
	}
	case CON_NEWBMSG:
		STATE(this) = CON_MENU;
		goto menu;
	case CON_MENU:    		/* get selection from main menu  */
	{
		menu:
		completeEnterGame();
		break;
	}
	default:
		Log( "SYSERR: nanny: illegal state of con'ness (%d) for '%s'; closing connection.",
		     STATE( this ), this->character ? GET_NAME( this->character ) : "<unknown>" );
		STATE( this ) = CON_DISCONNECT;	/* Safest to do. */
		break;
	}
}
