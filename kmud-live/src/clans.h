/****************************************************************
 *		clans.h, header file for clans for KinslayerMUD			*
 *																*
 *	Coded by Galnor in February of 2005 (Sortak@gmail.com)		*
 *																*
 ****************************************************************/

#ifndef HAS_CLAN_H
#define HAS_CLAN_H

extern class Clan *ClanList;

class PlayerClan;

void BootClans();
int NumClans();
int GetClanByString(std::string name);
bool IsTowerClan(int clan);
Clan *GetRealClan(sh_int vnum);
Warrant *WarrantByVnum(int vnum);
Warrant *WarrantByVnum(std::list<Warrant *> List, int vnum);

//MySQL Quest Stuff
Quest *MySQLGrabQuest(const std::string &playername, const std::string &questname);
void MySQLSaveQuest(const std::string &playername, const Quest *quest, bool update);

//MySQL Clan Stuff
void	MySQLSavePlayerClan(const std::string &playername, PlayerClan *clan, bool update);
void	MySQLDeletePlayerClan(const std::string &playername, const int clan);
void	MySQLDeletePlayerClan(const std::string &playername);
bool	MySQLIsPlayerInClan(const std::string &playername, const int clan_vnum);
PlayerClan *MySQLGrabPlayerClans(const std::string &playername);

class Clan
{
	public:
		int vnum;
		bool secret;
		int warrant;
		int hidden_level;
		int bonus_skill;
		int bonus_skill_val;
		int bonus_skill_rank;
		std::vector<std::string> RankNames;
		std::string Name;
		Clan *Next, *Prev;

		Clan();
		Clan(bool AddToList);
		Clan(std::string n, int v);
		~Clan();
		Warrant *GetWarrant();
		std::string GetRankName( int r );
		void FindRankIterator(int Rank, std::vector<std::string>::iterator &iter);
		void Copy(Clan *source);
		void RemoveRank(int Rank);
		void AddRank(std::string n, int loca);		/* Add a rank */
		void GetPlayerList(std::vector<std::list<std::string> > &Players, std::list<std::string> &Council);
};

class PlayerClan
{
private:
		time_t rank_time;
		time_t clan_time;
		sh_int clan;
		sh_int quest_points;
		byte rank;
		bool council;
		bool altered;
public:
		const std::string GetClanName();
		time_t	GetRankTime();
		time_t	GetClanTime();
		sh_int	GetClanVnum();
		sh_int	GetQuestPoints();
		sh_int	GetRank();
		bool	IsCouncil();
		bool	IsAltered();

//		void	SetRankTime(const time_t rt);
//		void	SetClanTime(const time_t ct);
		void	SetClanVnum(const int vnum);
		void	SetQuestPoints(const int qps);
		void	SetRank(const byte r);
		void	SetCouncil(const bool c);
		void	SetAltered(const bool a);

		PlayerClan();
		PlayerClan(const int vnum);
		PlayerClan( PlayerClan *Source );
		PlayerClan(const sh_int vnum, const time_t rt, const time_t ct, const sh_int qps,
			const byte r, const bool c);
		PlayerClan *next;
};

class Quest
{
	public:
		std::string name;
		sh_int var;

		Quest ();
		Quest(const std::string &QuestName, const short value);
		Quest(Character *ch, const std::string &QuestName, const short value);
};

#endif
