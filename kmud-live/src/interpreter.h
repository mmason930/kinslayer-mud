/* ************************************************************************
*   File: interpreter.h                                 Part of CircleMUD *
*  Usage: header file: public procs, macro defs, subcommand defines       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef K_INTERPRETER_H
#define K_INTERPRETER_H

class Character;
class Object;
class Room;

#define ACMD(name)  \
   void name(Character *ch, char *argument, int cmd, int subcmd)

ACMD( do_move );

#define CMD_NAME (cmd)
#define CMD_IS(ch, cmd_name)	(!strcmp(complete_cmd_info[ch->cmd_no].command, cmd_name))
#define IS_MOVE(cmdnum) (complete_cmd_info[cmdnum].command_pointer == do_move)

void	CommandInterpreter( Character *ch, char *argument );
int		search_block( const char *arg, const char **listy, int exact );
int		reserved_word(const char *argument);
int		_parse_name(const char *arg, char *name);
char	lower( char c );
char	*OneArgument( char *argument, char *first_arg, bool conv_case=true );
char	*OneWord( char *argument, char *first_arg );
char	*AnyOneArg( char *argument, char *first_arg );
char	*TwoArguments( char *argument, char *first_arg, char *second_arg, bool conv_case=true );
int		fill_word( const char *argument );
void	HalfChop( char *string, char *arg1, char *arg2 );
int		IsAbbrev( const char *arg1, const char *arg2 );
int		IsNumber( const char *str );
int		FindCommand( const char *command );
std::string	GetCommand( char *argument );
void	skip_spaces( char **str );
char	*delete_doubledollar( char *str );
char *PERS(Character *ch, Character *vict, const char *bgColor=0);

/* for compatibility with 2.20: */
#define argument_interpreter(a, b, c) TwoArguments(a, b, c)

class CommandInfo;

class CommandManager
{
private:
	static CommandManager *Self;
	std::vector< CommandInfo * > MyCommands;

	CommandManager();
	~CommandManager();
public:
	CommandManager &GetManager();

	void AppendCommand( const std::string cmd, const std::string sort, void (*command_pointer)(Character *,char *,int,int),
						const byte mp, const byte ml, const sh_int subcmd, const float timer, const byte vml);
	void BootCommands();
	void SaveCommands();
};

class CommandInfo
{
public:
	~CommandInfo();
	//CommandInfo();

	std::string command;
	std::string sort_as;
	byte minimum_position;
	void	( *command_pointer )
	( Character *ch, char * argument, int cmd, int subcmd );
	byte minimum_level;
	sh_int	subcmd;
	float timer;
	byte view_minimum_level;
};

extern std::map<int, std::pair<std::string,int> > DisabledCommands;

class CommManager
{
private:
	static CommManager *Self;
	CommManager();
	~CommManager();

public:
	static CommManager &GetManager();

	void Free();
	const int SaveComm( const std::string &Type, const std::string &Message, Character *sender,
		const int room_vnum, Character *recipient=0);
	const int SaveComm( const std::string &Type, const std::string &Message, Character *sender,
		const int room_vnum, const int rid, const int invis_level);
private:
	const int SaveComm( const std::string &Type, const std::string &Message, const char sType, 
		const int sid, const char rType, const int rid, const int invis_level, const int room_vnum);
};

/*
 * Necessary for CMD_IS macro.  Borland needs the structure defined first
 * so it has been moved down here.
 */
#ifndef __INTERPRETER_C__
extern class CommandInfo *complete_cmd_info;
#endif

const int ALIAS_SIMPLE = 0;
const int ALIAS_COMPLEX = 1;

#define ALIAS_SEP_CHAR		';'
#define ALIAS_VAR_CHAR		'$'
#define ALIAS_GLOB_CHAR		'*'

/*
 * SUBCOMMANDS
 *   You can define these however you want to, and the definitions of the
 *   subcommands are independent from function to function.
 */

/* do_change */
const int SCMD_TITLE = 0;

/* Embrace/Seize */
const int SCMD_EMBRACE = 1;
const int SCMD_SEIZE = 2;

/* directions */
const int SCMD_NORTH = 1;
const int SCMD_EAST = 2;
const int SCMD_SOUTH = 3;
const int SCMD_WEST = 4;
const int SCMD_UP = 5;
const int SCMD_DOWN = 6;

