#ifndef KINSLAYER_OLC_H
#define KINSLAYER_OLC_H

/************************************************************************
 * OasisOLC - olc.h						v1.5	*
 *									*
 * Copyright 1996 Harvey Gilpin.				*
 ************************************************************************/

/*
 * If you don't want a short explanation of each field in your zone files,
 * change the number below to a 0 instead of a 1.
 */
#if 0
#define ZEDIT_HELP_IN_FILE
#endif

/*
 * If you want to clear the screen before certain Oasis menus, set to 1.
 */
#if 0
const int CLEAR_SCREEN = 1;
#endif

/*
 * Set this to 1 to enable MobProg support.
 */
#if 0
const int OASIS_MPROG = 1;
#endif

const int NO = 0;
const int YES = 1;
#define CHECK_VAR(var)  ((var == YES) ? "Yes" : "No")
#define TOGGLE_VAR(var)	if (var == YES) { var = NO; } else { var = YES; }
/*
 * Macros, defines, structs and globals for the OLC suite.
 */

const int NUM_ROOM_FLAGS = 26;
const int NUM_ROOM_SECTORS = 11;

const int NUM_MOB_FLAGS = 28;
const int NUM_AFF_FLAGS = 36;
const int NUM_ATTACK_TYPES = 21;

const int NUM_ITEM_TYPES = 20;
const int NUM_ITEM_FLAGS = 17;
const int NUM_ITEM_WEARS = 18;
const int NUM_APPLIES = 29;
const int NUM_LIQ_TYPES = 17;
const int NUM_POSITIONS = 10;
const int NUM_SPELLS = 23;
const int NUM_AGGROS = 7;

const int NUM_GENDERS = 3;
const int NUM_SHOP_FLAGS = 2;
const int NUM_TRADERS = 7;

/*
 * Define this to how many MobProg scripts you have.
 */
const int NUM_PROGS = 12;

/*
 * Aedit Permissions # - set <char> olc 999
 */
const int AEDIT_PERMISSION = 999;

const byte LVL_BUILDER = LVL_IMMORT;

/*
 * Utilities exported from olc.c.
 */
std::string	GetTip(int n);
void	strip_string(char *);
void	cleanup_olc(Descriptor *d, byte cleanup_type);
void	get_char_cols(Character *ch);
void	olc_add_to_save_list(int zone, byte type);
void	olc_remove_from_save_list(int zone, byte type);
void	RemoveIndex(int znum, char *type);

void	redit_save_to_disk(int zone_num);
void	oedit_save_to_disk(int zone_num);

void AddOlcLog( Character *ch, const std::string &Type, const int tid );

void	CleditSetupExisting(Descriptor *d, int vnum);
void	CleditSetupNew(Descriptor *d, int vnum);
void	CleditSaveInternally(Descriptor *d);
void	CleditDispMenu(Descriptor *d);
void	CleditParse(Descriptor *d, const std::string &arg);
void	CleditRankMenu(Descriptor *d);
void	CleditDisplayWarrants(Descriptor *d);

void	WeditParse(Descriptor *d, const std::string &arg);
void	WeditDispMenu(Descriptor *d);

#ifdef KINSLAYER_JAVASCRIPT
void JeditDispMenu( Descriptor *d );
int JScriptParse(Descriptor *d, const std::string &arg);
void JScriptDispMenu(Descriptor *d);
#endif

int		DeleteZone(int vnum);
/*
 * OLC structures.
 */

#define OLC_SCRIPT_EDIT_MODE(d) ((d)->olc->script_mode)

enum atTypes {
	AT_WEAPON=0, AT_ARMOR, AT_MISC
};

#define NUM_ATYPES (3)
extern const char *atTypeStr[4];

class AuctionItem
{
	std::string objShortDesc;
	boost::uuids::uuid objID;

	time_t endTime;
	long long buyout;
	long long starting;
	int id;
	bool is_active;
	bool is_retrieved;
	int ownerID;

	void Zero();
public:
	sql::Query BidQuery;
	AuctionItem();
	AuctionItem( const sql::Row &MyRow );
	~AuctionItem();

	bool IsActive() { return is_active; }
	const time_t GetEndTime() { return endTime; }
	const long long GetBuyout() { return buyout; }
	const long long GetStartingBid() { return starting; }
	const long long GetNextMinBid();
	const long long GetTopBid();
	const boost::uuids::uuid &GetObjID() { return objID; }
	const long long GetLastBid( const int uid );
	int GetID() { return id; }
	int GetOwnerID() { return ownerID; }
	const std::string &GetObjShortDesc() { return objShortDesc; }
	void Refresh();

	bool IsRetrieved() { return is_retrieved; }
	void SetRetrieved() { is_retrieved = true; }

	bool CanBeRetrievedBy( Character *user );

	const std::string GetTopBidderName();
	const int GetTopBidderID();
};

