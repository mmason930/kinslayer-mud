
/* ************************************************************************
*   File: interpreter.c                                 Part of CircleMUD *
*  Usage: parse user commands, search for specials, call ACMD functions   *
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
#include "olc.h"
#include "weaves.h"
#include "stats.h"
#include "mobs.h"
#include "md5.h"
#include "accounts.h"

#include "js_functions.h"
#include "js.h"

#include "StringUtil.h"
#include "UserLogoutType.h"
#include "CharacterUtil.h"
#include "Descriptor.h"

extern const char *human_class_menu;
extern const char *other_class_menu;
extern const char *race_menu;
extern char *motd;
extern char *imotd;
extern char *background;
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
};

std::list<PendingSession> pendingSessions;

/* external functions */
int parse_class( char arg );
int special( Character *ch, char *cmd, char *arg );
int	search_block( char *arg, char **listy, int exact );
void oedit_parse( Descriptor *d, char *arg );
void redit_parse( Descriptor *d, char *arg );
void zedit_parse( Descriptor *d, char *arg );
void medit_parse( Descriptor *d, char *arg );
void sedit_parse( Descriptor *d, char *arg );
void kedit_parse( Descriptor *d, char *arg );
void aedit_parse( Descriptor *d, char *arg );
void cedit_parse( Descriptor *d, char *arg );
void StateditParse( Descriptor *d, const std::string &arg );
void AuctionParse( Descriptor *d, const std::string &arg );
#ifdef KINSLAYER_JAVASCRIPT
void JeditParse( Descriptor *d, const std::string &arg );
#endif

SPECIAL( receptionist );
SPECIAL( bank );
SPECIAL( guild );
SPECIAL( PokerDealer );

//int Crash_load(Character *ch, int show);

/* local functions */
int performDupeCheck( Descriptor *d );
void performFlee( Character *ch );
int performAlias( Descriptor *d, char *orig );
int reserved_word( char *argument );
int _parse_name( char *arg, char *name );


/* prototypes for all do_x functions. */
ACMD( do_action );
ACMD( do_advance );
ACMD( do_alias );
ACMD( do_assist );
ACMD( do_at );
ACMD( do_auction );
ACMD( do_award );
ACMD( do_backstab );
ACMD( do_ban );
ACMD( do_bash );
ACMD( do_butcher );
ACMD( do_break );
ACMD( do_scalp );
ACMD( do_call );
ACMD( do_cast );
ACMD( do_cedit );
ACMD( do_charge );
ACMD( do_choke );
ACMD( do_clan );
ACMD( do_cledit );
ACMD( do_color );
ACMD( do_commands );
ACMD( do_compel );
ACMD( do_consider );
ACMD( do_copy );
//ACMD(do_copyover);
ACMD( do_countdown );
ACMD( do_council );
ACMD( do_credits );
ACMD( do_diceroll );
ACMD( do_dig );
ACMD( do_disable );
ACMD( do_dismount );
ACMD( do_demote );
ACMD( do_drink );
ACMD( do_date );
ACMD( do_dc );
ACMD( do_declan );
ACMD( do_deny );
ACMD( do_diagnose );
ACMD( do_display );
ACMD( do_draw );
ACMD( do_drop );
ACMD( do_eat );
ACMD( do_echo );
ACMD( do_effuse );
ACMD( do_enable );
ACMD( do_enter );
ACMD( do_equipment );
ACMD( do_examine );
ACMD( do_exit );
ACMD( do_exits );
ACMD( do_extra );
ACMD( do_fade );
ACMD( do_flee );
ACMD( do_find );
ACMD( do_follow );
ACMD( do_force );
ACMD( do_gecho );
ACMD( do_gcomm );
ACMD( do_gen_comm );
ACMD( do_gen_door );
ACMD( do_gen_ps );
ACMD( do_gen_tog );
ACMD( do_get );
ACMD( do_give );
ACMD( do_global_mute );
ACMD( do_gold );
ACMD( do_goto );
ACMD( do_grab );
ACMD( do_grant );
ACMD( do_group );
ACMD( do_gsay );
ACMD( do_hamstring );
ACMD( do_hide );
ACMD( do_hit );
ACMD( do_ignore );
ACMD( do_incognito );
ACMD( do_info );
ACMD( do_insult );
ACMD( do_inventory );
ACMD( do_invert );
ACMD( do_invis );
ACMD( do_ipfind );
ACMD( do_install );
#ifdef KINSLAYER_JAVASCRIPT
ACMD( do_jedit );
ACMD( do_jstat );
ACMD( do_jattach );
#endif
ACMD( do_insert );
ACMD( do_kick );
ACMD( do_kill );
ACMD( do_klist );
ACMD( do_knock );
ACMD( do_lag );
ACMD( do_last );
ACMD( do_lead );
ACMD( do_leave );
ACMD( do_levels );
// ACMD(do_load);
ACMD( do_look );
ACMD( do_mark );
ACMD( do_mbload );
ACMD( do_memory );
ACMD( do_mlist );
ACMD( do_myzones );
/* ACMD(do_move); -- interpreter.h */
ACMD( do_not_here );
ACMD( do_notice );
ACMD( do_noreply );
ACMD( do_offer );
ACMD( do_oload );
ACMD( do_olc );
ACMD( do_olist );
ACMD( do_order );
ACMD( do_override );
ACMD( do_pardon );
ACMD( do_page );
ACMD( do_poofset );
ACMD( do_pour );
ACMD( do_practice );
ACMD( do_precisestrike );
ACMD( do_pulverize );
ACMD( do_purge );
ACMD( do_put );
ACMD( do_quit );
ACMD( do_qval );
ACMD( do_rage );
ACMD( do_rank );
ACMD( do_reboot );
ACMD( do_release );
ACMD( do_remove );
ACMD( do_rent );
ACMD( do_reply );
ACMD( do_reroll );
ACMD( do_rescue );
ACMD( do_reset );
ACMD( do_rest );
ACMD( do_restat );
ACMD( do_restore );
ACMD( do_retool );
ACMD( do_return );
ACMD( do_retrieve );
ACMD( do_ride );
ACMD( do_rlist );
ACMD( do_say );
ACMD( do_speak );
ACMD( do_save );
ACMD( do_scan );
ACMD( do_scite );
ACMD( do_search );
ACMD( do_self_delete );
ACMD( do_send );
ACMD( do_sense );
ACMD( do_set );
ACMD( do_shadowstep );
ACMD( do_sheath );
ACMD( do_shieldblock );
ACMD( do_show );
ACMD( do_shutdown );
ACMD( do_sit );
ACMD( do_skillset );
ACMD( do_sleep );
ACMD( do_slist );
ACMD( do_sneak );
ACMD( do_snoop );
ACMD( do_source );
ACMD( do_spec_comm );
ACMD( do_stand );
ACMD( do_statfind );
ACMD( do_statedit );
ACMD( do_steal );
ACMD( do_swap );
ACMD( do_switch );
ACMD( do_syslog );
ACMD( do_teleport );
ACMD( do_tell );
ACMD( do_tell_mute );
ACMD( do_test_roll );
ACMD( do_time );
ACMD( do_toggle );
ACMD( do_track );
ACMD( do_trans );
ACMD( do_unban );
ACMD( do_ungroup );
ACMD( do_untie );
ACMD( do_use );
ACMD( do_users );
ACMD( do_visible );
ACMD( do_view );
ACMD( do_vnum );
ACMD( do_vstat );
ACMD( do_wake );
ACMD( do_warn );
ACMD( do_warrant );
ACMD( do_warrants );
ACMD( do_wear );
ACMD( do_weather );
ACMD( do_wedit );
ACMD( do_where );
ACMD( do_whirlwind );
ACMD( do_wield );
ACMD( do_wimpy );
ACMD( do_wizlock );
ACMD( do_wiznet );
ACMD( do_wizutil );
ACMD( do_wotmud );
ACMD( do_write );
ACMD( do_wshow );
ACMD( do_zap );
ACMD( do_zlist );
ACMD( do_zreset );
ACMD( do_saveall );

/**************************
 * Our Commands
 *************************/
ACMD( do_saveolc );
ACMD( do_stat );
ACMD( do_change );
ACMD( do_jslist );
/* DG Script ACMD's */
ACMD( do_attach );
ACMD( do_detach );
ACMD( do_tlist );
ACMD( do_tstat );
ACMD( do_masound );
ACMD( do_mkill );
ACMD( do_mjunk );
ACMD( do_damage );

class CommandInfo *complete_cmd_info;

CommandManager *CommandManager::Self = (NULL);

CommandManager::CommandManager()  {}
CommandManager::~CommandManager() {}

CommandManager &CommandManager::GetManager()
{
	if( this->Self == (NULL) ) this->Self = (new CommandManager());
	return (*Self);
}

void CommandManager::AppendCommand( const std::string cmd, const std::string sort, void (*command_pointer)(Character *,char *,int,int),
						const byte mp, const byte ml, const sh_int subcmd, const float timer, const byte vml)
{
	CommandInfo *Com = new CommandInfo();
	Com->command			= cmd;
	Com->sort_as			= sort;
	Com->command_pointer	= command_pointer;
	Com->minimum_position	= mp;
	Com->minimum_level		= ml;
	Com->subcmd				= subcmd;
	Com->timer				= timer;
	Com->view_minimum_level	= vml;

	MyCommands.push_back( Com );
}
void CommandManager::BootCommands()
{
	
}
void CommandManager::SaveCommands() {}//Currently not used.
/*
CommandInfo::CommandInfo()
{
	minimum_position	= (POS_DEAD);
	minimum_level		= (LVL_IMPL);
	view_minimum_level	= (LVL_IMPL);
	command_pointer		= (NULL);
	subcmd				= (0);
	timer				= (0.0f);
}
*/
CommandInfo::~CommandInfo()
{
}