/* do_gen_ps */
const int SCMD_CREDITS = 0;
const int SCMD_WIZLIST = 1;
const int SCMD_VERSION = 2;
const int SCMD_MOTD = 3;
const int SCMD_IMOTD = 4;
const int SCMD_CLEAR = 5;
const int SCMD_WHOAMI = 6;

/* do_gen_tog */
const int SCMD_NOHASSLE = 0;
const int SCMD_BRIEF = 1;
const int SCMD_COMPACT = 2;
const int SCMD_NOTELL = 3;
const int SCMD_NONARRATE = 4;
const int SCMD_NOCHAT = 5;
const int SCMD_NOSHOUT = 6;
const int SCMD_NOYELL = 7;
const int SCMD_NOWIZ = 8;
const int SCMD_ROOMFLAGS = 9;
const int SCMD_NOREPEAT = 10;
const int SCMD_HOLYLIGHT = 11;
const int SCMD_AUTOEXIT = 12;
const int SCMD_BUILDWALK = 13;
const int SCMD_NOTIPS = 14;
const int SCMD_AUTOSCAN = 15;   //RHOLLOR 05.15.09
const int SCMD_NOSPEAKS = 16;
const int SCMD_NOGLOBAL = 17;

/* do_wizutil */
const int SCMD_REROLL = 0;
const int SCMD_PARDON = 1;
const int SCMD_NOTITLE = 2;
const int SCMD_SQUELCH = 3;
const int SCMD_FREEZE = 4;
const int SCMD_THAW = 5;
const int SCMD_UNAFFECT = 6;
const int SCMD_ZONE_BAN = 7;

/* do_spec_com */
const int SCMD_WHISPER = 0;
const int SCMD_ASK = 1;

/* do_gen_com */
const int SCMD_YELL = 0;
const int SCMD_SHOUT = 1;
const int SCMD_CHAT = 2;
const int SCMD_NARRATE = 3;
const int SCMD_GLOBAL = 4;

/* do_shutdown */
const int SCMD_SHUTDOW = 0;
const int SCMD_SHUTDOWN = 1;

/* do_quit */
const int SCMD_QUI = 0;
const int SCMD_QUIT = 1;

/* do_date */
const int SCMD_DATE = 0;
const int SCMD_UPTIME = 1;

/* do_commands */
const int SCMD_COMMANDS = 0;
const int SCMD_SOCIALS = 1;
const int SCMD_WIZHELP = 2;

/* do_drop */
const int SCMD_DROP = 0;
const int SCMD_JUNK = 1;
const int SCMD_DONATE = 2;

/* do_look */
const int SCMD_LOOK = 0;
const int SCMD_READ = 1;

/* do_qcomm */
const int SCMD_QSAY = 0;
const int SCMD_QECHO = 1;

/* do_pour */
const int SCMD_POUR = 0;
const int SCMD_FILL = 1;

/* do_poof */
const int SCMD_POOFIN = 0;
const int SCMD_POOFOUT = 1;

/* do_hit */
const int SCMD_HIT = 0;
const int SCMD_MURDER = 1;

/* do_eat */
const int SCMD_EAT = 0;
const int SCMD_TASTE = 1;
const int SCMD_DRINK = 2;
const int SCMD_SIP = 3;

/* do_use */
const int SCMD_USE = 0;
const int SCMD_QUAFF = 1;

/* do_echo */
const int SCMD_ECHO = 0;

/* do_gen_door */
const int SCMD_OPEN = 0;
const int SCMD_CLOSE = 1;
const int SCMD_UNLOCK = 2;
const int SCMD_LOCK = 3;
const int SCMD_PICK = 4;

/*. do_olc .*/
const int SCMD_OLC_REDIT = 0;
const int SCMD_OLC_OEDIT = 1;
const int SCMD_OLC_ZEDIT = 2;
const int SCMD_OLC_MEDIT = 3;
const int SCMD_OLC_SEDIT = 4;
const int SCMD_OLC_AEDIT = 5;
const int SCMD_OLC_KEDIT = 6;
const int SCMD_OLC_SAVEINFO = 7;
const int SCMD_OLC_SGEDIT = 8;
const int SCMD_OLC_EMAILS = 9;

/* do_charge */
const int SCMD_CHARGE = 0;
const int SCMD_SKEWER = 1;

/* do butcher */
const int SCMD_BUTCHER = 0;
const int SCMD_SKIN = 1;

#endif