/* Data for the user's interactions with the Auction interface */
struct AuctionData
{
private:
	int item_sub_type;
	std::string SearchTerm;
	int page;
	int sellDuration;
	int buyoutPrice;
	int startingPrice;
	int selectedItem;
	long long bidRequest;

	Object *itemToSell;
public:
	std::vector< AuctionItem* > aiCache;
	std::bitset< 3 > item_types;
	AuctionData();
	~AuctionData();

	void SetItemToSell( Object *o );
	Object *GetItemToSell();
	const int GetSellDuration();
	void SetSellDuration( const int duration );
	const int GetBuyoutPrice( const char t='\0' );
	void SetBuyoutPrice( const unsigned int bp, const char t='\0' );
	const int GetStartingPrice( const char t='\0' );
	void SetStartingPrice( const unsigned int sp, const char t='\0' );

	void SetSearchTerm( const std::string &st );
	const std::string &GetSearchTerm();

	const int GetPage();
	const int NumPages();
	void SetPage( const int _page );

	long long GetBidRequest() { return bidRequest; }
	void SetBidRequest( const long long br ) { bidRequest=br; }

	void ClearCache();
	void ClearSellData();
	void SetSelectedItem( const int _nr );
	AuctionItem *GetSelectedItem();
};

class OLC
{
	public:
		int mode;
		int sgmode;
		int zone_num;
		int number;
		int value;
		int value2;
		int pos;
		int kit_num;
		int script_mode;

		//Weave Editing
		std::string WeaveAttribute;

		Character *mob;
		Room *room;
		Object *obj;
		class Warrant *warrant;
		class Recipe *recipe;
		std::list<Warrant *> WarrantList;
		std::list<Recipe *>	RecipeList;
		class Clan *clan;
		class Zone *zone;
		class Shop *shop;
		class StatGroup *statgroup;
		class Config *config;    /* used for 'cedit'         */
		struct extra_descr_data *desc;
		class Weave *weave;
		class Auction *auction;
		struct AuctionData *auction_data;


#if defined(OASIS_MPROG)

		struct mob_prog_data *mprog;
		struct mob_prog_data *mprogl;
#endif

#ifdef KINSLAYER_JAVASCRIPT
		class JSTrigger *jsTrig;
		std::shared_ptr<std::vector<JSTrigger*> > jsScripts;
#endif
		class Kit *kit;
		int trigger_position;
		int item_type;
		Social *action;
		char *storage; /* for holding commands etc.. */
		int * sg_index_ptr;

		std::list<affected_type*>::iterator aff_iter;
		std::list<class WeaveObject*>::iterator obj_iter;

		int aff_num;
		int obj_num;

		std::vector< byte> sgRolls;
		int sg_race;
		int sg_class;
		int sg_sex;
		int sg_roll;
		int sg_stat;

		class OLCWeaveConfig * olcweaveconfig;
		char *buf;

		OLC();
};

struct olc_save_info
{
	int zone;
	char type;
	struct olc_save_info *next;
};

/*
 * Exported globals.
 */
#ifdef _OASIS_OLC_
char *nrm, *grn, *cyn, *yel, *bld, *mag;
struct olc_save_info *olc_save_list = NULL;
#else
extern char *nrm, *grn, *cyn, *yel, *bld, *mag;
extern struct olc_save_info *olc_save_list;
#endif

/*
 * Descriptor access macros.
 */
#define OLC_MODE(d) 			((d)->olc->mode)
/* Parse input mode.	*/
#define OLC_NUM(d) 				((d)->olc->number)
/* Room/Obj VNUM.		*/
#define OLC_VAL(d) 				((d)->olc->value)
/* Scratch variable.	*/
#define OLC_POS(d)				((d)->olc->pos)
/* Eq position for kits	*/
#define OLC_KNUM(d)				((d)->olc->kit_num)
/* Sub-kit number		*/
#define OLC_ZNUM(d) 			((d)->olc->zone_num)
/* Real zone number.	*/
#define OLC_ROOM(d)				((d)->olc->room)
/* Room structure.		*/
#define OLC_OBJ(d) 				((d)->olc->obj)
/* Object structure.	*/
#define OLC_ZONE(d)				((d)->olc->zone)
/* Zone structure.		*/
#define OLC_MOB(d)				((d)->olc->mob)
/* Mob structure.		*/
#define OLC_SHOP(d) 			((d)->olc->shop)
/* Shop structure.		*/
#define OLC_DESC(d) 			((d)->olc->desc)
/* Extra description.	*/
#define OLC_HELP(d)				((d)->olc->help)