/* This is the Master Command List(tm).

 * You can put new commands in, take commands out, change the order
 * they appear in, etc.  You can adjust the "priority" of commands
 * simply by changing the order they appear in the command list.
 * (For example, if you want "as" to mean "assist" instead of "ask",
 * just put "assist" above "ask" in the Master Command List(tm).
 *
 * In general, utility commands such as "at" should have high priority;
 * infrequently used and dangerously destructive commands should have low
 * priority.
 */

class CommandInfo cmd_info[] =
    {
	    {	"<RESERVED>", "<RESERVED>"	, 0		, 0					, 0			, 0					, 0.0	,  0	},
	    /* this must be first -- for specprocs */

	    /* directions must come before other commands but after RESERVED */
		{	"north"		, "n"	, POS_STANDING	, do_move			, 0			, SCMD_NORTH		, 0.0	,  0 	},
	    {	"east"		, "e"	, POS_STANDING	, do_move			, 0			, SCMD_EAST			, 0.0	,  0 	},
	    {	"south"		, "s"	, POS_STANDING	, do_move			, 0			, SCMD_SOUTH		, 0.0	,  0 	},
	    {	"west"		, "w"	, POS_STANDING	, do_move			, 0			, SCMD_WEST			, 0.0	,  0 	},
	    {	"up"		, "u"	, POS_STANDING	, do_move			, 0			, SCMD_UP			, 0.0	,  0 	},
	    {	"down"		, "d"	, POS_STANDING	, do_move			, 0			, SCMD_DOWN			, 0.0	,  0 	},

	    /* now, the main list */
	    {	"at"		, "at"	, POS_DEAD		, do_at				, LVL_APPR	, 0					, 0.0	,  0 	},
	    {	"advance"	, "adv"	, POS_DEAD		, do_advance		, LVL_GRGOD	, 0					, 0.0	,  0 	},
	    {	"aedit"		, "aed"	, POS_DEAD		, do_olc			, LVL_APPR	, SCMD_OLC_AEDIT	, 0.0	,  0 	},
	    {	"alias"		, "ali"	, POS_DEAD		, do_alias			, 0			, 0					, 0.0	,  0 	},
	    {	"assist"	, "ass"	, POS_FIGHTING	, do_assist			, 1			, 0					, 0.0	,  0 	},
	    {	"ask"		, "ask"	, POS_RESTING	, do_spec_comm		, 0			, SCMD_ASK			, 0.0	,  0 	},
	    {	"attack"	, "att"	, POS_FIGHTING	, do_kill			, 0			, 0					, 0.0	,  0 	},
	    {	"autoexit"	, "autoe",POS_DEAD		, do_gen_tog		, 0			, SCMD_AUTOEXIT		, 0.0	,  0 	},
		{   "autoscan"  , "autos",POS_DEAD		, do_gen_tog		, 0			, SCMD_AUTOSCAN		, 0.0	,  0 	},
		{	"auction"	, "auc"	, POS_RESTING	, do_auction		, 0			, 0					, 0.0	,  0	},
		{	"award"		, "aw"	, POS_DEAD		, do_award			, 0			, 0					, 0.0	,  0 	},

	    {	"backstab"	, "bac"	, POS_FIGHTING	, do_backstab		, 1			, 0					, 4.5f	,  0 	},
	    {	"ban"		, "ban"	, POS_DEAD		, do_ban			, LVL_GRGOD	, 0					, 0.0	,  0	},
	    {	"balance"	, "bal"	, POS_STANDING	, do_not_here		, 1			, 0					, 0.0	,  0	},
	    {	"bash"		, "bas"	, POS_FIGHTING	, do_bash			, 1			, 0					, 4.2f	,  0 	},
		{	"block"		, "blo"	, POS_FIGHTING	, do_shieldblock	, 1			, 0					, 5.0f	,  0 	},
	    { 	"break"		, "bre" , POS_DEAD		, do_break			, 0			, 0					, 0.0	,  0	},
	    {	"brief"		, "br"	, POS_DEAD		, do_gen_tog		, 0			, SCMD_BRIEF		, 0.0	,  0	},
	    {	"buildwalk"	, "bui"	, POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_BUILDWALK	, 0.0	,  0	},

	    {	"butcher"	, "bu"	, POS_STANDING	, do_butcher		, 0			, SCMD_BUTCHER		, 0.0	,  0	},
	    {	"buy"		, "b"	, POS_STANDING	, do_not_here		, 0			, 0					, 0.0	,  0 	},

	    {	"channel"	, "c"	, POS_DEAD		, do_cast			, 1			, 0					, 0.0	,  0		},
	    //{	"call"		, "ca"	, POS_RESTING	, do_call			, 0			, 0					, 0.0	,  0		},
	    {	"cedit"		, "ced"	, POS_DEAD		, do_cedit			, LVL_APPR	, 0					, 0.0	,  0		},
	    {	"check"		, "ch"	, POS_STANDING	, do_not_here		, 1			, 0					, 0.0	,  0		},
	    {	"chat"		, "cha"	, POS_RESTING	, do_gen_comm		, 0			, SCMD_CHAT			, 0.0	,  0		},
	    {	"charge"	, "char", POS_FIGHTING	, do_charge			, 0			, SCMD_CHARGE		, 2.25	,  0		},
	    {	"change"	, "chang",POS_DEAD		, do_change			, 0			, 0					, 0.0	,  0		},
		{	"choke"		, "cho"	, POS_FIGHTING	, do_choke			, 0			, 0					, 4.0	,  0		},
		{	"clan"		, "clan", POS_DEAD		, do_clan			, 0			, 0					, 0.0	,  LVL_APPR		},
	    {	"clear"		, "cle"	, POS_DEAD		, do_gen_ps			, 0			, SCMD_CLEAR		, 0.0	,  0		},
	    {	"cledit"	, "cled", POS_DEAD		, do_cledit			, LVL_GOD	, 0					, 0.0f	,  0		},
	    {	"close"		, "clo"	, POS_SITTING	, do_gen_door		, 0			, SCMD_CLOSE		, 0.0	,  0		},
	    {	"cls"		, "cls"	, POS_DEAD		, do_gen_ps			, 0			, SCMD_CLEAR		, 0.0	,  0		},
		{	"compel"	, "compe",POS_STANDING	, do_compel			, 1			, 0					, 4	,  0 	},
	    {	"consider"	, "con"	, POS_RESTING	, do_consider		, 0			, 0					, 0.0	,  0		},
	    {	"color"		, "col"	, POS_DEAD		, do_color			, 0			, 0					, 0.0	,  0	},
	    {	"commands"	, "comm", POS_DEAD		, do_commands		, 0			, SCMD_COMMANDS		, 0.0	,  0		},
	    {	"compact"	, "comp", POS_DEAD		, do_gen_tog		, 0			, SCMD_COMPACT		, 0.0	,  0		},
		{	"copy"		, "copy", POS_DEAD		, do_copy			, LVL_IMMORT, 0					, 0.0	,  0			},
//		{	"copyover"	, "copyo",POS_DEAD		, do_copyover		, LVL_IMPL	, 0					, 0.0	, 0		},
		{	"credits"	, "cre"	, POS_DEAD		, do_gen_ps			, 0			, SCMD_CREDITS		, 0.0	,  0		},
		{	"council"	, "cou"	, POS_DEAD		, do_council		, 0			, 0					, 0.0	,  LVL_APPR		},

	    {	"date"		, "date", POS_DEAD		, do_date			, LVL_IMMORT, SCMD_DATE			, 0.0	,  0	},
	    {	"dc"		, "dc"	, POS_DEAD		, do_dc				, LVL_GOD	, 0					, 0.0	,  LVL_APPR		},
	    {	"demote"	, "dem"	, POS_DEAD		, do_demote			, 0			, 0					, 0.0	,  LVL_APPR 	},
	    {	"declan"	, "dec"	, POS_DEAD		, do_declan			, 0			, 0					, 0.0	,  LVL_APPR 	},
		{	"deny"		, "den"	, POS_DEAD		, do_deny			, 0			, 0					, 0.0	,  0 	},
	    {	"deposit"	, "dep"	, POS_STANDING	, do_not_here		, 1			, 0					, 0.0	,  0 	},
	    {	"diagnose"	, "dia"	, POS_RESTING	, do_diagnose		, 0			, 0					, 0.0	,  0 	},
	    {	"diceroll"	, "dic"	, POS_SITTING	, do_diceroll		, 0			, 0					, 0.0	,  0 	},
	    {	"dig"		, "dig"	, POS_DEAD		, do_dig			, LVL_IMMORT, 0					, 0.0	,  0	},
	    {	"disable"	, "disa", POS_DEAD		, do_disable		, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"dismount"	, "dis"	, POS_FIGHTING	, do_dismount		, 0			, 0					, 0.0	,  0 	},
	    {	"display"	, "disp", POS_DEAD		, do_display		, 0			, 0					, 0.0	,  0 	},
	    {	"drink"		, "dr"	, POS_RESTING	, do_drink			, 0			, SCMD_DRINK		, 0.0	,  0 	},
	    {	"draw"		, "dra"	, POS_RESTING	, do_draw			, 0			, 0					, 0.0f	,  0	},
	    {	"drop"		, "dro"	, POS_RESTING	, do_drop			, 0			, SCMD_DROP			, 0.0	,  0 	},

	    {	"eat"		, "ea"	, POS_RESTING	, do_eat			, 0			, SCMD_EAT			, 0.0	,  0 	},
	    {	"echo"		, "echo", POS_SLEEPING	, do_echo			, LVL_GOD	, SCMD_ECHO			, 0.0	,  0 	},
	    {	"effuse"	, "ef"	, POS_STANDING	, do_effuse			, 0			, 0					, 0.0	,  0 	},
	    {	"embrace"	, "emb"	, POS_RESTING	, do_source			, 0			, SCMD_EMBRACE		, 0.0	,  0 	},
	    {	"enable"	, "ena"	, POS_DEAD		, do_enable			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"enter"		, "en"	, POS_STANDING	, do_enter			, 0			, 0					, 0.0	,  0 	},
	    {	"equipment"	, "eq"	, POS_SLEEPING	, do_equipment		, 0			, 0					, 0.0	,  0 	},
	    {	"exits"		, "exi"	, POS_RESTING	, do_exits			, 0			, 0					, 0.0	,  0 	},
	    {	"examine"	, "exa"	, POS_RESTING	, do_examine		, 0			, 0					, 0.0	,  0 	},
	    {	"extra"		, "ext"	, POS_DEAD		, do_extra			, LVL_GOD	, 0					, 0.0	,  0 	},

	    {	"flee"		, "f"	, POS_FIGHTING	, do_flee			, 0			, 0					, 0.0	,  0 	},
	    {	"fade"		, "fa"	, POS_STANDING	, do_fade			, 0			, 0					, 5		,  0 	},
	    {	"find"		, "fi"	, POS_DEAD		, do_find			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"force"		, "for"	, POS_SLEEPING	, do_force			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"fill"		, "fil"	, POS_STANDING	, do_pour			, 0			, SCMD_FILL			, 0.0	,  0 	},
	    {	"follow"	, "fol"	, POS_RESTING	, do_follow			, 0			, 0					, 0.0	,  0 	},
	    {	"freeze"	, "free", POS_DEAD		, do_wizutil		, LVL_FREEZE, SCMD_FREEZE		, 0.0	,  0 	},

	    {	"get"		, "g"	, POS_RESTING	, do_get			, 0			, 0					, 0.0	,  0 	},
		{	"gcomm"		, "gc"	, POS_DEAD		, do_gcomm			, LVL_IMMORT, 0					, 0.0	,  0 	},
		{	"gecho"		, "gech", POS_DEAD		, do_gecho			, LVL_GRGOD	, 0					, 0.0	,  0 	},
	    {	"give"		, "giv"	, POS_STANDING	, do_give			, 0			, 0					, 0.0	,  0 	},
		{	"global"	, "glo"	, POS_RESTING	, do_gen_comm		, 0			, SCMD_GLOBAL		, 0.0	,  0 	},
		{	"globalmute", "globalm",POS_DEAD	, do_global_mute	, LVL_APPR	, 0					, 0.0f	,0 	},
		{	"goto"		, "got"	, POS_SLEEPING	, do_goto			, LVL_IMMORT, 0					, 0.0	,  0 	},
	    {	"gold"		, "gol"	, POS_RESTING	, do_gold			, 0			, 0					, 0.0	,  0 	},
	    {	"group"		, "gr"	, POS_RESTING	, do_group			, 1			, 0					, 0.0	,  0 	},
		{	"grant"		, "gra"	, POS_DEAD		, do_grant			, 0			, 0					, 0.0	,  0 	},
	    {	"grab"		, "gra"	, POS_RESTING	, do_grab			, 0			, 0					, 0.0	,  0 	},

		{	"hamstring"	, "ham"	, POS_FIGHTING	, do_hamstring		, 1			, 0					, 4.2f	,  0	},
	    {	"handbook"	, "hand", POS_DEAD		, do_gen_ps			, LVL_IMMORT, SCMD_HANDBOOK		, 0.0	,  0 	},
	    {	"hide"		, "hid"	, POS_STANDING	, do_hide			, 0			, 0					, 0.0	,  0 	},
	    {	"hit"		, "h"	, POS_FIGHTING	, do_hit			, 0			, SCMD_HIT			, 0.0	,  0 	},
	    {	"hold"		, "ho"	, POS_RESTING	, do_grab			, 1			, 0					, 0.0	,  0 	},
	    {	"holylight"	, "holy", POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_HOLYLIGHT	, 1.0	,  0	},

	    {	"inventory"	, "i"	, POS_DEAD		, do_inventory		, 0			, 0					, 0.0	,  0 	},
	    {	"ignore"	, "ig"	, POS_DEAD		, do_ignore			, 0			, 0					, 0.0	,  0 	},
	    {	"incognito"	, "in"	, POS_DEAD		, do_incognito		, 0			, 0					, 0.0	,  0 	},
	    {	"imotd"		, "imo"	, POS_DEAD		, do_gen_ps			, LVL_IMMORT, SCMD_IMOTD		, 0.0	,  0 	},
	    {	"insult"	, "ins"	, POS_RESTING	, do_insult			, 0			, 0					, 0.0	,  0 	},
		{	"invert"	, "inve", POS_FIGHTING	, do_invert			, 0			, 0					, 2.0	,  0	},
	    {	"invis"		, "inv"	, POS_DEAD		, do_invis			, LVL_IMMORT, 0					, 0.0	,  0 	},
	    {	"ipfind"	, "ip"	, POS_DEAD		, do_ipfind			, LVL_GRGOD	, 0					, 0.0	,  0 	},
	    {	"insert"	, "ip"	, POS_DEAD		, do_insert			, LVL_GOD	, 0					, 0.0	,  0 	},
		{	"install"	, "ins"	, POS_DEAD		, do_install		, LVL_GOD	, 0					, 0.0	,  0	},
#ifdef KINSLAYER_JAVASCRIPT
		{	"jattach"	, "ja"	, POS_DEAD		, do_jattach		, LVL_GOD	, 0					, 0.0	,  0 	},
		{	"jedit"		, "jed"	, POS_DEAD		, do_jedit			, LVL_APPR	, 0					, 0.0	,  0 	},
		{	"jstat"		, "js"	, POS_DEAD		, do_jstat			, LVL_APPR	, 0					, 0.0	,  0 	},
		{	"jslist"	, "jsl"	, POS_DEAD		, do_jslist			, LVL_APPR	, 0					, 0.0	,  0	},
#endif
	    {	"kill"		, "k"	, POS_FIGHTING	, do_kill			, 0			, 0					, 0.0	,  0 	},
	    {	"kedit"		, "ke"	, POS_DEAD		, do_olc			, LVL_APPR	, SCMD_OLC_KEDIT	, 0.0	,  0 	},
	    {	"kick"		, "kic"	, POS_FIGHTING	, do_kick			, 1			, 0					, 0.0	,  0 	},
	    {	"klist"		, "kl"	, POS_DEAD		, do_klist			, LVL_IMMORT, 0					, 0.0	,  0 	},
		{	"knock"		, "kn"	, POS_STANDING	, do_knock			, 0			, 0					, 0.0	,  0 	},

	    {	"look"		, "l"	, POS_RESTING	, do_look			, 0			, SCMD_LOOK			, 0.0	,  0 	},
	    {	"last"		, "la"	, POS_DEAD		, do_last			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"lag"		, "lag"	, POS_DEAD		, do_lag			, LVL_IMPL	, 0					, 0.0	,  0 	},
	    {	"lead"		, "lead", POS_STANDING	, do_lead			, 0			, 0					, 0.0	,  0 	},
		{	"leave"		, "lea"	, POS_STANDING	, do_leave			, 0			, 0					, 0.0	,  0 	},
	    {	"levels"	, "lev"	, POS_DEAD		, do_levels			, 0			, 0					, 0.0	,  0 	},
	    {	"list"		, "li"	, POS_STANDING	, do_not_here		, 0			, 0					, 0.0	,  0 	},
	    {	"lock"		, "loc"	, POS_SITTING	, do_gen_door		, 0			, SCMD_LOCK			, 0.0	,  0 	},

	    {	"medit"		, "med"	, POS_DEAD		, do_olc			, LVL_BLDER	, SCMD_OLC_MEDIT	, 0.0f	,  0	},
	    {	"mark"		, "mar"	, POS_FIGHTING	, do_mark			, 0			, 0					, 0.0	,  0	},
	    {	"mbload"	, "mblo", POS_DEAD		, do_mbload			, LVL_BLDER	, 0					, 0.0f	,  0	},
	    {	"memory"	, "mem"	, POS_DEAD		, do_memory			, LVL_BLDER	, 0					, 0.0f	,  0	},
	    {	"mlist"		, "mli"	, POS_DEAD		, do_mlist			, LVL_BLDER	, 0					, 0.0f	,  0	},
	    {	"motd"		, "motd", POS_DEAD		, do_gen_ps			, 0			, SCMD_MOTD			, 0.0f	,  0	},
	    {	"mute"		, "mut"	, POS_DEAD		, do_wizutil		, LVL_APPR	, SCMD_SQUELCH		, 0.0f	,  0	},
	    {	"murder"	, "mur"	, POS_FIGHTING	, do_hit			, 0			, SCMD_MURDER		, 0.0f	,  0	},
	    {	"myzones"	, "myz"	, POS_DEAD		, do_myzones		, LVL_IMMORT, 0					, 0.0f	,  0	},

	    {	"narrate"	, "nar"	, POS_RESTING	, do_gen_comm		, 0			, SCMD_NARRATE		, 0.0	,  0 	},
	    {	"news"		, "new"	, POS_SLEEPING	, do_gen_ps			, 0			, SCMD_NEWS			, 0.0	,  0 	},
	    {	"notice"	, "not"	, POS_RESTING	, do_notice			, 0			, 0					, 0.0	,  0 	},
	    {	"noalerts"	, "nogr", POS_DEAD		, do_gen_tog		, 0			, SCMD_NOALERTS		, 0.0	,  0 	},
	    {	"nochats"	, "nogr", POS_DEAD		, do_gen_tog		, 0			, SCMD_NOCHAT		, 0.0	,  0 	},
	    {	"noglobals" , "nog"	, POS_DEAD		, do_gen_tog		, 0			, SCMD_NOGLOBAL		, 0.0	,  0 	},
	    {	"nohassle"	, "noh"	, POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_NOHASSLE		, 0.0	,  0 	},
	    {	"nonarrates", "noa"	, POS_DEAD		, do_gen_tog		, 0			, SCMD_NONARRATE	, 0.0	,  0 	},
	    {	"norepeat"	, "nor"	, POS_DEAD		, do_gen_tog		, 0			, SCMD_NOREPEAT		, 0.0	,  0 	},
		{	"noreply"	, "nore", POS_DEAD		, do_noreply		, LVL_IMMORT, 0					, 0.0	,  0	},
		{	"nospeaks"	, "nos"	, POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_NOSPEAKS		, 0.0	,  0 	},
		{	"notell"	, "note", POS_DEAD		, do_gen_tog		, LVL_APPR	, SCMD_NOTELL		, 0.0	,  0 	},
	    {	"notips"	, "notip",POS_DEAD		, do_gen_tog		, 0			, SCMD_NOTIPS		, 0.0	,  0 	},
	    {	"notitle"	, "noti", POS_DEAD		, do_wizutil		, LVL_GOD	, SCMD_NOTITLE		, 0.0	,  0 	},
	    {	"nowiz"		, "now"	, POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_NOWIZ		, 0.0	,  0 	},

	    {	"order"		, "ord"	, POS_RESTING	, do_order			, 1			, 0					, 0.0	,  0 	},
	    {	"offer"		, "off"	, POS_STANDING	, do_not_here		, 1			, 0					, 0.0	,  0 	},
	    {	"open"		, "op"	, POS_SITTING	, do_gen_door		, 0			, SCMD_OPEN			, 0.0	,  0 	},
	    {	"oload"		, "oloa", POS_DEAD		, do_oload			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"olc"		, "olc"	, POS_DEAD		, do_olc			, LVL_IMMORT, SCMD_OLC_SAVEINFO	, 0.0	,  0 	},
	    {	"olist"		, "oli"	, POS_DEAD		, do_olist			, LVL_GOD	, 0					, 0.0f	,  0 	},
	    {	"oedit"		, "oed"	, POS_DEAD		, do_olc			, LVL_GOD	, SCMD_OLC_OEDIT	, 0.0	,  0	},
	    {	"override"	, "overr",POS_DEAD		, do_override		, LVL_IMMORT, 0					, 0.0	,  0	},
	    {	"put"		, "p"	, POS_RESTING	, do_put			, 0			, 0					, 0.0	,  0 	},
	    {	"practice"	, "pr"	, POS_RESTING	, do_practice		, 1			, 0					, 0.0	,  0 	},
	    {	"page"		, "pag"	, POS_DEAD		, do_page			, LVL_APPR	, 0					, 0.0	,  0 	},
	    {	"pardon"	, "par"	, POS_DEAD		, do_pardon			, 0			, 0					, 0.0	,  0 	},
	    {	"pick"		, "pi"	, POS_STANDING	, do_gen_door		, 1			, SCMD_PICK			, 0.0	,  0 	},
	    {	"policy"	, "pol"	, POS_DEAD		, do_gen_ps			, 0			, SCMD_POLICIES		, 0.0	,  0 	},
	    {	"poofin"	, "poofi",POS_DEAD		, do_poofset		, LVL_IMMORT, SCMD_POOFIN		, 0.0	,  0 	},
		{	"poofout"	, "poofo",POS_DEAD		, do_poofset		, LVL_IMMORT, SCMD_POOFOUT		, 0.0	,  0 	},
	    {	"pour"		, "pour", POS_STANDING	, do_pour			, 0			, SCMD_POUR			, 0.0	,  0 	},
		{	"precisestrike","pre",POS_FIGHTING	, do_precisestrike	, 1			, 0					, 3.0f	,  0 	},
	    {	"prompt"	, "promp",POS_DEAD		, do_display		, 0			, 0					, 0.0	,  0 	},
		{	"pulverize"	, "pul"	, POS_DEAD		, do_pulverize		, 0			, 0					, 4.3f	,  0 	},
	    {	"purge"		, "pur"	, POS_DEAD		, do_purge			, LVL_BLDER	, 0					, 0.0	,  0 	},

	    {	"quaff"		, "q"	, POS_RESTING	, do_use			, 0			, SCMD_QUAFF		, 0.0	,  0 	},
	    {	"qui"		, "qui"	, POS_DEAD		, do_quit			, 0			, 0					, 0.0	,  0 	},
	    {	"quit"		, "quit", POS_DEAD		, do_quit			, 0			, SCMD_QUIT			, 0.0	,  0 	},
	    {	"qval"		, "qv"	, POS_DEAD		, do_qval			, LVL_APPR	, 0					, 0.0	,  0 	},

	    {	"reply"		, "r"	, POS_SLEEPING	, do_reply			, 0			, 0					, 0.0	,  0 	},
	    {	"rank"		, "ran"	, POS_DEAD		, do_rank			, 0			, 0					, 0.0	,  0 	},
		{	"rage"		, "rag" , POS_FIGHTING	, do_rage			, 1			, 0					, 0.0	,  0	},
	    {	"read"		, "rea"	, POS_RESTING	, do_look			, 0			, SCMD_READ			, 0.0	,  0 	},
	    {	"rest"		, "re"	, POS_RESTING	, do_rest			, 0			, 0					, 0.0	,  0 	},
	    {	"reboot"	, "reb"	, POS_DEAD		, do_countdown		, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"release"	, "rel"	, POS_RESTING	, do_release		, 0			, 0					, 0.0	,  0 	},
	    {	"reload"	, "rele", POS_DEAD		, do_reboot			, LVL_IMPL	, 0					, 0.0	,  0 	},
	    {	"remove"	, "rem"	, POS_RESTING	, do_remove			, 0			, 0					, 0.0	,  0 	},
	    {	"rent"		, "ren"	, POS_STANDING	, do_not_here		, 1			, 0					, 0.0	,  0 	},
	    {	"reroll"	, "rer"	, POS_DEAD		, do_reroll			, LVL_GRGOD	, 0					, 0.0	,  0 	},
	    {	"rescue"	, "resc", POS_FIGHTING	, do_rescue			, 1			, 0					, 0.0	,  0 	},
	    {	"reset"		, "res"	, POS_DEAD		, do_reset			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"restat"	, "resta",POS_RESTING	, do_restat			, 0			, 0					, 0.0	,  0 	},
	    {	"restore"	, "resto",POS_DEAD		, do_restore		, LVL_GOD	, 0					, 0.0	,  0 	},
		{	"return"	, "ret"	, POS_DEAD		, do_return			, 0			, 0					, 0.0	,  0 	},
		{	"retrieve"	, "retr", POS_DEAD		, do_retrieve		, LVL_GOD	, 0					, 0.0	,  0	},
		{   "retool"    , "reto", POS_DEAD		, do_retool			, LVL_GOD	, 0					, 0.0	,  0	},
	    {	"redit"		, "redit",POS_DEAD		, do_olc			, LVL_IMMORT, SCMD_OLC_REDIT	, 0.0	,  0	},
	    {	"ride"		, "ri"	, POS_STANDING	, do_ride			, 1			, 0					, 0.0	,  0 	},
	    {	"rlist"		, "rlist",POS_DEAD		, do_rlist			, LVL_IMMORT, 0					, 0.0	,  0 	},
	    {	"roomflags"	, "roomf",POS_DEAD		, do_gen_tog		, LVL_IMMORT, SCMD_ROOMFLAGS	, 0.0	,  0 	},

	    {	"say"		, "sa"	, POS_RESTING	, do_say			, 0			, 0					, 0.0	,  0 	},
	    {	"'"			, "'"	, POS_RESTING	, do_say			, 0			, 0					, 0.0	,  0 	},
	    {	"scalp"		, "sca"	, POS_STANDING	, do_scalp			, 0			, 0					, 0.0	,  0 	},
	    {	"speak"		, "sp"	, POS_RESTING	, do_speak			, LVL_BLDER	, 0					, 0.0	,  0 	},
	    {	"save"		, "save", POS_SLEEPING	, do_save			, 0			, 0					, 0.0	,  0 	},
	    {	"scan"		, "scan", POS_STANDING	, do_scan			, 0			, 0					, 0.0	,  0 	},
		{	"scite"		, "sci"	, POS_DEAD		, do_scite			, LVL_APPR	, 0					, 0.0	,  0	},
		{	"search"	, "sea"	, POS_STANDING	, do_search			, 0			, 0					, 0.0	,  0 	},
	    {	"saveall"	, "sall", POS_DEAD		, do_saveall		, LVL_GOD	, 0					, 0.0	,  0	},
	    {	"seize"		, "sei"	, POS_RESTING	, do_source			, 0			, SCMD_SEIZE		, 0.0	,  0 	},
	    {	"sell"		, "sel"	, POS_STANDING	, do_not_here		, 0			, 0					, 0.0	,  0 	},
	    {	"send"		, "sen"	, POS_SLEEPING	, do_send			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"sense"		, "sens", POS_STANDING	, do_sense			, 0			, 0					, 5	,  0 	},
	    {	"set"		, "set"	, POS_DEAD		, do_set			, LVL_GOD	, 0					, 0.0	,  0 	},
	    {	"sedit"		, "sedi", POS_DEAD		, do_olc			, LVL_BLDER	, SCMD_OLC_SEDIT	, 0.0	,  0	},
	    {	"selfdelete", "self", POS_DEAD		, do_self_delete	, 0			, 0					, 0.0	,  0 	},
	    {	"sheath"	, "she"	, POS_RESTING	, do_sheath			, 0			, 0					, 0.0f	,  0		},
	    {	"shout"		, "sho"	, POS_RESTING	, do_gen_comm		, 1			, SCMD_SHOUT		, 0.0	,  0 	},
		{	"shadowstep", "shad", POS_FIGHTING	, do_shadowstep		, 1			, 0					, 2.0	,  0	},
	    {	"shake"		, "sha"	, POS_RESTING	, do_action			, 0			, 0					, 0.0	,  0 	},
		{	"shieldblock","shie", POS_FIGHTING	, do_shieldblock	, 1			, 0					, 5.0f	,  0 	}, 
	    {	"shiver"	, "shiv", POS_RESTING	, do_action			, 0			, 0					, 0.0	,  0 	},
		{	"show"		, "show", POS_STANDING	, do_show			, 0			, 0					, 0.0	,  0	},
		{	"shutdow"	, "shutdow",POS_DEAD	, do_shutdown		, LVL_GRGOD	, 0					, 0.0	,  0	},
	    {	"shutdown"	, "shutdown",POS_DEAD	, do_shutdown		, LVL_IMPL	, SCMD_SHUTDOWN		, 0.0	,  0 	},
	    {	"sip"		, "sip"	, POS_RESTING	, do_drink			, 0			, SCMD_SIP			, 0.0	,  0 	},
	    {	"sit"		, "sit"	, POS_RESTING	, do_sit			, 0			, 0					, 0.0f	,  0 	},
	    {	"skewer"	, "ske"	, POS_FIGHTING	, do_charge			, 0			, SCMD_SKEWER		, 2.25	,  0		},
	    {	"skills"	, "skills",POS_RESTING	, do_practice		, 1			, 0					, 0.0	,  0 	},
	    {	"skillset"	, "skillse",POS_SLEEPING, do_skillset		, LVL_GRGOD	, 0					, 0.0f	,  0 	},
	    {	"skin"	    , "ski"	, POS_STANDING	, do_butcher    	, 0			, SCMD_SKIN			, 0.0	,  0	},
		{	"sleep"		, "sl"	, POS_SLEEPING	, do_sleep			, 0			, 0					, 0.0f	,  0 	},
	    {	"slist"		, "sli"	, POS_DEAD		, do_slist			, LVL_BLDER	, 0					, 0.0f	,  0		},
	    {	"slowns"	, "slown",POS_DEAD		, do_gen_tog		, LVL_IMPL	, SCMD_SLOWNS		, 0.0f	,  0 	},
	    {	"sneak"		, "snea", POS_STANDING	, do_sneak			, 1			, 0					, 0.0f	,  0 	},
	    {	"snoop"		, "sno"	, POS_DEAD		, do_snoop			, LVL_IMMORT, 0					, 0.0	,  0 	},
	    {	"socials"	, "soc"	, POS_DEAD		, do_commands		, 0			, SCMD_SOCIALS		, 0.0f	,  0 	},
	    {	"stand"		, "st"	, POS_RESTING	, do_stand			, 0			, 0					, 0.0f	,  0 	},
	    {	"stats"		, "stat", POS_DEAD		, do_stat			, 1			, 0					, 0.0f	,  0 	},
	    {	"statfind"	, "statf",POS_DEAD		, do_statfind		, LVL_GOD	, 0					, 0.0f	,  0 	},
		{	"statedit"	, "state",POS_DEAD		, do_statedit		, LVL_APPR	, 0					, 0.0	,  0	},
	    {	"steal"		, "stea", POS_STANDING	, do_steal			, 1			, 0					, 3.0f	,0 	},
		{	"strike"	, "str", POS_FIGHTING	, do_precisestrike	, 1			, 0					, 3.0f	,0 	},
	    {	"swap"		, "swa"	, POS_DEAD		, do_swap			, LVL_GOD	, 0					, 0.0f	,0 	},
	    {	"switch"	, "sw"	, POS_DEAD		, do_switch			, LVL_APPR	, 0					, 0.0f	,0 	},
	    {	"syslog"	, "sys"	, POS_DEAD		, do_syslog			, LVL_APPR	, 0					, 0.0f	,0 	},

	    {	"tell"		, "t"	, POS_RESTING	, do_tell			, 0			, 0					, 0.0f	,0 	},
	    {	"tellmute"	, "tellm",POS_DEAD		, do_tell_mute		, LVL_APPR	, 0					, 0.0f	,0 	},
	    {	"testroll"	, "testr",POS_DEAD		, do_test_roll		, LVL_APPR	, 0					, 0.0f	,0 	},
	    {	"take"		, "ta"	, POS_RESTING	, do_get			, 0			, 0					, 0.0f	,0 	},
	    {	"taste"		, "tas"	, POS_RESTING	, do_eat			, 0			, SCMD_TASTE		, 0.0f	,0 	},
	    {	"teleport"	, "tel"	, POS_DEAD		, do_teleport		, LVL_APPR	, 0					, 0.0f	,0 	},
	    {	"thaw"		, "thaw", POS_DEAD		, do_wizutil		, LVL_FREEZE, SCMD_THAW			, 0.0f	,0 	},
	    {	"time"		, "tim"	, POS_DEAD		, do_time			, 0			, 0					, 0.0f	,0 	},
	    {	"toggle"	, "tog"	, POS_DEAD		, do_toggle			, 0			, 0					, 0.0f	,0 	},
	    {	"track"		, "tr"	, POS_STANDING	, do_track			, 0			, 0					, 1.0f	,0 	},
	    {	"transfer"	, "tran", POS_SLEEPING	, do_trans			, LVL_APPR	, 0					, 0.0f	,0 	},

	    {	"unlock"	, "unl"	, POS_SITTING	, do_gen_door		, 0			, SCMD_UNLOCK		, 0.0f	,0		},
	    {	"ungroup"	, "ung"	, POS_DEAD		, do_ungroup		, 0			, 0					, 0.0	,0		},
	    {	"untie"		, "unt"	, POS_SITTING	, do_untie			, 0			, 0					, 0.0f	,0		},
	    {	"unban"		, "unb"	, POS_DEAD		, do_unban			, LVL_GRGOD	, 0					, 0.0f	,0		},
	    {	"unaffect"	, "una"	, POS_DEAD		, do_wizutil		, LVL_GOD	, SCMD_UNAFFECT		, 0.0f	,0		},
		{	"uptime"	, "upt"	, POS_DEAD		, do_date			, LVL_IMMORT, SCMD_UPTIME		, 0.0f	,0		},
		{	"use"		, "us"	, POS_SITTING	, do_use			, 0			, SCMD_USE			, 0.0f	,0		},
		{	"users"		, "user", POS_DEAD		, do_users			, LVL_GRGOD	, 0					, 0.0f	,0		},

		{	"value"		, "val"	, POS_STANDING	, do_not_here		, 0			, 0					, 0.0f	, 0 	},
		{	"version"	, "ver"	, POS_DEAD		, do_gen_ps			, 0			, SCMD_VERSION		, 0.0f	, 0 	},
		{	"visible"	, "vis"	, POS_RESTING	, do_visible		, 1			, 0					, 0.0f	, 0 	},
		{	"view"		, "vie"	, POS_DEAD		, do_view			, 0			, 0					, 0.0f	, 0 	},
		{	"vnum"		, "vnum", POS_DEAD		, do_vnum			, LVL_IMMORT, 0					, 0.0f	, 0 	},
		{	"vstat"		, "vsta", POS_DEAD		, do_vstat			, LVL_GOD	, 0					, 0.0f	, 0 	},

		{	"wake"		, "wak"	, POS_SLEEPING	, do_wake			, 0			, 0					, 0.0f	, 0 	},
		{	"warn"		, "warn", POS_DEAD		, do_warn			, LVL_APPR	, 0					, 0.0f	, 0 	},
		{	"warrant"	, "war"	, POS_DEAD		, do_warrant		, 0			, 0					, 0.0f	, 0 	},
		{	"warrants"	, "warr", POS_DEAD		, do_warrants		, LVL_APPR	, 0					, 0.0f	, 0 	},
		{	"wear"		, "wea"	, POS_RESTING	, do_wear			, 0			, 0					, 0.0	, 0 	},
		{	"weather"	, "weat", POS_RESTING	, do_weather		, 0			, 0					, 0.0	, 0 	},
		{	"wedit"		, "wed"	, POS_DEAD		, do_wedit			, LVL_GRGOD	, 0					, 0.0f	, 0 	},
		{	"where"		, "whe"	, POS_RESTING	, do_where			, 1			, 0					, 0.0	, 0 	},
		{	"whirlwind"	, "whi"	, POS_FIGHTING	, do_whirlwind		, 1			, 0					, 3.0f	, 0 	},
		{	"whisper"	, "whis", POS_RESTING	, do_spec_comm		, 0			, SCMD_WHISPER		, 0.0	, 0 	},
		{	"wield"		, "wie"	, POS_RESTING	, do_wield			, 0			, 0					, 0.0	, 0 	},
		{	"wimpy"		, "wim"	, POS_DEAD		, do_wimpy			, 0			, 0					, 0.0	, 0 	},
		{	"withdraw"	, "with", POS_STANDING	, do_not_here		, 1			, 0					, 0.0	, 0 	},
		{	"wiznet"	, "wiz"	, POS_DEAD		, do_wiznet			, LVL_IMMORT, 0					, 0.0	, 0 	},
		{	";"			, ";"	, POS_DEAD		, do_wiznet			, LVL_IMMORT, 0					, 0.0	, 0 	},
		{	"wizhelp"	, "wizh", POS_SLEEPING	, do_commands		, LVL_IMMORT, SCMD_WIZHELP		, 0.0	, 0 	},
		{	"wizlist"	, "wizl", POS_DEAD		, do_gen_ps			, 0			, SCMD_WIZLIST		, 0.0	, 0 	},
		{	"wizlock"	, "wizlo",POS_DEAD		, do_wizlock		, LVL_IMPL	, 0					, 0.0	, 0 	},
		{	"wotmud"	, "wot"	, POS_DEAD		, do_wotmud			, LVL_IMMORT, 0					, 0.0	, 0		},
		{	"write"		, "wr"	, POS_STANDING	, do_write			, 1			, 0					, 0.0	, 0 	},
		{	"wshow"		, "shoo", POS_DEAD		, do_wshow			, LVL_IMMORT, 0					, 0.0	,  0 	},

		{	"yell"		, "yell", POS_RESTING	, do_gen_comm		, 0			, SCMD_YELL			, 0.0	, 0 	},

		{	"zap"		, "z"	, POS_DEAD		, do_zap			, LVL_GOD	, 0					, 0.0	, 0 	},
		{	"zedit"		, "zed"	, POS_DEAD		, do_olc			, LVL_BLDER	, SCMD_OLC_ZEDIT	, 0.0	, 0		},
		{	"zlist"		, "zl"	, POS_DEAD		, do_zlist			, LVL_BLDER	, 0					, 0.0f	, 0		},
		{	"zreset"	, "zre"	, POS_DEAD		, do_zreset			, LVL_BLDER	, 0					, 0.0	, 0 	},
		{	"zoneban"	, "zon"	, POS_DEAD		, do_wizutil		, LVL_GRGOD	, SCMD_ZONE_BAN		, 0.0	, 0		},

		/* this must be last */
	    {	"\n"		, "zzzzzzz"	, 0			, 0					, 0			, 0					, 0.0	, 0				}
    };

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
	CommandInterpreter( this, Format );
	va_end( args );
}

