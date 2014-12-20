#include "../../conf.h"
#include "../../sysdep.h"

#include "../../structs.h"
#include "../../interpreter.h"
#include "../../constants.h"
#include "../../screen.h"
#include "../../StringUtil.h"
#include "../../MiscUtil.h"
#include "../../utils.h"
#include "../../olc.h"
#include "../../js_functions.h"
#include "../../guilds/GuildUtil.h"
#include "../../Descriptor.h"

#include "CommandUtil.h"
#include "CommandInfo.h"
#include "Social.h"

int performAlias( Descriptor *d, char *orig );
int special( Character *ch, char *cmd, char *arg );

CommandUtil *CommandUtil::self;

CommandUtil::CommandUtil()
{
	addCommand(new CommandInfo("north", "n", POS_STANDING, do_move, 0, SCMD_NORTH, 0.0, 0));
	addCommand(new CommandInfo("east", "e", POS_STANDING, do_move, 0, SCMD_EAST, 0.0, 0));
	addCommand(new CommandInfo("south", "s", POS_STANDING, do_move, 0, SCMD_SOUTH, 0.0, 0));
	addCommand(new CommandInfo("west", "w", POS_STANDING, do_move, 0, SCMD_WEST, 0.0, 0));
	addCommand(new CommandInfo("up", "u", POS_STANDING, do_move, 0, SCMD_UP, 0.0, 0));
	addCommand(new CommandInfo("down", "d", POS_STANDING, do_move, 0, SCMD_DOWN, 0.0, 0));

	addCommand(new CommandInfo("at", "at", POS_DEAD, do_at, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("advance", "adv", POS_DEAD, do_advance, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("aedit", "aed", POS_DEAD, do_olc, LVL_APPR, SCMD_OLC_AEDIT, 0.0, 0));
	addCommand(new CommandInfo("alias", "a", POS_DEAD, do_alias, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("assist", "ass", POS_FIGHTING, do_assist, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("ask", "ask", POS_RESTING, do_spec_comm, 0, SCMD_ASK, 0.0, 0));
	addCommand(new CommandInfo("attack", "att", POS_FIGHTING, do_kill, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("autoexit", "autoe", POS_DEAD, do_gen_tog, 0, SCMD_AUTOEXIT, 0.0, 0));
	addCommand(new CommandInfo("autoscan", "autos", POS_DEAD, do_gen_tog, 0, SCMD_AUTOSCAN, 0.0, 0));
	addCommand(new CommandInfo("auction", "auc", POS_RESTING, do_auction, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("award", "aw", POS_DEAD, do_award, 0, 0, 0.0, 0));

	addCommand(new CommandInfo("backstab", "b", POS_FIGHTING, do_backstab, 1, 0, 4.5f, 0));
	addCommand(new CommandInfo("ban", "ban", POS_DEAD, do_ban, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("balance", "bal", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("bash", "bas", POS_FIGHTING, do_bash, 1, 0, 4.2f, 0));
	addCommand(new CommandInfo("block", "blo", POS_FIGHTING, do_shieldblock, 1, 0, 5.0f, 0));
	addCommand(new CommandInfo("break", "bre", POS_DEAD, do_break, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("brief", "br", POS_DEAD, do_gen_tog, 0, SCMD_BRIEF, 0.0, 0));
	addCommand(new CommandInfo("buildwalk", "bui", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_BUILDWALK, 0.0, 0));
	addCommand(new CommandInfo("butcher", "bu", POS_STANDING, do_butcher, 0, SCMD_BUTCHER, 0.0, 0));
	addCommand(new CommandInfo("buy", "buy", POS_STANDING, do_not_here, 0, 0, 0.0, 0));
	
	addCommand(new CommandInfo("channel", "c", POS_DEAD, do_cast, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("cedit", "ced", POS_DEAD, do_cedit, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("check", "ch", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("chat", "cha", POS_RESTING, do_gen_comm, 0, SCMD_CHAT, 0.0, 0));
	addCommand(new CommandInfo("charge", "char", POS_FIGHTING, do_charge, 0, SCMD_CHARGE, 2.25, 0));
	addCommand(new CommandInfo("change", "chang", POS_DEAD, do_change, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("choke", "cho", POS_FIGHTING, do_choke, 0, 0, 4.0, 0));
	addCommand(new CommandInfo("clan", "clan", POS_DEAD, do_clan, 0, 0, 0.0, LVL_APPR));
	addCommand(new CommandInfo("clear", "cle", POS_DEAD, do_gen_ps, 0, SCMD_CLEAR, 0.0, 0));
	addCommand(new CommandInfo("cledit", "cled", POS_DEAD, do_cledit, LVL_GOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("close", "clo", POS_SITTING, do_gen_door, 0, SCMD_CLOSE, 0.0, 0));
	addCommand(new CommandInfo("cls", "cls", POS_DEAD, do_gen_ps, 0, SCMD_CLEAR, 0.0, 0));
	addCommand(new CommandInfo("compel", "compe", POS_STANDING, do_compel, 1, 0, 4, 0));
	addCommand(new CommandInfo("consider", "con", POS_RESTING, do_consider, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("color", "col", POS_DEAD, do_color, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("commands", "comm", POS_DEAD, do_commands, 0, SCMD_COMMANDS, 0.0, 0));
	addCommand(new CommandInfo("compact", "comp", POS_DEAD, do_gen_tog, 0, SCMD_COMPACT, 0.0, 0));
	addCommand(new CommandInfo("copy", "copy", POS_DEAD, do_copy, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("credits", "cre", POS_DEAD, do_gen_ps, 0, SCMD_CREDITS, 0.0, 0));
	addCommand(new CommandInfo("council", "cou", POS_DEAD, do_council, COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL, 0, 0.0, COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL));
	
	addCommand(new CommandInfo("date", "date", POS_DEAD, do_date, LVL_IMMORT, SCMD_DATE, 0.0, 0));
	addCommand(new CommandInfo("dc", "dc", POS_DEAD, do_dc, LVL_GOD, 0, 0.0, LVL_APPR));
	addCommand(new CommandInfo("demote", "dem", POS_DEAD, do_demote, 0, 0, 0.0, LVL_APPR));
	addCommand(new CommandInfo("declan", "dec", POS_DEAD, do_declan, COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL, 0, 0.0, COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL));
	addCommand(new CommandInfo("deny", "den", POS_DEAD, do_deny, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("deposit", "dep", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("diagnose", "dia", POS_RESTING, do_diagnose, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("diceroll", "dic", POS_SITTING, do_diceroll, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("dig", "dig", POS_DEAD, do_dig, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("disable", "disa", POS_DEAD, do_disable, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("dismount", "dis", POS_FIGHTING, do_dismount, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("display", "disp", POS_DEAD, do_display, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("drink", "dr", POS_RESTING, do_drink, 0, SCMD_DRINK, 0.0, 0));
	addCommand(new CommandInfo("draw", "dra", POS_RESTING, do_draw, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("drop", "dro", POS_RESTING, do_drop, 0, SCMD_DROP, 0.0, 0));
	
	addCommand(new CommandInfo("eat", "ea", POS_RESTING, do_eat, 0, SCMD_EAT, 0.0, 0));
	addCommand(new CommandInfo("echo", "echo", POS_SLEEPING, do_echo, LVL_GOD, SCMD_ECHO, 0.0, 0));
	addCommand(new CommandInfo("effuse", "ef", POS_STANDING, do_effuse, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("email", "ema", POS_DEAD, do_email, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("embrace", "emb", POS_RESTING, do_source, 0, SCMD_EMBRACE, 0.0, 0));
	addCommand(new CommandInfo("enable", "ena", POS_DEAD, do_enable, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("enter", "en", POS_STANDING, do_enter, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("equipment", "eq", POS_SLEEPING, do_equipment, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("exits", "exi", POS_RESTING, do_exits, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("examine", "exa", POS_RESTING, do_examine, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("extra", "ext", POS_DEAD, do_extra, LVL_GOD, 0, 0.0, 0));
	
	addCommand(new CommandInfo("flee", "f", POS_FIGHTING, do_flee, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("fade", "fa", POS_STANDING, do_fade, 0, 0, 5, 0));
	addCommand(new CommandInfo("find", "fi", POS_DEAD, do_find, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("force", "for", POS_SLEEPING, do_force, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("fill", "fil", POS_STANDING, do_pour, 0, SCMD_FILL, 0.0, 0));
	addCommand(new CommandInfo("follow", "fol", POS_RESTING, do_follow, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("freeze", "free", POS_DEAD, do_wizutil, LVL_FREEZE, SCMD_FREEZE, 0.0, 0));
	
	addCommand(new CommandInfo("get", "g", POS_RESTING, do_get, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("gcomm", "gc", POS_DEAD, do_gcomm, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("gecho", "gech", POS_DEAD, do_gecho, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("give", "giv", POS_STANDING, do_give, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("global", "glo", POS_RESTING, do_gen_comm, 0, SCMD_GLOBAL, 0.0, 0));
	addCommand(new CommandInfo("globalmute", "globalm", POS_DEAD, do_global_mute, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("goto", "got", POS_SLEEPING, do_goto, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("gold", "gol", POS_RESTING, do_gold, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("group", "gr", POS_RESTING, do_group, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("grant", "gra", POS_DEAD, do_grant, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("grab", "gra", POS_RESTING, do_grab, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("guilds", "gu", POS_RESTING, std::bind(&GuildUtil::guildsCommandHandler, GuildUtil::get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), 0, 0, 0.0, 0));
	
	addCommand(new CommandInfo("hamstring", "ham", POS_FIGHTING, do_hamstring, 1, 0, 4.2f, 0));
	addCommand(new CommandInfo("hide", "hid", POS_STANDING, do_hide, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("hit", "h", POS_FIGHTING, do_hit, 0, SCMD_HIT, 0.0, 0));
	addCommand(new CommandInfo("hold", "ho", POS_RESTING, do_grab, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("holylight", "holy", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_HOLYLIGHT, 1.0, 0));
	
	addCommand(new CommandInfo("inventory", "i", POS_DEAD, do_inventory, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("ignore", "ig", POS_DEAD, do_ignore, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("incognito", "in", POS_DEAD, do_incognito, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("imotd", "imo", POS_DEAD, do_gen_ps, LVL_IMMORT, SCMD_IMOTD, 0.0, 0));
	addCommand(new CommandInfo("insult", "ins", POS_RESTING, do_insult, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("invert", "inve", POS_FIGHTING, do_invert, 0, 0, 2.0, 0));
	addCommand(new CommandInfo("invis", "inv", POS_DEAD, do_invis, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("ipfind", "ip", POS_DEAD, do_ipfind, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("insert", "ip", POS_DEAD, do_insert, LVL_GOD, 0, 0.0, 0));
	
	addCommand(new CommandInfo("jattach", "ja", POS_DEAD, do_jattach, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("jedit", "jed", POS_DEAD, do_jedit, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("jmap", "jm", POS_DEAD, do_jmap, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("jstat", "js", POS_DEAD, do_jstat, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("jslist", "jsl", POS_DEAD, do_jslist, LVL_APPR, 0, 0.0, 0));
	
	addCommand(new CommandInfo("kill", "k", POS_FIGHTING, do_kill, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("kedit", "ke", POS_DEAD, do_olc, LVL_APPR, SCMD_OLC_KEDIT, 0.0, 0));
	addCommand(new CommandInfo("kick", "kic", POS_FIGHTING, do_kick, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("klist", "kl", POS_DEAD, do_klist, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("knock", "kn", POS_STANDING, do_knock, 0, 0, 0.0, 0));
	
	addCommand(new CommandInfo("look", "l", POS_RESTING, do_look, 0, SCMD_LOOK, 0.0, 0));
	addCommand(new CommandInfo("last", "la", POS_DEAD, do_last, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("lag", "lag", POS_DEAD, do_lag, LVL_IMPL, 0, 0.0, 0));
	addCommand(new CommandInfo("lead", "lead", POS_STANDING, do_lead, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("leave", "lea", POS_STANDING, do_leave, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("levels", "lev", POS_DEAD, do_levels, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("list", "li", POS_STANDING, do_not_here, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("lock", "loc", POS_SITTING, do_gen_door, 0, SCMD_LOCK, 0.0, 0));
	addCommand(new CommandInfo("locate", "loca", POS_DEAD, do_locate, 0, 0, 0.0, 0));
	
	addCommand(new CommandInfo("medit", "med", POS_DEAD, do_olc, LVL_BLDER, SCMD_OLC_MEDIT, 0.0f, 0));
	addCommand(new CommandInfo("mark", "mar", POS_FIGHTING, do_mark, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("mbload", "mblo", POS_DEAD, do_mbload, LVL_BLDER, 0, 0.0f, 0));
	addCommand(new CommandInfo("memory", "mem", POS_DEAD, do_memory, LVL_BLDER, 0, 0.0f, 0));
	addCommand(new CommandInfo("mlist", "mli", POS_DEAD, do_mlist, LVL_BLDER, 0, 0.0f, 0));
	addCommand(new CommandInfo("motd", "motd", POS_DEAD, do_gen_ps, 0, SCMD_MOTD, 0.0f, 0));
	addCommand(new CommandInfo("mute", "mut", POS_DEAD, do_wizutil, LVL_APPR, SCMD_SQUELCH, 0.0f, 0));
	addCommand(new CommandInfo("murder", "mur", POS_FIGHTING, do_hit, 0, SCMD_MURDER, 0.0f, 0));
	addCommand(new CommandInfo("myzones", "myz", POS_DEAD, do_myzones, LVL_IMMORT, 0, 0.0f, 0));
	
	addCommand(new CommandInfo("narrate", "nar", POS_RESTING, do_gen_comm, 0, SCMD_NARRATE, 0.0, 0));
	addCommand(new CommandInfo("notice", "not", POS_RESTING, do_notice, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("nochats", "nogr", POS_DEAD, do_gen_tog, 0, SCMD_NOCHAT, 0.0, 0));
	addCommand(new CommandInfo("noglobals", "nog", POS_DEAD, do_gen_tog, 0, SCMD_NOGLOBAL, 0.0, 0));
	addCommand(new CommandInfo("nohassle", "noh", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_NOHASSLE, 0.0, 0));
	addCommand(new CommandInfo("nonarrates", "noa", POS_DEAD, do_gen_tog, 0, SCMD_NONARRATE, 0.0, 0));
	addCommand(new CommandInfo("norepeat", "nor", POS_DEAD, do_gen_tog, 0, SCMD_NOREPEAT, 0.0, 0));
	addCommand(new CommandInfo("noreply", "nore", POS_DEAD, do_noreply, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("nospeaks", "nos", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_NOSPEAKS, 0.0, 0));
	addCommand(new CommandInfo("notell", "note", POS_DEAD, do_gen_tog, LVL_APPR, SCMD_NOTELL, 0.0, 0));
	addCommand(new CommandInfo("notips", "notip", POS_DEAD, do_gen_tog, 0, SCMD_NOTIPS, 0.0, 0));
	addCommand(new CommandInfo("notitle", "noti", POS_DEAD, do_wizutil, LVL_GOD, SCMD_NOTITLE, 0.0, 0));
	addCommand(new CommandInfo("nowiz", "now", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_NOWIZ, 0.0, 0));
	
	addCommand(new CommandInfo("order", "o", POS_RESTING, do_order, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("offer", "off", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("open", "op", POS_SITTING, do_gen_door, 0, SCMD_OPEN, 0.0, 0));
	addCommand(new CommandInfo("oload", "oloa", POS_DEAD, do_oload, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("olc", "olc", POS_DEAD, do_olc, LVL_IMMORT, SCMD_OLC_SAVEINFO, 0.0, 0));
	addCommand(new CommandInfo("olist", "oli", POS_DEAD, do_olist, LVL_GOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("oedit", "oed", POS_DEAD, do_olc, LVL_GOD, SCMD_OLC_OEDIT, 0.0, 0));
	addCommand(new CommandInfo("override", "overr", POS_DEAD, do_override, LVL_IMMORT, 0, 0.0, 0));
	
	addCommand(new CommandInfo("put", "p", POS_RESTING, do_put, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("practice", "pr", POS_RESTING, do_practice, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("page", "pag", POS_DEAD, do_page, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("pardon", "par", POS_DEAD, do_pardon, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("pick", "pi", POS_STANDING, do_gen_door, 1, SCMD_PICK, 0.0, 0));
	addCommand(new CommandInfo("poofin", "poofi", POS_DEAD, do_poofset, LVL_IMMORT, SCMD_POOFIN, 0.0, 0));
	addCommand(new CommandInfo("poofout", "poofo", POS_DEAD, do_poofset, LVL_IMMORT, SCMD_POOFOUT, 0.0, 0));
	addCommand(new CommandInfo("pour", "pour", POS_STANDING, do_pour, 0, SCMD_POUR, 0.0, 0));
	addCommand(new CommandInfo("precisestrike", "pre", POS_FIGHTING, do_precisestrike, 1, 0, 3.0f, 0));
	addCommand(new CommandInfo("prompt", "promp", POS_DEAD, do_display, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("pulverize", "pul", POS_DEAD, do_pulverize, 0, 0, 4.3f, 0));
	addCommand(new CommandInfo("purge", "pur", POS_DEAD, do_purge, LVL_BLDER, 0, 0.0, 0));
	
	addCommand(new CommandInfo("quaff", "q", POS_RESTING, do_use, 0, SCMD_QUAFF, 0.0, 0));
	addCommand(new CommandInfo("qui", "qui", POS_DEAD, do_quit, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("quit", "quit", POS_DEAD, do_quit, 0, SCMD_QUIT, 0.0, 0));
	addCommand(new CommandInfo("qval", "qv", POS_DEAD, do_qval, LVL_APPR, 0, 0.0, 0));
	
	addCommand(new CommandInfo("reply", "r", POS_SLEEPING, do_reply, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("rank", "ran", POS_DEAD, do_rank, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("rage", "rag", POS_FIGHTING, do_rage, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("read", "rea", POS_RESTING, do_look, 0, SCMD_READ, 0.0, 0));
	addCommand(new CommandInfo("rest", "re", POS_RESTING, do_rest, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("reboot", "reb", POS_DEAD, do_countdown, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("release", "rel", POS_RESTING, do_release, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("reload", "rele", POS_DEAD, do_reboot, LVL_IMPL, 0, 0.0, 0));
	addCommand(new CommandInfo("remove", "rem", POS_RESTING, do_remove, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("rent", "ren", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("reroll", "rer", POS_DEAD, do_reroll, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("rescue", "resc", POS_FIGHTING, do_rescue, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("reset", "res", POS_DEAD, do_reset, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("restat", "resta", POS_RESTING, do_restat, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("restore", "resto", POS_DEAD, do_restore, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("return", "ret", POS_DEAD, do_return, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("retrieve", "retr", POS_DEAD, do_retrieve, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("retool", "reto", POS_DEAD, do_retool, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("redit", "redit", POS_DEAD, do_olc, LVL_IMMORT, SCMD_OLC_REDIT, 0.0, 0));
	addCommand(new CommandInfo("ride", "ri", POS_STANDING, do_ride, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("rlist", "rlist", POS_DEAD, do_rlist, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("roomflags", "roomf", POS_DEAD, do_gen_tog, LVL_IMMORT, SCMD_ROOMFLAGS, 0.0, 0));
	
	addCommand(new CommandInfo("say", "sa", POS_RESTING, do_say, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("'", "'", POS_RESTING, do_say, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("scalp", "sca", POS_STANDING, do_scalp, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("speak", "sp", POS_RESTING, do_speak, LVL_BLDER, 0, 0.0, 0));
	addCommand(new CommandInfo("save", "save", POS_SLEEPING, do_save, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("scan", "scan", POS_STANDING, do_scan, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("search", "sea", POS_STANDING, do_search, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("saveall", "savea", POS_DEAD, do_saveall, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("seize", "sei", POS_RESTING, do_source, 0, SCMD_SEIZE, 0.0, 0));
	addCommand(new CommandInfo("sell", "sel", POS_STANDING, do_not_here, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("send", "sen", POS_SLEEPING, do_send, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("sense", "sens", POS_STANDING, do_sense, 0, 0, 5, 0));
	addCommand(new CommandInfo("set", "set", POS_DEAD, do_set, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("sedit", "sedi", POS_DEAD, do_olc, LVL_BLDER, SCMD_OLC_SEDIT, 0.0, 0));
	addCommand(new CommandInfo("selfdelete", "self", POS_DEAD, do_self_delete, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("sheath", "she", POS_RESTING, do_sheath, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("shout", "sho", POS_RESTING, do_gen_comm, 1, SCMD_SHOUT, 0.0, 0));
	addCommand(new CommandInfo("shadowstep", "shad", POS_FIGHTING, do_shadowstep, 1, 0, 2.0, 0));
	addCommand(new CommandInfo("shake", "sha", POS_RESTING, do_action, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("shieldblock", "shie", POS_FIGHTING, do_shieldblock, 1, 0, 5.0f, 0));
	addCommand(new CommandInfo("shiver", "shiv", POS_RESTING, do_action, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("show", "show", POS_STANDING, do_show, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("shutdow", "shutdow", POS_DEAD, do_shutdown, LVL_GRGOD, 0, 0.0, 0));
	addCommand(new CommandInfo("shutdown", "shutdown", POS_DEAD, do_shutdown, LVL_IMPL, SCMD_SHUTDOWN, 0.0, 0));
	addCommand(new CommandInfo("sip", "sip", POS_RESTING, do_drink, 0, SCMD_SIP, 0.0, 0));
	addCommand(new CommandInfo("sit", "sit", POS_RESTING, do_sit, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("skewer", "ske", POS_FIGHTING, do_charge, 0, SCMD_SKEWER, 2.25, 0));
	addCommand(new CommandInfo("skills", "skills", POS_RESTING, do_practice, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("skillset", "skillse", POS_SLEEPING, do_skillset, LVL_GRGOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("skin", "ski", POS_STANDING, do_butcher, 0, SCMD_SKIN, 0.0, 0));
	addCommand(new CommandInfo("sleep", "sl", POS_SLEEPING, do_sleep, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("slist", "sli", POS_DEAD, do_slist, LVL_BLDER, 0, 0.0f, 0));
	addCommand(new CommandInfo("sneak", "snea", POS_STANDING, do_sneak, 1, 0, 0.0f, 0));
	addCommand(new CommandInfo("snoop", "sno", POS_DEAD, do_snoop, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("socials", "soc", POS_DEAD, do_commands, 0, SCMD_SOCIALS, 0.0f, 0));
	addCommand(new CommandInfo("stand", "st", POS_RESTING, do_stand, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("stats", "stat", POS_DEAD, do_stat, 1, 0, 0.0f, 0));
	addCommand(new CommandInfo("statfind", "statf", POS_DEAD, do_statfind, LVL_GOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("statedit", "state", POS_DEAD, do_statedit, LVL_APPR, 0, 0.0, 0));
	addCommand(new CommandInfo("steal", "stea", POS_STANDING, do_steal, 1, 0, 3.0f, 0));
	addCommand(new CommandInfo("strike", "str", POS_FIGHTING, do_precisestrike, 1, 0, 3.0f, 0));
	addCommand(new CommandInfo("switch", "sw", POS_DEAD, do_switch, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("syslog", "sys", POS_DEAD, do_syslog, LVL_APPR, 0, 0.0f, 0));
	
	addCommand(new CommandInfo("tell", "t", POS_RESTING, do_tell, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("tellmute", "tellm", POS_DEAD, do_tell_mute, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("testroll", "testr", POS_DEAD, do_test_roll, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("take", "ta", POS_RESTING, do_get, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("taste", "tas", POS_RESTING, do_eat, 0, SCMD_TASTE, 0.0f, 0));
	addCommand(new CommandInfo("teleport", "tel", POS_DEAD, do_teleport, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("thaw", "thaw", POS_DEAD, do_wizutil, LVL_FREEZE, SCMD_THAW, 0.0f, 0));
	addCommand(new CommandInfo("time", "tim", POS_DEAD, do_time, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("toggle", "tog", POS_DEAD, do_toggle, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("track", "tr", POS_STANDING, do_track, 0, 0, 1.0f, 0));
	addCommand(new CommandInfo("transfer", "tran", POS_SLEEPING, do_trans, LVL_APPR, 0, 0.0f, 0));
	
	addCommand(new CommandInfo("unlock", "un", POS_SITTING, do_gen_door, 0, SCMD_UNLOCK, 0.0f, 0));
	addCommand(new CommandInfo("ungroup", "ung", POS_DEAD, do_ungroup, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("untie", "unt", POS_SITTING, do_untie, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("unban", "unb", POS_DEAD, do_unban, LVL_GRGOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("unaffect", "una", POS_DEAD, do_wizutil, LVL_GOD, SCMD_UNAFFECT, 0.0f, 0));
	addCommand(new CommandInfo("uptime", "upt", POS_DEAD, do_date, LVL_IMMORT, SCMD_UPTIME, 0.0f, 0));
	addCommand(new CommandInfo("use", "us", POS_SITTING, do_use, 0, SCMD_USE, 0.0f, 0));
	addCommand(new CommandInfo("users", "user", POS_DEAD, do_users, LVL_GRGOD, 0, 0.0f, 0));
	
	addCommand(new CommandInfo("value", "val", POS_STANDING, do_not_here, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("version", "ver", POS_DEAD, do_gen_ps, 0, SCMD_VERSION, 0.0f, 0));
	addCommand(new CommandInfo("visible", "vis", POS_RESTING, do_visible, 1, 0, 0.0f, 0));
	addCommand(new CommandInfo("view", "vie", POS_DEAD, do_view, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("vnum", "vnum", POS_DEAD, do_vnum, LVL_IMMORT, 0, 0.0f, 0));
	addCommand(new CommandInfo("vstat", "vsta", POS_DEAD, do_vstat, LVL_GOD, 0, 0.0f, 0));
	
	addCommand(new CommandInfo("wake", "wak", POS_SLEEPING, do_wake, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("warn", "warn", POS_DEAD, do_warn, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("warrant", "war", POS_DEAD, do_warrant, 0, 0, 0.0f, 0));
	addCommand(new CommandInfo("warrants", "warr", POS_DEAD, do_warrants, LVL_APPR, 0, 0.0f, 0));
	addCommand(new CommandInfo("wear", "wea", POS_RESTING, do_wear, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("weather", "weat", POS_RESTING, do_weather, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("wedit", "wed", POS_DEAD, do_wedit, LVL_GRGOD, 0, 0.0f, 0));
	addCommand(new CommandInfo("where", "whe", POS_RESTING, do_where, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("whirlwind", "whi", POS_FIGHTING, do_whirlwind, 1, 0, 3.0f, 0));
	addCommand(new CommandInfo("whisper", "whis", POS_RESTING, do_spec_comm, 0, SCMD_WHISPER, 0.0, 0));
	addCommand(new CommandInfo("wield", "wie", POS_RESTING, do_wield, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("wimpy", "wim", POS_DEAD, do_wimpy, 0, 0, 0.0, 0));
	addCommand(new CommandInfo("withdraw", "with", POS_STANDING, do_not_here, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("wiznet", "wiz", POS_DEAD, do_wiznet, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo(";", ";", POS_DEAD, do_wiznet, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("wizhelp", "wizh", POS_SLEEPING, do_commands, LVL_IMMORT, SCMD_WIZHELP, 0.0, 0));
	addCommand(new CommandInfo("wizlist", "wizl", POS_DEAD, do_gen_ps, 0, SCMD_WIZLIST, 0.0, 0));
	addCommand(new CommandInfo("wizlock", "wizlo", POS_DEAD, do_wizlock, LVL_IMPL, 0, 0.0, 0));
	addCommand(new CommandInfo("wotmud", "wot", POS_DEAD, do_wotmud, LVL_IMMORT, 0, 0.0, 0));
	addCommand(new CommandInfo("write", "wr", POS_STANDING, do_write, 1, 0, 0.0, 0));
	addCommand(new CommandInfo("wshow", "wsh", POS_DEAD, do_wshow, LVL_IMMORT, 0, 0.0, 0));
	
	addCommand(new CommandInfo("yell", "yell", POS_RESTING, do_gen_comm, 0, SCMD_YELL, 0.0, 0));
	
	addCommand(new CommandInfo("zap", "z", POS_DEAD, do_zap, LVL_GOD, 0, 0.0, 0));
	addCommand(new CommandInfo("zedit", "zed", POS_DEAD, do_olc, LVL_BLDER, SCMD_OLC_ZEDIT, 0.0, 0));
	addCommand(new CommandInfo("zlist", "zl", POS_DEAD, do_zlist, LVL_BLDER, 0, 0.0f, 0));
	addCommand(new CommandInfo("zreset", "zre", POS_DEAD, do_zreset, LVL_BLDER, 0, 0.0, 0));
	addCommand(new CommandInfo("zoneban", "zon", POS_DEAD, do_wizutil, LVL_GRGOD, SCMD_ZONE_BAN, 0.0, 0));
}

CommandUtil::~CommandUtil()
{
	for(auto commandInfo : commandInfoVector)
		delete commandInfo;
	for(auto social : socialVector)
		delete social;
}

CommandUtil *CommandUtil::get()
{
	return self == nullptr ? (self = new CommandUtil()) : self;
}

std::vector<CommandInfo *> CommandUtil::getCommandVector()
{
	return commandInfoVector;
}

int CommandUtil::getCommandIndex(const std::string &command)
{
	for(auto index = 0;index < commandInfoVector.size();++index)
	{
		auto commandInfo = commandInfoVector[index];

		if(!str_cmp(commandInfo->command, command))
			return index;
	}

	return -1;
}

void CommandUtil::addCommand(CommandInfo *commandInfo)
{
	commandInfoVector.push_back(commandInfo);
	commandMap[commandInfo->command] = commandInfo;
}

std::vector<CommandInfo *>::iterator CommandUtil::removeAndDeleteCommand(int index)
{
	auto commandInfo = commandInfoVector[index];
	auto removedIter = commandInfoVector.erase(commandInfoVector.begin() + index);
	commandMap.erase(commandInfo->command);
	delete commandInfo;

	return removedIter;
}

void CommandUtil::createCommandList()
{
	//Remove all socials from the list.
	int index = 0;
	for(auto commandIter = commandInfoVector.begin();commandIter != commandInfoVector.end();)
	{
		auto commandInfo = (*commandIter);
		if(commandInfo->is_social)
		{
			commandIter = removeAndDeleteCommand(index);
		}
		else
		{
			++commandIter;
			++index;
		}
	}

	sortSocials();

	for(auto social : socialVector)
	{
		auto commandInfo = new CommandInfo(
			social->command,
			social->sort_as,
			(short)social->min_char_position,
			do_action,
			(short)social->min_level_char,
			0,
			0.00f,
			(short)0);
		
		commandInfo->is_social = true;
		addCommand(commandInfo);
	}

	sortCommands();

	Log( "Command info rebuilt, %d total commands.", (int)commandInfoVector.size() );
}

void CommandUtil::addSocial(Social *social)
{
	socialVector.push_back(social);
}

void CommandUtil::sortSocials()
{
	std::sort(socialVector.begin(), socialVector.end(), [](Social *social1, Social *social2) { return strcmp(social1->sort_as, social2->sort_as) < 1; });
}

Social *CommandUtil::getSocial(int index)
{
	return socialVector[index];
}

Social *CommandUtil::getSocialByAbbreviation(const std::string &input)
{
	for(auto social : socialVector)
	{
		if(IsAbbrev(input.c_str(), social->command))
			return social;
	}

	return nullptr;
}

std::vector<Social *> CommandUtil::getSocialVector()
{
	return socialVector;
}

Social *CommandUtil::getSocialByCommandIndex(int commandIndex)
{
	return commandIndexToSocialMap[commandIndex];
}

void CommandUtil::sortCommands()
{
	//Sort the command vector.
	std::sort(commandInfoVector.begin(), commandInfoVector.end(), [](CommandInfo *commandInfo1, CommandInfo *commandInfo2) {
		return strcmp(commandInfo1->sort_as.c_str(), commandInfo2->sort_as.c_str()) < 1;
	});

	commandIndexToSocialMap.clear();

	//Update the social table, setting the new command index.
	for(auto social : socialVector)
	{
		social->act_nr = getCommandIndex(social->command);
		commandIndexToSocialMap[social->act_nr] = social;
	}
}

void CommandUtil::destroy()
{
	delete self;
}

CommandInfo *CommandUtil::getCommandByName(const std::string &commandName)
{
	return commandMap[commandName];
}

CommandInfo *CommandUtil::getCommandByIndex(const int index)
{
	return commandInfoVector[index];
}

CommandInfo *CommandUtil::getCommandByFullInput(const std::string &fullCommandInput)
{
	char cmd[ MAX_INPUT_LENGTH ];
	
	char *fullCommandInputCopy = str_dup(fullCommandInput.c_str());
	
	OneArgument( fullCommandInputCopy, cmd );

	delete[] fullCommandInputCopy;

	for(auto commandInfo : commandInfoVector)
	{
		if(!strn_cmp(commandInfo->command, cmd, strlen(cmd)))
			return commandInfo;
	}

	return nullptr;
}

int CommandUtil::getNumberOfSocials()
{
	return (int)socialVector.size();
}

void CommandUtil::saveSocials(sql::Connection connection)
{
	connection->sendRawQuery("DROP TABLE IF EXISTS tempSocial");
	connection->sendRawQuery("CREATE TABLE tempSocial LIKE social");

	sql::BatchInsertStatement batchInsertStatement(connection, "tempSocial", 100000);

	batchInsertStatement.addField("command");
	batchInsertStatement.addField("sort");
	batchInsertStatement.addField("hide");
	batchInsertStatement.addField("minimum_victim_position");
	batchInsertStatement.addField("minimum_char_position");
	batchInsertStatement.addField("minimum_char_level");
	batchInsertStatement.addField("char_no_arg");
	batchInsertStatement.addField("others_no_arg");
	batchInsertStatement.addField("char_found");
	batchInsertStatement.addField("others_found");
	batchInsertStatement.addField("vict_found");
	batchInsertStatement.addField("char_body_found");
	batchInsertStatement.addField("others_body_found");
	batchInsertStatement.addField("vict_body_found");
	batchInsertStatement.addField("not_found");
	batchInsertStatement.addField("char_auto");
	batchInsertStatement.addField("others_auto");
	batchInsertStatement.addField("char_obj_found");
	batchInsertStatement.addField("others_obj_found");

	batchInsertStatement.start();

	for(auto social : socialVector)
	{
		batchInsertStatement.beginEntry();

		batchInsertStatement.putString(social->command);
		batchInsertStatement.putString(social->sort_as);
		batchInsertStatement.putInt(social->hide);
		batchInsertStatement.putInt(social->min_victim_position);
		batchInsertStatement.putInt(social->min_char_position);
		batchInsertStatement.putInt(social->min_level_char);
		batchInsertStatement.putString(social->char_no_arg);
		batchInsertStatement.putString(social->others_no_arg);
		batchInsertStatement.putString(social->char_found);
		batchInsertStatement.putString(social->others_found);
		batchInsertStatement.putString(social->vict_found);
		batchInsertStatement.putString(social->char_body_found);
		batchInsertStatement.putString(social->others_body_found);
		batchInsertStatement.putString(social->vict_body_found);
		batchInsertStatement.putString(social->not_found);
		batchInsertStatement.putString(social->char_auto);
		batchInsertStatement.putString(social->others_auto);
		batchInsertStatement.putString(social->char_obj_found);
		batchInsertStatement.putString(social->others_obj_found);

		batchInsertStatement.endEntry();
	}

	batchInsertStatement.finish();

	connection->sendRawQuery("RENAME TABLE social TO socialOld, tempSocial TO social, socialOld TO tempSocial");
	connection->sendRawQuery("DROP TABLE tempSocial");
}

void CommandUtil::bootSocials(sql::Connection connection)
{
	sql::Query query = connection->sendQuery("SELECT * FROM social ORDER BY id");

	while(query->hasNextRow()) {

		sql::Row socialRow = query->getRow();

		Social *social = new Social();

		social->command = str_dup(socialRow.getString("command").c_str());
		social->sort_as = str_dup(socialRow.getString("sort").c_str());
		social->hide = socialRow.getInt("hide");
		social->min_victim_position = socialRow.getInt("minimum_victim_position");
		social->min_char_position = socialRow.getInt("minimum_char_position");
		social->min_level_char = socialRow.getInt("minimum_char_level");
		social->char_no_arg = str_dup(socialRow.getString("char_no_arg").c_str());
		social->others_no_arg = str_dup(socialRow.getString("others_no_arg").c_str());
		social->char_found = str_dup(socialRow.getString("char_found").c_str());
		social->others_found = str_dup(socialRow.getString("others_found").c_str());
		social->vict_found = str_dup(socialRow.getString("vict_found").c_str());
		social->char_body_found = str_dup(socialRow.getString("char_body_found").c_str());
		social->others_body_found = str_dup(socialRow.getString("others_body_found").c_str());
		social->vict_body_found = str_dup(socialRow.getString("vict_body_found").c_str());
		social->not_found = str_dup(socialRow.getString("not_found").c_str());
		social->char_auto = str_dup(socialRow.getString("char_auto").c_str());
		social->others_auto = str_dup(socialRow.getString("others_auto").c_str());
		social->char_obj_found = str_dup(socialRow.getString("char_obj_found").c_str());
		social->others_obj_found = str_dup(socialRow.getString("others_obj_found").c_str());

		addSocial(social);
	}
}

void CommandUtil::interpretCommand(Character *ch, char *argument)
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

	/* otherwise, find the command */
	if ( ( !ch->ignoreCommandTrigger ) && (
		str_cmp(vArgs[ 0 ].c_str(), "override")
		&& js_command_triggers(ch, vArgs[ 0 ].c_str(), argument, true) 
		))
	{//command trigger took over
		return ;
	}

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

	/* otherwise, find the command */
	if ( ( !ch->ignoreCommandTrigger ) && (
		str_cmp(arg,"override")
		&& js_command_triggers(ch, arg, argument, false) 
		))
	{//command trigger took over
		return ;
	}

	//This will get set to true in an override command. Always set false once we bypass the above check.
	ch->ignoreCommandTrigger = false;
	CommandInfo *commandInfo = nullptr;
	for ( length = strlen( arg ), cmd = 0; cmd < commandInfoVector.size();++cmd )
	{
		commandInfo = commandInfoVector[cmd];
		if ( !strn_cmp( commandInfo->command.c_str(), arg, length ) &&
			ch->HasAccessToCommand( cmd ) )
		{
			ch->cmd_no = cmd;
			break;
		}
	}
	if ( special( ch, arg, line ) )
		return ;

	if(cmd == commandInfoVector.size())
	{
		ch->send("What?!?\r\n");
		return;
	}

	if (strcmp(commandInfo->command.c_str(), "where")
	&& strcmp(commandInfo->command.c_str(), "backstab")
	&& strcmp(commandInfo->command.c_str(), "scan")
	&& strcmp(commandInfo->command.c_str(), "diagnose")
	&& strcmp(commandInfo->command.c_str(), "examine")
	&& strcmp(commandInfo->command.c_str(), "consider")
	&& strcmp(commandInfo->command.c_str(), "stat")
	&& strcmp(commandInfo->command.c_str(), "look"))
		REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );

	if(commandInfo == nullptr) {}

	else if ( PLR_FLAGGED( ch, PLR_FROZEN ) && GET_LEVEL( ch ) < LVL_IMPL )
	{
		ch->send( "You try, but the mind-numbing cold prevents you...\r\n" );
	}
	else if ( DisabledCommands.find(cmd) != DisabledCommands.end() )
	{
		ch->send( "That command has been disabled.\r\n" );
	}
	else if ( GET_POS( ch ) < commandInfo->minimum_position )
	{
		get_char_cols(ch);
		switch ( GET_POS( ch ) )
		{
		case POS_DEAD:
			ch->send( "Lie still; you are DEAD!!!\r\n" );
			break;
		case POS_INCAP:
		case POS_MORTALLYW:
			ch->send( "You are in a pretty bad shape, unable to do anything!\r\n" );
			break;
		case POS_STUNNED:
			ch->send( "All you can do right now is think about the stars!\r\n" );
			break;
		case POS_SLEEPING:
			ch->send( "In your dreams, or what?\r\n" );
			if(GET_LEVEL(ch) <= 5)
				ch->send( "Try [%sWAK%s]ing first.\r\n", cyn, nrm );
			break;
		case POS_RESTING:
			ch->send( "Nah... You feel too relaxed to do that..\r\n" );
			if(GET_LEVEL(ch) <= 5)
				ch->send( "Try [%sST%s]anding first.\r\n", cyn, nrm );
			break;
		case POS_SITTING:
			ch->send( "Maybe you should get on your feet first?\r\n" );
			if(GET_LEVEL(ch) <= 5)
				ch->send( "Try [%sST%s]anding first.\r\n", cyn, nrm );
			break;
		case POS_FIGHTING:
			ch->send( "No way!  You're fighting for your life!\r\n" );
			break;
		}
	}
	else
	{
		if ( ( !strcmp( commandInfo->command, "channel" )
		        || !strcmp( commandInfo->command, "search" ) ) )
		{
			ch->delayed_command = argument;
		}

		if ( commandInfo->timer != 0.0 && !ch->command_ready )
			ch->SetupTimer( argument, commandInfo->timer );

		ch->Command = commandInfo->command;

		( ( commandInfo->command_pointer ) ( ch, line, cmd, commandInfo->subcmd ) );

		if ( !ch->command_ready )
			ch->CancelTimer( false );
	}
}