#define OLC_KIT(d)				((d)->olc->kit)
/* Kit structure		*/
#define OLC_STORAGE(d)			((d)->olc->storage)
/* For command storage	*/
#define OLC_ACTION(d)			((d)->olc->action)
/* Action structure		*/
#define OLC_CONFIG(d)			((d)->olc->config)
/* Config structure.	*/
#define OLC_CLAN(d)				((d)->olc->clan)
/* Clan class.			*/
#define OLC_WARRANT(d)			((d)->olc->warrant)
/* Warrant class		*/
#define OLC_SG(d)				((d)->olc->statgroup)
// Weave class
#define OLC_W(d)				((d)->olc->weave)
// affect list iterator
#define OLC_AFF_ITER(d)			((d)->olc->aff_iter)
// which affect we are at
#define OLC_AFF_NUM(d)			((d)->olc->aff_num)
// affect list iterator
#define OLC_OBJ_ITER(d)			((d)->olc->obj_iter)
// which affect we are at
#define OLC_OBJ_NUM(d)			((d)->olc->obj_num)
// config options for wvedit
#define OLC_WCONFIG(d)			((d)->olc->olcweaveconfig)


// Other macros.
#define OLC_EXIT(d)	(OLC_ROOM(d)->dir_option[OLC_VAL(d)])
#define GET_OLC_ZONE(c)	((c)->player_specials->saved.olc_zone)

/*
 * Cleanup types.
 */
const byte CLEANUP_ALL = (byte)	1; //	 Free the whole lot.
const byte CLEANUP_STRUCTS = (byte)	2; //	 Don't free strings.
const byte	CLEANUP_CONFIG = (byte)	3; //	 Cleanup Config
/*
 * Add/Remove save list types.
 */
const byte OLC_SAVE_ROOM   = 	0;
const byte OLC_SAVE_OBJ    = 	1;
const byte OLC_SAVE_ZONE   = 	2;
const byte OLC_SAVE_MOB	   = 	3;
const byte OLC_SAVE_SHOP   = 	4;
const byte OLC_SAVE_HELP   = 	5;
const byte OLC_SAVE_ACTION = 	6;
const byte OLC_SAVE_KIT	   = 	7;
const byte OLC_SAVE_SG     =	8;
const byte OLC_SAVE_WV     =	9;

/*
 * Submodes of KEDIT connectedness.
 */
const int KEDIT_ITEM_PERCENT = -1;
const int KEDIT_CONFIRM_SAVESTRING = 0;
const int KEDIT_MAIN_MENU = 1;
const int KEDIT_NAME = 2;
const int KEDIT_WEAR_LIGHT = 3;
const int KEDIT_WEAR_HOLD = 4;
const int KEDIT_WEAR_NECK_1 = 5;
const int KEDIT_WEAR_NECK_2 = 6;
const int KEDIT_WEAR_HEAD = 7;
const int KEDIT_WEAR_ABOUT = 8;
const int KEDIT_WEAR_BACK = 9;
const int KEDIT_WEAR_BODY = 10;
const int KEDIT_WEAR_ARMS = 11;
const int KEDIT_WEAR_WAIST = 12;
const int KEDIT_WEAR_WRIST_R = 13;
const int KEDIT_WEAR_WRIST_L = 14;
const int KEDIT_WEAR_HANDS = 15;
const int KEDIT_WEAR_FINGER_R = 16;
const int KEDIT_WEAR_FINGER_L = 17;
const int KEDIT_WEAR_WIELD = 18;
const int KEDIT_WEAR_SHIELD = 19;
const int KEDIT_WEAR_LEGS = 20;
const int KEDIT_WEAR_FEET = 21;
const int KEDIT_WEAR_EAR_1 = 22;
const int KEDIT_WEAR_EAR_2 = 23;
const int KEDIT_WEAR_SHOULDERS = 24;
const int KEDIT_INVENTORY = 25;
const int KEDIT_DELETE_ITEM = 26;
const int KEDIT_ADD_ITEM_POS = 27;
const int KEDIT_ITEM_VNUM_INVENTORY = 28;
/*
 * Submodes of OEDIT connectedness.
 */
const int OEDIT_MAIN_MENU = 1;
const int OEDIT_EDIT_NAMELIST = 2;
const int OEDIT_SHORTDESC = 3;
const int OEDIT_LONGDESC = 4;
const int OEDIT_ACTDESC = 5;
const int OEDIT_TYPE = 6;
const int OEDIT_EXTRAS = 7;
const int OEDIT_WEAR = 8;
const int OEDIT_WEIGHT = 9;
const int OEDIT_COST = 10;
const int OEDIT_COSTPERDAY = 11;
const int OEDIT_TIMER = 12;
const int OEDIT_VALUE_1 = 13;
const int OEDIT_VALUE_2 = 14;
const int OEDIT_VALUE_3 = 15;
const int OEDIT_VALUE_4 = 16;
const int OEDIT_VALUE_5 = 17;
const int OEDIT_VALUE_6 = 18;
const int OEDIT_VALUE_7 = 19;
const int OEDIT_VALUE_8 = 20;
const int OEDIT_VALUE_9 = 21;
const int OEDIT_VALUE_10 = 22;
const int OEDIT_VALUE_11 = 23;
const int OEDIT_VALUE_12 = 24;
const int OEDIT_APPLY = 25;
const int OEDIT_APPLYMOD = 26;
const int OEDIT_EXTRADESC_KEY = 27;
const int OEDIT_CONFIRM_SAVEDB = 28;
const int OEDIT_CONFIRM_SAVESTRING = 29;
const int OEDIT_PROMPT_APPLY = 30;
const int OEDIT_EXTRADESC_DESCRIPTION = 31;
const int OEDIT_EXTRADESC_MENU = 32;
const int OEDIT_LEVEL = 33;
const int OEDIT_OB = 34;
const int OEDIT_PB = 35;
const int OEDIT_DB = 36;
const int OEDIT_ABS = 37;
const int OEDIT_WEAP_TYPE = 38;
const int OEDIT_CLAN_1 = 39;
const int OEDIT_CLAN_2 = 40;
const int OEDIT_CLAN_3 = 41;
const int OEDIT_CLAN_4 = 42;
const int OEDIT_CLAN_5 = 43;
const int OEDIT_CLAN_6 = 44;
const int OEDIT_CLAN_7 = 45;
const int OEDIT_CLAN_8 = 46;
const int OEDIT_CLAN_9 = 47;
const int OEDIT_CLAN_10 = 48;
const int OEDIT_DECAYS_MENU = 49;
const int OEDIT_DECAY_TIMER = 50;
const int OEDIT_DECAY_TYPE = 51;
const int OEDIT_DECAY_TIMERTYPE = 52;
const int OEDIT_MAX = 53;