std::string GetCommand( char *argument )
{
	char cmd[ MAX_INPUT_LENGTH ];
	int i = 0;

	OneArgument( argument, cmd );

	for ( i = 0;complete_cmd_info[ i ].command[0] != '\n';++i )
	{
		if ( !strn_cmp( complete_cmd_info[ i ].command, cmd, strlen( cmd ) ) )
			return complete_cmd_info[ i ].command;
	}
	return 0;
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
	if( GET_LEVEL( this ) >= complete_cmd_info[ nr ].minimum_level )
		return true;
	if( HasMagicMarker() && complete_cmd_info[nr].command_pointer == do_oload )
		return true;
	if( HasMagicWand() && complete_cmd_info[nr].command_pointer == do_olc 
	&& complete_cmd_info[nr].subcmd == SCMD_OLC_OEDIT )
		return true;
	return false;
}


// This is the actual command interpreter called from gameLoop() in comm.cpp
// It makes sure you are the proper level and position to execute the command,
// then calls the appropriate function.
void CommandInterpreter( Character *ch, char *argument )
{
	int cmd, length;
	char *line, arg[MAX_INPUT_LENGTH];

	if( ch->IsPurged() ) return;

	if ( ch->timer > 0.0f )
		ch->CancelTimer( true );

	/* just drop to next line for hitting CR */
	skip_spaces( &argument );

	if ( !*argument )
		return ;

	std::vector< std::string > vArgs = StringUtil::getArgVector( argument );

#ifdef KINSLAYER_JAVASCRIPT
	/* otherwise, find the command */
	if ( ( !ch->ignoreCommandTrigger ) && (
		str_cmp(arg,"override")
		&& js_command_triggers(ch, vArgs[ 0 ].c_str(), argument, true) 
		))
	{//command trigger took over
		return ;
	}
#endif

	if( ch->desc ) {

		performAlias( ch->desc, argument );
	}

	skip_spaces(&argument);

	/*
	 * special case to handle one-character, non-alphanumeric commands;
	 * requested by many people so "'hi" or ";godnet test" is possible.
	 * Patch sent by Eric Green and Stefan Wasilewski.
	 */
	if ( !isalpha( *argument ) )
	{
		arg[ 0 ] = argument[ 0 ];
		arg[ 1 ] = '\0';
		line = argument + 1;
	}
	else
		line = AnyOneArg( argument, arg );

#ifdef KINSLAYER_JAVASCRIPT

	/* otherwise, find the command */
	if ( ( !ch->ignoreCommandTrigger ) && (
		str_cmp(arg,"override")
		&& js_command_triggers(ch, arg, argument, false) 
		))
	{//command trigger took over
		return ;
	}
#endif

	//This will get set to true in an override command. Always set false once we bypass the above check.
	ch->ignoreCommandTrigger = false;
	for ( length = strlen( arg ), cmd = 0; complete_cmd_info[ cmd ].command[0] != '\n';++cmd )
	{
		if ( !strn_cmp( complete_cmd_info[ cmd ].command, arg, length ) &&
			ch->HasAccessToCommand( cmd ) )
		{
			ch->cmd_no = cmd;
			break;
		}
	}
	if ( special( ch, arg, line ) )
		return ;

	if (complete_cmd_info[cmd].command_pointer != do_where
	&& complete_cmd_info[cmd].command_pointer != do_backstab
	&& complete_cmd_info[cmd].command_pointer != do_scan
	&& complete_cmd_info[cmd].command_pointer != do_diagnose
	&& complete_cmd_info[cmd].command_pointer != do_examine
	&& complete_cmd_info[cmd].command_pointer != do_consider
	&& complete_cmd_info[cmd].command_pointer != do_stat
	&& complete_cmd_info[cmd].command_pointer != do_look)
		REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );

	if ( complete_cmd_info[ cmd ].command[0] == '\n' )
	{
		ch->Send( "What?!?\r\n" );
	}
	else if ( PLR_FLAGGED( ch, PLR_FROZEN ) && GET_LEVEL( ch ) < LVL_IMPL )
	{
		ch->Send( "You try, but the mind-numbing cold prevents you...\r\n" );
	}
	else if ( complete_cmd_info[ cmd ].command_pointer == NULL )
	{
		ch->Send( "Sorry, that command hasn't been implemented yet.\r\n" );
	}
	else if ( DisabledCommands.find(cmd) != DisabledCommands.end() )
	{
		ch->Send( "That command has been disabled.\r\n" );
	}
	else if ( GET_POS( ch ) < complete_cmd_info[ cmd ].minimum_position )
	{
		switch ( GET_POS( ch ) )
		{
		case POS_DEAD:
			ch->Send( "Lie still; you are DEAD!!!\r\n" );
			break;
		case POS_INCAP:
		case POS_MORTALLYW:
			ch->Send( "You are in a pretty bad shape, unable to do anything!\r\n" );
			break;
		case POS_STUNNED:
			ch->Send( "All you can do right now is think about the stars!\r\n" );
			break;
		case POS_SLEEPING:
			ch->Send( "In your dreams, or what?\r\n" );
			break;
		case POS_RESTING:
			ch->Send( "Nah... You feel too relaxed to do that..\r\n" );
			break;
		case POS_SITTING:
			ch->Send( "Maybe you should get on your feet first?\r\n" );
			break;
		case POS_FIGHTING:
			ch->Send( "No way!  You're fighting for your life!\r\n" );
			break;
		}
	}
	else
	{
		if ( ( !str_cmp( complete_cmd_info[ cmd ].command, "channel" )
		        || !str_cmp( complete_cmd_info[ cmd ].command, "search" ) ) )
		{
			ch->delayed_command = argument;
		}

		if ( complete_cmd_info[ cmd ].timer != 0.0 && !ch->command_ready )
			ch->SetupTimer( argument, complete_cmd_info[ cmd ].timer );

		ch->Command = complete_cmd_info[ cmd ].command;

		( ( *complete_cmd_info[ cmd ].command_pointer ) ( ch, line, cmd, complete_cmd_info[ cmd ].subcmd ) );

		if ( !ch->command_ready )
			ch->CancelTimer( false );
	}
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

