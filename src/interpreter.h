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

#include <functional>

#include "commands/infrastructure/CommandUtil.h"

#define DEFINE_COMMAND	[](Character *ch, char *argument, int cmd, int subcmd)
#define CMD_IS(ch, cmd_name)	(!strcmp(CommandUtil::get()->getCommandByIndex(ch->cmd_no)->command, cmd_name))

int		search_block( const char *arg, const char **listy, int exact );
int		reserved_word(const char *argument);
int		_parse_name(const char *arg, char *name);
char	*OneArgument( char *argument, char *first_arg, bool conv_case=true );
char	*OneWord( char *argument, char *first_arg );
char	*AnyOneArg( char *argument, char *first_arg );
char	*TwoArguments( char *argument, char *first_arg, char *second_arg, bool conv_case=true );
int		fill_word( const char *argument );
void	HalfChop( char *string, char *arg1, char *arg2 );
int		IsAbbrev( const char *arg1, const char *arg2 );
int		IsNumber( const char *str );
void	skip_spaces( char **str );
char	*delete_doubledollar( char *str );
char	*PERS(Character *ch, Character *vict, const char *bgColor=0);


class CommManager
{
private:
	static CommManager *Self;
	CommManager();
	~CommManager();
public:
	static CommManager &GetManager();
		
	void Free();
	int SaveComm( const std::string &Type, const std::string &Message, Character *sender,
		const int room_vnum, Character *recipient=0);
	int SaveComm( const std::string &Type, const std::string &Message, Character *sender,
		const int room_vnum, const int rid, const int invis_level);
private:
	int SaveComm( const std::string &Type, const std::string &Message, const char sType,
		const int sid, const char rType, const int rid, const int invis_level, const int room_vnum);
};

extern std::map<int, std::pair<std::string,int> > DisabledCommands;

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