/*
 * Submodes of REDIT connectedness.
 */
const int REDIT_MAIN_MENU = 1;
const int REDIT_NAME = 2;
const int REDIT_DESC = 3;
const int REDIT_FLAGS = 4;
const int REDIT_SECTOR = 5;
const int REDIT_EXIT_MENU = 6;
const int REDIT_CONFIRM_SAVEDB = 7;
const int REDIT_CONFIRM_SAVESTRING = 8;
const int REDIT_EXIT_NUMBER = 9;
const int REDIT_EXIT_DESCRIPTION = 10;
const int REDIT_EXIT_KEYWORD = 11;
const int REDIT_EXIT_KEY = 12;
const int REDIT_EXIT_DOORFLAGS = 13;
const int REDIT_EXIT_PICKREQ = 14;
const int REDIT_HIDDEN = 15;
const int REDIT_EXTRADESC_MENU = 16;
const int REDIT_EXTRADESC_KEY = 17;
const int REDIT_EXTRADESC_DESCRIPTION = 18;
const int REDIT_AUCTION_VNUM = 19;

/*
 * Submodes of ZEDIT connectedness.
 */
const int ZEDIT_MAIN_MENU = 0;
const int ZEDIT_DELETE_ENTRY = 1;
const int ZEDIT_NEW_ENTRY = 2;
const int ZEDIT_CHANGE_ENTRY = 3;
const int ZEDIT_COMMAND_TYPE = 4;
const int ZEDIT_IF_FLAG = 5;
const int ZEDIT_ARG1 = 6;
const int ZEDIT_ARG2 = 7;
const int ZEDIT_ARG3 = 8;
const int ZEDIT_ARG4 = 9;
const int ZEDIT_ARG5 = 10;
const int ZEDIT_ARG6 = 11;
const int ZEDIT_ARG7 = 12;
const int ZEDIT_ZONE_NAME = 13;
const int ZEDIT_ZONE_LIFE = 14;
const int ZEDIT_ZONE_TOP = 15;
const int ZEDIT_ZONE_RESET = 16;
const int ZEDIT_CONFIRM_SAVESTRING = 17;
const int ZEDIT_ZONE_BUILDERS = 18;
const int ZEDIT_X = 19;
const int ZEDIT_Y = 20;
const int ZEDIT_SUNRISE = 21;
const int ZEDIT_SUNSET = 22;

/*
 * Submodes of MEDIT connectedness.
 */
const int MEDIT_MAIN_MENU = 0;
const int MEDIT_ALIAS = 1;
const int MEDIT_S_DESC = 2;
const int MEDIT_L_DESC = 3;
const int MEDIT_D_DESC = 4;
const int MEDIT_NPC_FLAGS = 5;
const int MEDIT_AFF_FLAGS = 6;
const int MEDIT_CONFIRM_SAVESTRING = 7;
/*
 * Numerical responses.
 */