#ifdef KINSLAYER_JAVASCRIPT
	if (this->delayed_command == "delayed_javascript" && this->delayed_script)
	{
		JSManager::get()->execute_timer(this->delayed_script, false);
		this->delayed_script.reset();
		isKJS = true;
	}	
#endif
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
		this->Send( "Cancelled...\r\n" );
}

/**************************************************************************
 * Routines to handle aliasing                                             *
  **************************************************************************/

// The interface to the outside world: do_alias
ACMD( do_alias )
{
	char *repl;
	char arg[MAX_INPUT_LENGTH];

	if ( IS_NPC( ch ) )
		return ;

	repl = AnyOneArg( argument, arg );

	if (!*arg)
	{			// no argument specified -- list currently defined aliases

		ch->Send( "Currently defined aliases:\r\n" );

		if( !ch->aliases.size() )
			ch->Send( " None.\r\n" );
		else
		{
			for(std::map<std::string,std::string>::iterator alias = ch->aliases.begin();alias != ch->aliases.end();++alias)
			{
				ch->Send( "%-15s %s\r\n", alias->first.c_str(), alias->second.c_str() );
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
				ch->Send("Alias deleted.\r\n");
				return;
			}
		}
		else if(!*repl)
		{
			ch->Send("No such alias.\r\n");
		}
		/* otherwise, either add or redefine an alias */
		if ( !str_cmp( arg, "alias" ) )
		{
			ch->Send( "You can't alias 'alias'.\r\n" );
			return;
		}
		ch->aliases[arg] = repl;
		ch->Send( "Alias added.\r\n" );
		MySQLSaveAlias(ch->player.name, ch->aliases.find(arg), false);
	}
}

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
int search_block( char *arg, const char **listy, int exact )
{
	register int i, l;
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
 * inputted std::string is eventually sent to Act().  If you are using user
 * input to produce screen output AND YOU ARE SURE IT WILL NOT BE SENT
 * THROUGH THE Act() FUNCTION (i.e., do_gecho, do_title, but NOT do_say),
 * you can call delete_doubledollar() to make the output look correct.
 *
 * Modifies the std::string in-place.
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

int fill_word( char *argument )
{
	return ( search_block( argument, filler, TRUE ) >= 0 );
}


int reserved_word( char *argument )
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
 * determine if a given std::string is an abbreviation of another
 * (now works symmetrically -- JE 7/25/94)
 *
 * that was dumb.  it shouldn't be symmetrical.  JE 5/1/95
 *
 * returnss 1 if arg1 is an abbreviation of arg2
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

/* Used in specprocs, mostly.  (Exactly) matches "command" to cmd number */
int FindCommand( const char *command )
{
	int cmd;

	for ( cmd = 0; complete_cmd_info[ cmd ].command[0] != '\n'; cmd++ )
		if ( !strcmp( complete_cmd_info[ cmd ].command, command ) )
			return cmd;

	return -1;
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
	register Object * i;
	register Character *k;
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

int _parse_name( char *arg, char *name )
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
			k->Send( "\r\nMultiple login detected -- disconnecting.\r\n" );
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
				k->Send( "\r\nThis body has been usurped!\r\n" );
				target = k->character;
				mode = USURP;
			}

			k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
			k->Send( "\r\nMultiple login detected -- disconnecting.\r\n" );
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
		d->Send( "Reconnecting.\r\n" );
		Act( "$n has reconnected.", TRUE, d->character, 0, 0, TO_ROOM );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE, "%s has reconnected.",
			GET_NAME( d->character ) );
		SwitchManager::GetManager().RemoveSwitchByName( d->character->player.name );
		break;

	case USURP:
		d->loggedIn = true;
		d->Send( "You take over your own body, already in use!\r\n" );
		Act( "$n suddenly keels over in pain, surrounded by a white aura...\r\n"
			"$n's body has been taken over by a new spirit!",
			TRUE, d->character, 0, 0, TO_ROOM );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE,
			"%s has re-logged in ... disconnecting old socket.", GET_NAME( d->character ) );
		SwitchManager::GetManager().RemoveSwitchByName( d->character->player.name );
		break;

	case UNSWITCH:
		d->Send( "Reconnecting to unswitched char." );
		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( d->character ) ), TRUE, "%s has reconnected.", GET_NAME( d->character ) );
		break;
	}

	return 1;
}

