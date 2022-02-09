/* ************************************************************************
*   File: db.h                                          Part of CircleMUD *
*  Usage: header file for database handling                               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef DB_H
#define DB_H

#include <thread>
#include <mutex>

extern std::list< Character * > CharPurgeList;
extern std::list< Object *    > ObjPurgeList;
extern std::vector<Object*> obj_proto;

#define LIB_MISC		"misc/"
#define LIB_JS_FILEs	"js_files/"
#define SLASH			"/"

extern std::string DebugFileName;

#define SUF_PLOG			"plog"

// names of various files and directories
#define INDEX_FILE			"index"					// index of world files

#define STDERR				LIB_MISC"MudLog"		// For MudLog file
#define CHEST_LOG_FILE		LIB_MISC"chestlog"		// For the chest logs

template <class _T >
std::string SQLVal( _T t, bool last=false )
{
	std::stringstream RetVal;
	RetVal << "'" << t << "'" << ( (last) ? ("") : "," );
	return RetVal.str();
}

void	boot_db(void);
void	zone_update(void);
int		real_room(unsigned int vnum);
std::string	getNameById(const long id);

bool	playerExists(const std::string &name);

int	CountMobsRoom(int mob_no, Room *room);
int	CountMobsTotal(int mob_no);
int	CountObjectsInv(int number, Character *target);
int	CountObjectsRoom(int number, int room);
void loadScreenText();

extern std::shared_ptr<std::vector<JSTrigger*> > globalJS_Scripts;
void BootGlobalScripts();
void SaveGlobalScripts();

Room *FindRoomByVnum(unsigned int vnum);

bool	MySQLLoadRolls(const std::string &playername, const std::string TableName, std::vector<int> &Rolls);
void	MySQLSaveAlias(const std::string &playername, std::map<std::string, std::string>::iterator a, bool update);
void	MySQLDeleteAlias(const std::string &playername, const std::string &command);
void	MySQLDeleteAll(const std::string &playername);
void	reset_zone(int zone);
void	playerAutoDelete(void);
void	add_to_clan_list(Character *ch, int p);

Object *create_obj(bool incID=true);
void	free_obj(Object *obj);
int		real_object(int vnum);
Object *read_object(int nr, int type, bool new_instance, bool increment_top_id = true);
int		PlayerBaseCount();
void	UpdateLegend();
void	GetDirectoryList(const std::string &Directory, std::list<std::string> &Files, const std::string &FileType);
int		NumPlayers(bool linkless, bool immortals);
void	BootWarrants();
void	SortCommands(void);
void	CreateCommandList(void);

extern void	eatwhite(std::istream &fin);
extern void EatUntil(std::istream &fin, char c, bool EatMarker);
extern std::vector<int>ItemCount;

extern	std::list<Legend *>			Legends;
extern	std::list<std::string>	   *Tips;
extern  std::vector<Room *>			World;
extern	std::vector<Character *>	MobProto;
extern	std::vector<Index *>		MobIndex;

const int REAL = 0;
const int VIRTUAL = 1;

/*Class for the player index */
class PlayerIndex
{
	public:
		std::string name;
		long id;
};

extern std::list<PlayerIndex *> PlayerTable;

//Galnor, 04/26/2009 - Converted to a singleton for better management...
enum BanType
{
	BAN_NOT=0, BAN_NEW, BAN_SELECT, BAN_ALL, BAN_NAME, BAN_SWITCH
};

class BanElement
{
public:
	BanElement();
	BanElement( const BanType t, const time_t d, std::string N, std::string S, const int id=-1 );
	~BanElement();

	BanType GetType();
	time_t GetDate();
	std::string getName();
	std::string GetSite();
	std::string GetTypeStr();

	void SetType(const BanType t);
	void SetDate(const time_t d);
	void setName(const std::string N);
	void SetSite(const std::string S);

	void AddToDatabase();
	void RemoveFromDatabase();

private:
	int dbID;
	BanType	type;
	time_t date;
	std::string Name;
	std::string Site;
};

class BanManager
{
private:
	static BanManager *Self;
	std::list< BanElement * >   BanList;
	std::vector< std::string >  BanTypes;
	
	BanManager();
	~BanManager();
public:
	static BanManager &GetManager();
	std::string GetStringByType( const BanType Type );
	BanType GetTypeByString( const std::string &Type );

	void Boot();
	void save();
	void Reload();

	void AddBan( BanElement *MyNewElement );
	void AddBan( std::string Name );
	std::list< BanElement * >::iterator RemoveBan( BanElement *MyElement );
	bool IsValidName( const std::string &Name );
	BanType IsBanned( const std::string &Host );

	std::list< BanElement * > GetBansByName(const std::string &Name);
	std::list< BanElement * > GetBansByType(const BanType type);
	BanElement *GetBanBySite( const std::string &Site );

	std::string GrabBanList();

	void Free();
};

/* global buffering system */

#ifdef __DB_C__
char	buf[MAX_STRING_LENGTH];
char	buf1[MAX_STRING_LENGTH];
char	buf2[MAX_STRING_LENGTH];
char	arg[MAX_STRING_LENGTH];
#else
extern Config *Conf;
extern char	buf[MAX_PRIMARY_BUFFER_LENGTH];
extern char	buf1[MAX_STRING_LENGTH];
extern char	buf2[MAX_STRING_LENGTH];
extern char	arg[MAX_STRING_LENGTH];
extern long long item_loads;
#endif

extern struct Charge	ChargeData;
extern class  Bash		BashData;
extern struct Melee		MeleeData;
extern class  Taint		TaintData;
extern struct Flee		FleeData;
extern struct PreciseStrike	PreciseStrikeData;

extern sql::Context dbContext;
extern sql::Connection gameDatabase;

extern const char	*OK;
extern const char	*NOPERSON;
extern const char	*NOEFFECT;

extern std::list<Warrant *> Warrants;

#endif