//Command function declarations.
extern CommandHandler  do_action ;
extern CommandHandler  do_advance ;
extern CommandHandler  do_alias ;
extern CommandHandler  do_assist ;
extern CommandHandler  do_at ;
extern CommandHandler  do_auction ;
extern CommandHandler  do_award ;
extern CommandHandler  do_backstab ;
extern CommandHandler  do_ban ;
extern CommandHandler  do_bash ;
extern CommandHandler  do_butcher ;
extern CommandHandler  do_break ;
extern CommandHandler  do_scalp ;
extern CommandHandler  do_call ;
extern CommandHandler  do_cast ;
extern CommandHandler  do_cedit ;
extern CommandHandler  do_charge ;
extern CommandHandler  do_choke ;
extern CommandHandler  do_clan ;
extern CommandHandler  do_cledit ;
extern CommandHandler  do_color ;
extern CommandHandler  do_commands ;
extern CommandHandler  do_compel ;
extern CommandHandler  do_consider ;
extern CommandHandler  do_copy ;
extern CommandHandler  do_countdown ;
extern CommandHandler  do_council ;
extern CommandHandler  do_credits ;
extern CommandHandler  do_diceroll ;
extern CommandHandler  do_dig ;
extern CommandHandler  do_disable ;
extern CommandHandler  do_dismount ;
extern CommandHandler  do_demote ;
extern CommandHandler  do_drink ;
extern CommandHandler  do_date ;
extern CommandHandler  do_dc ;
extern CommandHandler  do_declan ;
extern CommandHandler  do_deny ;
extern CommandHandler  do_diagnose ;
extern CommandHandler  do_display ;
extern CommandHandler  do_draw ;
extern CommandHandler  do_drop ;
extern CommandHandler  do_eat ;
extern CommandHandler  do_echo ;
extern CommandHandler  do_effuse ;
extern CommandHandler  do_email ;
extern CommandHandler  do_enable ;
extern CommandHandler  do_enter ;
extern CommandHandler  do_equipment ;
extern CommandHandler  do_examine ;
extern CommandHandler  do_exit ;
extern CommandHandler  do_exits ;
extern CommandHandler  do_extra ;
extern CommandHandler  do_fade ;
extern CommandHandler  do_flee ;
extern CommandHandler  do_find ;
extern CommandHandler  do_follow ;
extern CommandHandler  do_force ;
extern CommandHandler  do_gecho ;
extern CommandHandler  do_gcomm ;
extern CommandHandler  do_gen_comm ;
extern CommandHandler  do_gen_door ;
extern CommandHandler  do_gen_ps ;
extern CommandHandler  do_gen_tog ;
extern CommandHandler  do_get ;
extern CommandHandler  do_give ;
extern CommandHandler  do_global_mute ;
extern CommandHandler  do_gold ;
extern CommandHandler  do_goto ;
extern CommandHandler  do_grab ;
extern CommandHandler  do_grant ;
extern CommandHandler  do_group ;
extern CommandHandler  do_gsay ;
extern CommandHandler  do_hamstring ;
extern CommandHandler  do_hide ;
extern CommandHandler  do_hit ;
extern CommandHandler  do_ignore ;
extern CommandHandler  do_incognito ;
extern CommandHandler  do_info ;
extern CommandHandler  do_insult ;
extern CommandHandler  do_inventory ;
extern CommandHandler  do_invert ;
extern CommandHandler  do_invis ;
extern CommandHandler  do_ipfind ;
extern CommandHandler  do_jedit ;
extern CommandHandler  do_jmap ;
extern CommandHandler  do_jstat ;
extern CommandHandler  do_jattach ;
extern CommandHandler  do_insert ;
extern CommandHandler  do_kick ;
extern CommandHandler  do_kill ;
extern CommandHandler  do_klist ;
extern CommandHandler  do_knock ;
extern CommandHandler  do_lag ;
extern CommandHandler  do_last ;
extern CommandHandler  do_lead ;
extern CommandHandler  do_leave ;
extern CommandHandler  do_levels ;
extern CommandHandler  do_look ;
extern CommandHandler  do_locate;
extern CommandHandler  do_mark ;
extern CommandHandler  do_mbload ;
extern CommandHandler  do_memory ;
extern CommandHandler  do_mlist ;
extern CommandHandler  do_move ;
extern CommandHandler  do_myzones ;
extern CommandHandler  do_not_here ;
extern CommandHandler  do_notice ;
extern CommandHandler  do_noreply ;
extern CommandHandler  do_offer ;
extern CommandHandler  do_oload ;
extern CommandHandler  do_olc ;
extern CommandHandler  do_olist ;
extern CommandHandler  do_order ;
extern CommandHandler  do_override ;
extern CommandHandler  do_pardon ;
extern CommandHandler  do_page ;
extern CommandHandler  do_poofset ;
extern CommandHandler  do_pour ;
extern CommandHandler  do_practice ;
extern CommandHandler  do_precisestrike ;
extern CommandHandler  do_pulverize ;
extern CommandHandler  do_purge ;
extern CommandHandler  do_put ;
extern CommandHandler  do_quit ;
extern CommandHandler  do_qval ;
extern CommandHandler  do_rage ;
extern CommandHandler  do_rank ;
extern CommandHandler  do_reboot ;
extern CommandHandler  do_release ;
extern CommandHandler  do_remove ;
extern CommandHandler  do_rent ;
extern CommandHandler  do_reply ;
extern CommandHandler  do_reroll ;
extern CommandHandler  do_rescue ;
extern CommandHandler  do_reset ;
extern CommandHandler  do_rest ;
extern CommandHandler  do_restat ;
extern CommandHandler  do_restore ;
extern CommandHandler  do_retool ;
extern CommandHandler  do_return ;
extern CommandHandler  do_retrieve ;
extern CommandHandler  do_ride ;
extern CommandHandler  do_rlist ;
extern CommandHandler  do_say ;
extern CommandHandler  do_speak ;
extern CommandHandler  do_save ;
extern CommandHandler  do_scan ;
extern CommandHandler  do_search ;
extern CommandHandler  do_self_delete ;
extern CommandHandler  do_send ;
extern CommandHandler  do_sense ;
extern CommandHandler  do_set ;
extern CommandHandler  do_shadowstep ;
extern CommandHandler  do_sheath ;
extern CommandHandler  do_shieldblock ;
extern CommandHandler  do_show ;
extern CommandHandler  do_shutdown ;
extern CommandHandler  do_sit ;
extern CommandHandler  do_skillset ;
extern CommandHandler  do_sleep ;
extern CommandHandler  do_slist ;
extern CommandHandler  do_sneak ;
extern CommandHandler  do_snoop ;
extern CommandHandler  do_source ;
extern CommandHandler  do_spec_comm ;
extern CommandHandler  do_stand ;
extern CommandHandler  do_statfind ;
extern CommandHandler  do_statedit ;
extern CommandHandler  do_steal ;
extern CommandHandler  do_switch ;
extern CommandHandler  do_syslog ;
extern CommandHandler  do_teleport ;
extern CommandHandler  do_tell ;
extern CommandHandler  do_tell_mute ;
extern CommandHandler  do_test_roll ;
extern CommandHandler  do_time ;
extern CommandHandler  do_toggle ;
extern CommandHandler  do_track ;
extern CommandHandler  do_trans ;
extern CommandHandler  do_unban ;
extern CommandHandler  do_ungroup ;
extern CommandHandler  do_untie ;
extern CommandHandler  do_use ;
extern CommandHandler  do_users ;
extern CommandHandler  do_visible ;
extern CommandHandler  do_view ;
extern CommandHandler  do_vnum ;
extern CommandHandler  do_vstat ;
extern CommandHandler  do_wake ;
extern CommandHandler  do_warn ;
extern CommandHandler  do_warrant ;
extern CommandHandler  do_warrants ;
extern CommandHandler  do_wear ;
extern CommandHandler  do_weather ;
extern CommandHandler  do_wedit ;
extern CommandHandler  do_where ;
extern CommandHandler  do_whirlwind ;
extern CommandHandler  do_wield ;
extern CommandHandler  do_wimpy ;
extern CommandHandler  do_wizlock ;
extern CommandHandler  do_wiznet ;
extern CommandHandler  do_wizutil ;
extern CommandHandler  do_wotmud ;
extern CommandHandler  do_write ;
extern CommandHandler  do_wshow ;
extern CommandHandler  do_zap ;
extern CommandHandler  do_zlist ;
extern CommandHandler  do_zreset ;
extern CommandHandler  do_saveall ;
extern CommandHandler  do_saveolc ;
extern CommandHandler  do_stat ;
extern CommandHandler  do_change ;
extern CommandHandler  do_jslist ;

#endif