void Character::StatSelectionMenu( bool notify )
{
	if ( notify )
	{
		this->Send( "Now it is time for you to setup your character's status points. To increase a status point,\r\n"
		            "simply type the first letter of the stat type you wish to increase, followed by a '+' sign.\r\n"
		            "So for example, if you wish to increase your strength, you would type 's+'. The same goes for\r\n"
		            "decreasing, except you use a '-' sign('s-'). When you have the stats you desire, simply type 'done'\r\n\n" );
	}
	this->Send( "Current Stats:   Str: %d[%d], Int: %d[%d], Wis: %d[%d], Dex: %d[%d], Con: %d[%d]\r\n",
                (int)this->real_abils.str, this->CostPerStat( STAT_STR ), (int)this->real_abils.intel,
                 this->CostPerStat( STAT_INT ), (int)this->real_abils.wis, this->CostPerStat( STAT_WIS ),
                (int)this->real_abils.dex, this->CostPerStat( STAT_DEX ), (int)this->real_abils.con,
	            this->CostPerStat( STAT_CON ) );
	this->Send( "\r\n%d points remaining. Type 'done' when you are finished.\r\n", this->Stats );
}

void Descriptor::NewbieMenuFinish()
{
	STATE( this ) = CON_NEWBMSG;
	//this->Send( Conf->operation.NEWBIE_MSG );
	this->Send("Press any key to begin playing!\r\n");
	if ( this->character->GetCon() != this->character->real_abils.con )
	{
		this->character->SetCon( this->character->real_abils.con );
		this->character->ResetHitRolls( true );
		GET_HIT(this->character) = GET_MAX_HIT(this->character);
	}

	SET_BIT_AR( PRF_FLAGS( this->character ), PRF_COLOR_2 );
	SET_BIT_AR( PRF_FLAGS( this->character ), PRF_DISPHP );
	SET_BIT_AR( PRF_FLAGS( this->character ), PRF_DISPMOVE );
	SET_BIT_AR( PRF_FLAGS( this->character ), PRF_DISPMANA );
	SET_BIT_AR( PRF_FLAGS( this->character ), PRF_AUTOEXIT );

	this->character->SetStr(this->character->real_abils.str );
	this->character->SetInt(this->character->real_abils.intel );
	this->character->SetWis(this->character->real_abils.wis );
	this->character->SetDex(this->character->real_abils.dex );
	this->character->restat_time.setTime( time( 0 ) );

	if(playerExists(this->character->player.name))
		MySQLDeleteAll(this->character->player.name);

	this->character->MySQLInsertQuery();
	this->character->Save();
	this->character->CreatePlayerIndex();
}