const int MEDIT_NUMERICAL_RESPONSE = 10;
const int MEDIT_SEX = 11;
const int MEDIT_PB = 12;
const int MEDIT_DAMROLL = 13;
const int MEDIT_NDD = 14;
const int MEDIT_SDD = 15;
const int MEDIT_NUM_HP_DICE = 16;
const int MEDIT_SIZE_HP_DICE = 17;
const int MEDIT_ADD_HP = 18;
const int MEDIT_DB = 19;
const int MEDIT_EXP = 20;
const int MEDIT_POS = 21;
const int MEDIT_DEFAULT_POS = 22;
const int MEDIT_ATTACK = 23;
const int MEDIT_LEVEL = 24;
const int MEDIT_OB = 25;
const int MEDIT_RACE = 26;
const int MEDIT_MOVES = 27;
const int MEDIT_CLANS = 28;
#if defined(OASIS_MPROG)
const int MEDIT_MPROG = 29;
const int MEDIT_CHANGE_MPROG = 30;
const int MEDIT_MPROG_COMLIST = 31;
const int MEDIT_MPROG_ARGS = 32;
const int MEDIT_MPROG_TYPE = 33;
const int MEDIT_PURGE_MPROG = 34;
#endif
const int MEDIT_RANK = 35;
const int MEDIT_AGGROS = 36;
const int MEDIT_CLASS = 37;
const int MEDIT_KITS = 38;
const int MEDIT_ARRIVE = 39;
const int MEDIT_EXIT = 40;
const int MEDIT_FOOD_UNIT = 41;
const int MEDIT_FOOD_VNUM = 42;
const int MEDIT_FOOD_QUANTITY = 43;
const int MEDIT_WARRANTS = 44;
const int MEDIT_WARRANTS_NEW = 45;
const int MEDIT_WARRANTS_DELETE = 46;
const int MEDIT_ASSISTS = 47;
const int MEDIT_NEW_ASSIST = 48;
const int MEDIT_DELETE_ASSIST = 49;
const int MEDIT_GOLD = 50;
const int MEDIT_NSECTS = 51;
const int MEDIT_SKIN_UNIT = 52;
const int MEDIT_SKIN_VNUM = 53;
const int MEDIT_SKIN_REQUIRED = 54;

/*
 * Submodes of SEDIT connectedness.
 */
const int SEDIT_MAIN_MENU = 0;
const int SEDIT_CONFIRM_SAVESTRING = 1;
const int SEDIT_NOITEM1 = 2;
const int SEDIT_NOITEM2 = 3;
const int SEDIT_NOCASH1 = 4;
const int SEDIT_NOCASH2 = 5;
const int SEDIT_NOBUY = 6;
const int SEDIT_BUY = 7;
const int SEDIT_SELL = 8;
const int SEDIT_PRODUCTS_MENU = 11;
const int SEDIT_ROOMS_MENU = 12;
const int SEDIT_NAMELIST_MENU = 13;
const int SEDIT_NAMELIST = 14;
/*
 * Numerical responses.
 */
const int SEDIT_NUMERICAL_RESPONSE = 20;
const int SEDIT_OPEN1 = 21;
const int SEDIT_OPEN2 = 22;
const int SEDIT_CLOSE1 = 23;
const int SEDIT_CLOSE2 = 24;
const int SEDIT_KEEPER = 25;
const int SEDIT_BUY_PROFIT = 26;
const int SEDIT_SELL_PROFIT = 27;
const int SEDIT_TYPE_MENU = 29;
const int SEDIT_DELETE_TYPE = 30;
const int SEDIT_DELETE_PRODUCT = 31;
const int SEDIT_NEW_PRODUCT = 32;
const int SEDIT_DELETE_ROOM = 33;
const int SEDIT_NEW_ROOM = 34;
const int SEDIT_SHOP_FLAGS = 35;
const int SEDIT_NOTRADE = 36;

/*
 * Submodes of CEDIT connectedness.
 */
const int CEDIT_MAIN_MENU					= 0;
const int CEDIT_CONFIRM_SAVESTRING			= 1;
const int CEDIT_GAME_OPTIONS_MENU			= 2;
const int CEDIT_CRASHSAVE_OPTIONS_MENU		= 3;
const int CEDIT_OPERATION_OPTIONS_MENU		= 4;
const int CEDIT_DISP_EXPERIENCE_MENU		= 5;
const int CEDIT_ROOM_NUMBERS_MENU			= 6;
const int CEDIT_AUTOWIZ_OPTIONS_MENU		= 7;
const int CEDIT_SKILLS_MENU					= 8;
const int CEDIT_OK							= 9;
const int CEDIT_NOPERSON					= 10;
const int CEDIT_NOEFFECT					= 11;
const int CEDIT_DFLT_IP						= 12;
const int CEDIT_DFLT_DIR					= 13;
const int CEDIT_LOGNAME						= 14;
const int CEDIT_MENU						= 15;
const int CEDIT_WELC_MESSG					= 16;
const int CEDIT_START_MESSG					= 17;
const int CEDIT_WEAVE_MENU					= 18;
const int CEDIT_WEAVE_ATTR_EDIT				= 19;
const int CEDIT_GLOBAL_SCRIPT_MENU			= 20;

/*
 * Numerical responses.
 */
