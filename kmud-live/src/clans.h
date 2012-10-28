/****************************************************************
 *		clans.h, header file for clans for KinslayerMUD			*
 *																*
 *	Coded by Galnor in February of 2005 (Sortak@gmail.com)		*
 *																*
 ****************************************************************/

#ifndef HAS_CLAN_H
#define HAS_CLAN_H

extern class Clan *ClanList;

void BootClans();
int NumClans();
int GetClanByString(std::string name);
bool IsTowerClan(int clan);
Warrant *WarrantByVnum(int vnum);
Warrant *WarrantByVnum(std::list<Warrant *> List, int vnum);

//MySQL Quest Stuff
Quest *MySQLGrabQuest(const std::string &playername, const std::string &questname);
void MySQLSaveQuest(const std::string &playername, const Quest *quest, bool update);

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