int getCommandIndex(const std::string &commandText)
{
	for(int index = 0;complete_cmd_info[index].command[0] != '\n';++index)
	{
		if(!str_cmp(commandText, complete_cmd_info[index].command))
			return index;
	}

	return -1;
}

// deal with newcomers and other non-playing sockets
void Descriptor::Nanny( char* arg )
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
#ifdef KINSLAYER_JAVASCRIPT
	case CON_JEDIT:
		JeditParse( this, arg );
		break;
#endif
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

						STATE(this) = CON_GET_NAME;

						pendingSessions.erase(iter);

						pendingSessionFound = true;

						this->session = sessionKey;

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
			if ( ( _parse_name( arg, tmp_name ) ) || strlen( tmp_name ) < 2 ||
			strlen( tmp_name ) > MAX_NAME_LENGTH || !BanManager::GetManager().IsValidName( tmp_name ) ||
			fill_word( strcpy( buf, tmp_name ) ) || reserved_word( buf ) )
			{
				this->Send( "Invalid name, please try another.\r\n"
				         "Name: " );
				return ;
			}
			strcpy(tmp_name, StringUtil::cap(StringUtil::allLower(arg)));//Format the name correctly...

			if( Conf->play.switch_restriction && SwitchManager::GetManager().WillBeMultiplaying( this->host, tmp_name ) )
			{
				this->Send("You are already logged into another character. You must log out before switching.\r\n");
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
					this->Send("You must wait %d minutes, %d seconds before you may log into another character.\r\n",
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
						this->Send( "Invalid name, please try another.\r\nName:\r\n" );
						return ;
					}
					this->character->desc = this;

					this->character->player.name = StringUtil::cap( tmp_name );
					this->Send( "Did I get that right, %s (Y/N)? \r\n", tmp_name );
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
					this->Send( "Welcome back, %s. Please enter your password:", GET_NAME( this->character ) );
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
					this->Send( "Invalid name, please try another.\r\nName: " );
					return ;
				}

				if(!this->character)
					this->character = new Character(CharPlayer);
				this->character->desc = this;
				this->character->player.name = tmp_name;
				this->Send( "Are you certain, %s, that you wish to use this name? (Y/N) ", tmp_name );
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
				this->Send( "Sorry, new characters are not allowed from your site!\r\n" );
				STATE( this ) = CON_CLOSE;
				return ;
			}

			if ( circle_restrict )
			{
				this->Send( "Sorry, new players can't be created at the moment.\r\n" );
				MudLog( NRM, LVL_APPR, TRUE, "Request for new char %s denied from [%s] (wizlock)",
			        GET_NAME( this->character ), this->host );
				STATE( this ) = CON_CLOSE;
				return ;
				}
      
			this->Send( "Please enter a password: ", GET_NAME( this->character ) );
			STATE( this ) = CON_NEWPASSWD;
		}
		else if ( *arg == 'n' || *arg == 'N' )
		{
			this->Send( "Okay, what IS it, then? " );
			this->character->player.name.erase();
			STATE( this ) = CON_GET_NAME;
		}
		else
		{
			this->Send( "Please type Yes or No: " );
		}
		break;
	case CON_PASSWORD:    		/* get pwd for known player      */

		this->EchoOn();		/* turn echo back on */

		if ( !*arg )
			STATE( this ) = CON_CLOSE;
		else
		{
			/*this->EchoOff()*/EchoOffNew(this);
			if (
				(!this->character->PasswordUpdated() &&
				strncmp( CRYPT(arg, this->character->player.passwd.c_str()), GET_PASSWD(this->character).c_str(), 10 )) ||
				(this->character->PasswordUpdated() &&
				GET_PASSWD(this->character).compare( MD5::getHashFromString(arg)) )
				)
			{
				MudLog( BRF, LVL_GOD, TRUE,
			        "Bad PW: %s [%s]", GET_NAME( this->character ), this->host );

				++this->character->PlayerData->bad_pws;
				this->character->BasicSave();
				if ( ++( this->bad_pws ) >= max_bad_pws )
				{	/* 3 strikes and you're out. */
					this->Send( "Wrong password... disconnecting.\r\n" );
					STATE( this ) = CON_CLOSE;
				}
				else
				{
					this->Send( "Wrong password.\r\nTry again: " );
//					this->EchoOff();
				}

				return ;
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

				this->Send( "Sorry, this char has not been cleared for login from your site!\r\n" );
				STATE( this ) = CON_CLOSE;
				MudLog( NRM, LVL_GOD, TRUE,
					"Connection attempt for %s denied from %s", GET_NAME( this->character ), this->host );
				return;
			}
			if ( GET_LEVEL( this->character ) < circle_restrict )
			{
				this->Send( "The game is temporarily restricted.. try again later.\r\n" );
				STATE( this ) = CON_CLOSE;
				MudLog( NRM, LVL_GOD, TRUE, "Request for login denied for %s [%s] (wizlock)",
			        GET_NAME( this->character ), this->host );
				return ;
			}

			/* check to make sure that no other copies of this player are logged in */
			if ( performDupeCheck( this ) )
				return;

			MudLog( BRF, MAX( LVL_GRGOD, GET_INVIS_LEV( this->character ) ), TRUE,
		        "%s [%s] has connected.", GET_NAME( this->character ), this->host );

			if ( !this->character->NeedsRestat() )
			{
				if ( GET_LEVEL( this->character ) >= LVL_IMMORT )
					this->Send( imotd );
				else
					this->Send( motd );
				if ( load_result )
				{
					this->Send( "\r\n\r\n\007\007\007"
				         "%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
				         COLOR_RED( this->character, CL_SPARSE ), load_result,
				         ( load_result > 1 ) ? "S" : "", COLOR_NORMAL( this->character, CL_SPARSE ) );
					this->character->PlayerData->bad_pws = 0;
				}
			}

			if ( this->character->NeedsRestat() )
			{
				this->character->real_abils.intel = this->character->StatMinimum( STAT_INT );
				this->character->real_abils.wis = this->character->StatMinimum( STAT_WIS );
				this->character->real_abils.dex = this->character->StatMinimum( STAT_DEX );
				this->character->real_abils.str = this->character->StatMinimum( STAT_STR );
				this->character->real_abils.con = this->character->StatMinimum( STAT_CON );
				this->character->Stats = this->character->NumOfStats();

				this->character->Send( "%s%sA major change has occured with the statting system."
                    " You are being given the opportunity to setup your stats again.%s\r\n\n",
                    COLOR_RED( this->character, CL_SPARSE ), COLOR_BOLD( this->character, CL_SPARSE ),
                    COLOR_NORMAL( this->character, CL_SPARSE ) );

				this->character->StatSelectionMenu( true );
				STATE( this ) = CON_STATEDIT;
				return ;
			}
			STATE( this ) = CON_MENU;
			goto menu;
		}

	case CON_NEWPASSWD:
	case CON_CHPWD_GETNEW:
		if ( !*arg || strlen( arg ) > MAX_PWD_LENGTH || strlen( arg ) < 3 ||
        !str_cmp( arg, GET_NAME( this->character ) ) )
		{
			this->Send( "\r\nIllegal password.\r\nTry again:" );
			return;
		}

		this->character->player.passwd = MD5::getHashFromString( arg );
		this->Send( "\r\nPlease verify your password: " );

		if ( STATE( this ) == CON_NEWPASSWD )
			STATE( this ) = CON_CNFPASSWD;
		else
			STATE( this ) = CON_CHPWD_VRFY;
		break;

	case CON_CNFPASSWD:
	case CON_CHPWD_VRFY:
		if( this->character->player.passwd.compare( MD5::getHashFromString( arg ) ) )
		{
			this->Send( "\r\nPasswords don't match... start over.\r\n" );
			this->Send( "Password: " );

			if ( STATE( this ) == CON_CNFPASSWD )
				STATE( this ) = CON_NEWPASSWD;
			else
				STATE( this ) = CON_CHPWD_GETNEW;
			return ;
		}
		this->character->PasswordUpdated( true );//This will inform future logins that the PW is using m5, not crypt

		this->EchoOn();
		if ( STATE( this ) == CON_CNFPASSWD )
		{
			this->Send( "What is your gender: [M]ale or [F]emale? " );
			STATE( this ) = CON_QSEX;
		}
		else
		{
			this->EchoOn();
			this->Send( "\r\nDone.\r\n" );
			STATE( this ) = CON_MENU;
		}

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
			this->Send( "Invalid sex...\r\nTry again: " );
			return ;
		}
		this->Send( race_menu );
		this->Send( "Race:\r\n" );
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
			this->Send( "\r\nThat's not a race.\r\nRace: " );
			return ;
		}

		if ( GET_RACE( this->character ) == RACE_HUMAN || GET_RACE( this->character ) == RACE_AIEL )
			this->Send( human_class_menu );
		else
			this->Send( other_class_menu );

		this->Send( "Class:\r\n" );
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
				this->Send( "That is not a class!\r\nClass:" );
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
				this->Send( "That is not a class!\r\nClass:" );
				return;
			}