const int CEDIT_NUMERICAL_RESPONSE			= 100;
const int CEDIT_TUNNEL_SIZE					= 101;
const int CEDIT_MAX_EXP_GAIN				= 102;
const int CEDIT_MAX_NPC_CORPSE_TIME			= 103;
const int CEDIT_MAX_PC_CORPSE_TIME			= 104;
const int CEDIT_IDLE_VOID					= 105;
const int CEDIT_IDLE_RENT_TIME				= 106;
const int CEDIT_LOAD_INTO_INVENTORY			= 107;
const int CEDIT_MAX_OBJ_SAVE				= 108;
const int CEDIT_AUTOSAVE_TIME				= 109;
const int CEDIT_CRASH_FILE_TIMEOUT			= 110;
const int CEDIT_RENT_FILE_TIMEOUT			= 111;
const int CEDIT_HUMAN_START_ROOM			= 112;
const int CEDIT_TROLLOC_START_ROOM			= 113;
const int CEDIT_AIEL_START_ROOM				= 114;
const int CEDIT_SEANCHAN_START_ROOM			= 115;
const int CEDIT_IMMORT_START_ROOM			= 116;
const int CEDIT_FROZEN_START_ROOM			= 117;
const int CEDIT_DFLT_PORT					= 118;
const int CEDIT_MAX_PLAYING					= 119;
const int CEDIT_MAX_FILESIZE				= 120;
const int CEDIT_MAX_BAD_PWS					= 121;
const int CEDIT_SITEOK_EVERYONE				= 122;
const int CEDIT_NAMESERVER_IS_SLOW			= 123;
const int CEDIT_MIN_WIZLIST_LEV				= 124;
const int CEDIT_LEVEL_SHOUT					= 125;
const int CEDIT_BASH						= 126;
const int CEDIT_CHARGE						= 127;
const int CEDIT_BACKSTAB					= 128;
const int CEDIT_RIDE						= 129;
const int CEDIT_COVER_TRACKS				= 130;
const int CEDIT_MELEE						= 131;
const int CEDIT_DVO							= 132;
const int CEDIT_BODY_PERCENTS				= 133;
const int CEDIT_TAINT						= 134;
const int CEDIT_TIME_MENU					= 169;
const int CEDIT_MAX_HIT_EXP					= 170;
const int CEDIT_EXP_MULTIPLIER				= 171;
const int CEDIT_TIP_MENU					= 179;
const int CEDIT_TIP_NEW						= 180;
const int CEDIT_TIP_DELETE					= 181;
const int CEDIT_RESET						= 182;
const int CEDIT_RESTAT						= 183;
const int CEDIT_PRECSTRIKE					= 184;
const int CEDIT_HIT_EXP_MULT                = 185;

const int CEDIT_BODY_PERCENT_HEAD			= 800;
const int CEDIT_BODY_PERCENT_SHOULDERS		= 801;
const int CEDIT_BODY_PERCENT_ARMS			= 802;
const int CEDIT_BODY_PERCENT_HANDS			= 803;
const int CEDIT_BODY_PERCENT_BODY			= 804;
const int CEDIT_BODY_PERCENT_WAIST			= 805;
const int CEDIT_BODY_PERCENT_LEGS			= 806;
const int CEDIT_BODY_PERCENT_FEET			= 807;

const int CEDIT_BASH_OMULTIPLIER			= 900;
const int CEDIT_BASH_OBMIMULTIPLIER			= 901;
const int CEDIT_BASH_OWEAPONMULTIPLIER		= 902;
const int CEDIT_BASH_DBASEBONUS				= 903;
const int CEDIT_BASH_DDBMULTIPLIER			= 904;
const int CEDIT_BASH_DBMIMULTIPLIER			= 905;
const int CEDIT_BASH_WEIGHT_MULTIPLIER		= 906;
const int CEDIT_BASH_LEVEL_MULTIPLIER		= 907;
const int CEDIT_BASH_WEAPON_MULTIPLIER		= 908;
const int CEDIT_BASH_OBMIOFFSET				= 909;

const int CEDIT_MELEE_PARRYSPLIT			= 1000;
const int CEDIT_MELEE_WARDMULTIPLIER		= 1001;
const int CEDIT_MELEE_P_PER_O				= 1002;
const int CEDIT_MELEE_P_PER_P				= 1003;
const int CEDIT_MELEE_P_PER_D				= 1004;

const int CEDIT_TAINT_RANDOM_COMMAND		= 1100;
const int CEDIT_TAINT_RANDOM_CHANNEL		= 1101;
const int CEDIT_TAINT_RANDOM_SOCIAL			= 1102;
const int CEDIT_TAINT_RANDOM_TARGET			= 1103;
const int CEDIT_TAINT_STRAIN_TO_TAINT		= 1104;
const int CEDIT_TAINT_SEIZE_COST			= 1105;
const int CEDIT_TAINT_HUNGER_THIRST_COST	= 1106;
const int CEDIT_TAINT_LEVEL_STRAIN_REMOVAL	= 1107;
const int CEDIT_TAINT_WIS_STRAIN_REMOVAL	= 1108;
const int CEDIT_TAINT_BLACK_TOWER_BONUS		= 1109;
const int CEDIT_TAINT_TIMER_COST			= 1110;
const int CEDIT_TAINT_FAILURE_COST			= 1111;
const int CEDIT_TAINT_MANA_COST				= 1112;
const int CEDIT_TAINT_RANDOM_VOICE			= 1113;
const int CEDIT_TAINT_SEIZE_DIZZY			= 1114;

const int CEDIT_FLEE						= 1200;
const int CEDIT_FLEE_UNENGAGED				= 1201;
const int CEDIT_FLEE_ENGAGED				= 1202;
const int CEDIT_FLEE_AUTO					= 1203;
const int CEDIT_FLEE_REGAIN					= 1204;
const int CEDIT_FLEE_BASE					= 1205;
const int CEDIT_FLEE_MAX_LAG				= 1206;
const int CEDIT_FLEE_EXITS					= 1207;

const int CEDIT_CHARGE_OMULTIPLIER			= 1300;
const int CEDIT_CHARGE_OBMIMULTIPLIER		= 1301;
const int CEDIT_CHARGE_OWEAPONMALUS			= 1302;
const int CEDIT_CHARGE_DBASEBONUS			= 1303;
const int CEDIT_CHARGE_DDBMULTIPLIER		= 1304;
const int CEDIT_CHARGE_ONEGATIVESWAY		= 1305;
const int CEDIT_CHARGE_OPOSITIVESWAY		= 1306;
const int CEDIT_CHARGE_TIMER				= 1307;
const int CEDIT_CHARGE_CHARGE_ENGAGED_VICT	= 1308;
const int CEDIT_CHARGE_FINISH_WHILE_ENGAGED	= 1309;
const int CEDIT_CHARGE_START_WHILE_ENGAGED	= 1310;
const int CEDIT_CHARGE_DPULSES_LAG_LAND		= 1311;
const int CEDIT_CHARGE_OPULSES_LAG_LAND		= 1312;
const int CEDIT_CHARGE_OPULSES_LAG_MISS		= 1313;

const int CEDIT_WEAVE_VIEW					= 1400;
const int CEDIT_WEAVE_ATTRIBUTE				= 1401;
const int CEDIT_WEAVE_ADD_ATTRIBUTE			= 1402;
const int CEDIT_WEAVE_DELETE_ATTRIBUTE		= 1403;
const int CEDIT_DELETE_WEAVE				= 1404;
const int CEDIT_ADD_WEAVE_VNUM				= 1405;
const int CEDIT_ADD_WEAVE_NAME				= 1406;

const int CEDIT_MELEE_DODGEMULTIPLIER		= 1600;
const int CEDIT_MELEE_PARRYMULTIPLIER		= 1601;

const int CEDIT_AUCTION_MENU				= 1700;
const int CEDIT_AUCTION_ADD					= 1701;
const int CEDIT_AUCTION_EDIT				= 1702;
const int CEDIT_AUCTION_DELETE				= 1703;
const int CEDIT_AUCTION_EDIT_MENU			= 1704;
const int CEDIT_AUCTION_EDIT_NAME			= 1705;
const int CEDIT_AUCTION_EDIT_RACES			= 1706;
const int CEDIT_AUCTION_EDIT_CLANS			= 1707;

const int CEDIT_WPGAIN_MULTIPLIER			= 1800;
const int CEDIT_WPLOSS_MULTIPLIER			= 1801;

const int CEDIT_GLOBAL_SCRIPT_ADD_VNUM		= 1900;
const int CEDIT_GLOBAL_SCRIPT_REMOVE_VNUM	= 1901;

const int CEDIT_PRECSTRIKE_OBMULT			= 2000;
const int CEDIT_PRECSTRIKE_OFACTOR			= 2001;
const int CEDIT_PRECSTRIKE_WEIGHTMULT		= 2002;
const int CEDIT_PRECSTRIKE_BASEDB			= 2003;
const int CEDIT_PRECSTRIKE_BASHEDMULT		= 2004;
const int CEDIT_PRECSTRIKE_DBMULT			= 2005;
const int CEDIT_PRECSTRIKE_PBMULT			= 2006;

/*
* Limit information.
*/
const size_t MAX_ROOM_NAME		= 75;
const size_t MAX_MOB_NAME		= 50;
const size_t MAX_OBJ_NAME		= 50;
const size_t MAX_KIT_NAME		= 60;
const size_t MAX_ROOM_DESC		= 1024;
const size_t MAX_EXIT_DESC		= 256;
const size_t MAX_EXTRA_DESC		= 512;
const size_t MAX_MOB_DESC		= 512;
const size_t MAX_OBJ_DESC		= 512;