#endif
		default:
			GET_CLASS( this->character ) = CLASS_UNDEFINED;
			this->Send( "That is not a class!\r\nClass:" );
			return ;
		}
		STATE( this ) = CON_STAT_OPTION;

		MudLog( NRM, MAX( LVL_APPR, GET_INVIS_LEV( this->character ) ), TRUE,
	        "%s [%s] new player.", GET_NAME( this->character ), this->host );

		this->Send(
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
			this->NewbieMenuFinish();
			break;
		default:
			this->Send("Invalid option: Choose 'A' or 'B' : ");
			return;
		}
		break;
	case CON_STATEDIT:
	{
		if ( strlen( arg ) < 2 )
		{
			this->Send( "You must either type 'done' when you are finished, or choose the first letter of\r\n"
		         "the stat that you wish to increase or decrease followed by either a '+' to increase, or a\r\n"
		         "'-' to decrease.\r\n" );
			return ;
		}
		else if ( !str_cmp( arg, "done" ) )
		{
			this->NewbieMenuFinish();
			return;
		}

		byte* stat;
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
			this->Send( "You inserted an invalid STAT type to increase. You must choose either S, I, W, D, or C.\r\n"
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
			this->Send( "The stat you choose must be followed by either a '+' or a '-', depending on whether you wish to\r\n"
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
		loggedIn = true;
		int tmp_high;
		this->character->LoadWards();
		this->character->loadItems();

		//We need to update the player's points over the time they were not logged in.
		time_t timeDiff = (time(0) - this->character->points.last_logout);

		//Regen no more than 60 tics.
		for(int tics = MIN(60,(timeDiff / SECS_PER_MUD_HOUR));tics > 0;--tics) {
			this->character->PointUpdate(true);
		}

		LAST_LOGON( this->character ) = time( 0 );
		this->character->player.time.logon.setTime(time( 0 ));

		/* with the copyover patch, this next line goes in enter_player_game() */
		this->character->player.idnum = this->character->player.idnum;
		this->Send( "\r\n\n%s\r\n", CONFIG_WELC_MESSG );
		this->character->next = character_list;
		character_list = this->character;

		if ( this->character->NeedsReset() )
		{
			this->character->ResetAllSkills();
			this->Send( "%s%sYour skills have been automatically reset for free as a result of a global reset.%s\r\n",
			    COLOR_RED( this->character, CL_SPARSE ), COLOR_BOLD( this->character, CL_SPARSE ),
				COLOR_NORMAL( this->character, CL_SPARSE ) );
			this->character->reset_time = time( 0 );
		}

		if( (tmp_high = NumPlayers(true,false)) > boot_high )
		{
			UpdateBootHigh( tmp_high );
		}

		if ( !( load_room = FindRoomByVnum( this->character->PlayerData->load_room ) ) )
			load_room = this->character->StartRoom();
		this->character->MoveToRoom( load_room );
		Act( "$n has entered the game.", TRUE, this->character, 0, 0, TO_ROOM );

		MudLog( CMP, MAX( GET_INVIS_LEV( this->character ), LVL_APPR ), TRUE,
	        "%s logging in at room %d.", GET_NAME( this->character ),
		this->character->PlayerData->load_room );
		Switch* sw;
		if( (sw = SwitchManager::GetManager().GetSwitchByIP( this->host )) != NULL )
		{//Notification?
		}

		this->character->AddLogin(this->host, time(0));
		Character *mount = NULL;

		if ( this->character->PlayerData->mount_save > 0 )
		{
			if((mount = new Character(this->character->PlayerData->mount_save, VIRTUAL ))->nr == -1)
			{
				delete mount;
				mount = 0;
			}
			else
			{
				mount->MoveToRoom( load_room );
				if ( this->character->in_room->sector_type == SECT_INSIDE )
					do_follow( mount, ( char* ) GET_NAME( this->character ), 0, 0 );
				else
				{
				MOUNT( this->character ) = mount;
					RIDDEN_BY( mount ) = this->character;
				}
			}
		}

		STATE( this ) = CON_PLAYING;
		if ( !GET_LEVEL( this->character ) )
		{
			this->Send( CONFIG_START_MESSG, this );
		}

		look_at_room( this->character, 0 );

#ifdef KINSLAYER_JAVASCRIPT
		js_enter_game_trigger(character,character);
#endif

		break;
	}
	default:
		Log( "SYSERR: Nanny: illegal state of con'ness (%d) for '%s'; closing connection.",
		     STATE( this ), this->character ? GET_NAME( this->character ) : "<unknown>" );
		STATE( this ) = CON_DISCONNECT;	/* Safest to do. */
		break;
	}
}