/* Submodes of AEDIT connectedness	*/
const int AEDIT_CONFIRM_SAVESTRING = 0;
const int AEDIT_CONFIRM_EDIT = 1;
const int AEDIT_CONFIRM_ADD = 2;
const int AEDIT_MAIN_MENU = 3;
const int AEDIT_ACTION_NAME = 4;
const int AEDIT_SORT_AS = 5;
const int AEDIT_MIN_CHAR_POS = 6;
const int AEDIT_MIN_VICT_POS = 7;
const int AEDIT_HIDDEN_FLAG = 8;
const int AEDIT_MIN_CHAR_LEVEL = 9;
const int AEDIT_NOVICT_CHAR = 10;
const int AEDIT_NOVICT_OTHERS = 11;
const int AEDIT_VICT_CHAR_FOUND = 12;
const int AEDIT_VICT_OTHERS_FOUND = 13;
const int AEDIT_VICT_VICT_FOUND = 14;
const int AEDIT_VICT_NOT_FOUND = 15;
const int AEDIT_SELF_CHAR = 16;
const int AEDIT_SELF_OTHERS = 17;
const int AEDIT_VICT_CHAR_BODY_FOUND = 18;
const int AEDIT_VICT_OTHERS_BODY_FOUND = 19;
const int AEDIT_VICT_VICT_BODY_FOUND = 20;
const int AEDIT_OBJ_CHAR_FOUND = 21;
const int AEDIT_OBJ_OTHERS_FOUND = 22;

/***** CLAN EDIT STATES	********/
const int CLEDIT_CONFIRM_SAVE = 0;
const int CLEDIT_MENU = 1;
const int CLEDIT_NAME = 2;
const int CLEDIT_RANK_MENU = 3;
const int CLEDIT_WARRANT = 4;
const int CLEDIT_CONFIRM_DELETE = 5;
const int CLEDIT_RANK_INSERT_NUMBER = 6;
const int CLEDIT_RANK_INSERT_NAME = 7;
const int CLEDIT_RANK_DELETE = 8;
const int CLEDIT_RANK_EDIT_NUMBER = 9;
const int CLEDIT_HIDDEN_LEVEL = 10;
const int CLEDIT_BONUS_SKILL = 11;
const int CLEDIT_BONUS_SKILL_VAL = 12;
const int CLEDIT_BONUS_SKILL_RANK = 13;

/***** WARRANT EDIT STATES	********/
const int WEDIT_MENU = 0;
const int WEDIT_CONFIRM_SAVE = 1;
const int WEDIT_NEW_NUMBER = 2;
const int WEDIT_NEW_NAME = 3;
const int WEDIT_DELETE_NUMBER = 4;
const int WEDIT_EDIT_NUMBER = 5;

const int STATEDIT_MAIN_MENU                = 0;
const int STATEDIT_SG_MAIN_MENU             = 1;
const int STATEDIT_RACE                     = 2;
const int STATEDIT_CLASS                    = 3;
const int STATEDIT_SEX                      = 4;
const int STATEDIT_CHOOSE_STAT              = 5;
const int STATEDIT_CHOOSE_ROLL              = 6;
const int STATEDIT_SET_ROLL                 = 7;
const int STATEDIT_CHOOSE_PROBABILITY       = 8;
const int STATEDIT_CONFIRM_STAT_LIST_SAVE   = 9;

/* For the player auction interface */
const int AUCTION_MENU						= 0;
const int AUCTION_BUY_MENU					= 1;
const int AUCTION_SELL_MENU					= 2;
const int AUCTION_SELL_BUYOUT				= 3;
const int AUCTION_BUYOUT_GOLD				= 4;
const int AUCTION_BUYOUT_SILVER				= 5;
const int AUCTION_BUYOUT_COPPER				= 6;
const int AUCTION_SELL_STARTING_PRICE		= 7;
const int AUCTION_STARTING_GOLD				= 8;
const int AUCTION_STARTING_SILVER			= 9;
const int AUCTION_STARTING_COPPER			= 10;
const int AUCTION_ITEM_TO_SELL				= 11;
const int AUCTION_DURATION_MENU				= 12;
const int AUCTION_BUY_SEARCH_TERM			= 13;
const int AUCTION_BUY_TYPE					= 14;
const int AUCTION_BROWSE					= 15;
const int AUCTION_PLACE_BID_MENU			= 16;
const int AUCTION_PLACE_BID					= 17;
const int AUCTION_CONFIRM_BID				= 18;
const int AUCTION_PLACE_NEW_BID_DIALOG		= 19;
const int AUCTION_AUCTION_OVER_DIALOG		= 20;

#ifdef KINSLAYER_JAVASCRIPT
/* Javascript Editor States */
const int JEDIT_MAIN_MENU					= 0;
const int JEDIT_NAME						= 1;
const int JEDIT_ARGS						= 2;
const int JEDIT_NARG						= 3;
const int JEDIT_SAVE_CHANGES				= 4;
const int JEDIT_SCRIPT_BODY					= 5;
const int JEDIT_SELECT_TYPE					= 6;
const int JEDIT_TRIGGER_TYPES				= 7;
const int JEDIT_ALLOW_FLAGS					= 8;
const int JEDIT_OPTION_FLAGS				= 9;

/* Javascript Attachment Editor */
const int OLC_JSCRIPT_EDIT = 82866;
const int JSCRIPT_MAIN_MENU = 0;
const int JSCRIPT_NEW_TRIGGER = 1;
const int JSCRIPT_DEL_TRIGGER = 2;
#endif

#endif
