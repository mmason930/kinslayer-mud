/********************************************************************
 *	clans.cpp : Handles major code for the clans of Kinslayer		*
 *																	*
 *	Coded by Galnor in February of 2005 (Sortak@gmail.com)			*
 *																	*
 ********************************************************************/

/***************************************************************
  - March 3, 2005 - Changed Clan::AssignClanPlayers to global void AssignClanPlayers()
	in order to get rid of the lag when clans booted.
****************************************************************/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "olc.h"
#include "clans.h"

#include "ClanUtil.h"
#include "CharacterUtil.h"

Clan *ClanList = NULL, *ClanEnd = NULL;

void affect_modify_ar(Character *ch, sbyte loc, sbyte mod, int bitv[], bool add);

/* Search out a clan by a givin name or vnum, using strn_cmp() */
int GetClanByString(std::string name)
{
	Clan *clan;

	for(clan = ClanList;clan;clan = clan->Next)
	{
		if(!strn_cmp(name, clan->Name, name.size()) || atoi(name.c_str()) == clan->vnum)
			return clan->vnum;
	}

	return 0;
}

int NumClans()
{
	Clan *clan;
	int i;
	
	for(i = 0, clan = ClanList;clan;clan = clan->Next, ++i);
	return i;
}

//Return true if it is a tower related clan, besides Black Ajah
bool IsTowerClan(int clan)
{
	switch(clan)
	{
		case CLAN_GAIDIN:
		case CLAN_WHITE_TOWER:
		case CLAN_GRAY_AJAH:
		case CLAN_RED_AJAH:
		case CLAN_YELLOW_AJAH:
		case CLAN_BROWN_AJAH:
		case CLAN_WHITE_AJAH:
		case CLAN_GREEN_AJAH:
		case CLAN_BLUE_AJAH:
			return true;
		default:
			return false;
	}

	return false;
}

//Delete all of the player's clans.
void Character::removeFromClan()
{
	while(!userClans.empty())
	{
		delete userClans.front();
		userClans.pop_front();
	}
}

//Delete the clan by certain vnum.
void Character::removeFromClan(const int clanId)
{
	Object *obj = 0;

	for(auto userClanIter = userClans.begin();userClanIter != userClans.end();++userClanIter)
	{
		UserClan *userClan = (*userClanIter);
		if(userClan->getClanId() == clanId)
		{
			for(int i = 0; i < NUM_WEARS; ++i)
			{
				obj = GET_EQ(this, i);
				if ( obj && GET_OBJ_CLAN( obj ) == clanId)
				{
					affect_modify_ar(this, APPLY_MOVE, GET_OBJ_CL_MVS(obj), (int *) obj->obj_flags.bitvector, FALSE);
					affect_modify_ar(this, APPLY_HIT, GET_OBJ_CL_HPS(obj), (int *) obj->obj_flags.bitvector, FALSE);
				}
			}
			
			delete userClan;
			userClans.erase(userClanIter);
			break;
		}
	}
}
//Add to clan with certain vnum.
void Character::addToClan(const int clanId)
{
	UserClan *userClan = new UserClan();
	
	userClan->setClanId(clanId);
	userClan->setClannedDatetime(DateTime());
	userClan->setLastRankedDatetime(DateTime());
	userClan->setRank(1);
	userClan->setQuestPoints(0);
	userClan->setIsCouncil(false);
	userClan->setUserId(this->getUserId());

	this->addToClan(userClan);
}

void Character::addToClan(UserClan *userClan)
{
	Object *obj = 0;

	for(int i = 0; i < NUM_WEARS; ++i)
	{
		obj = GET_EQ(this, i);
		if ( obj && GET_OBJ_CLAN( obj ) == userClan->getClanId())
		{
			affect_modify_ar(this, APPLY_MOVE, GET_OBJ_CL_MVS(obj), (int *) obj->obj_flags.bitvector, TRUE);
			affect_modify_ar(this, APPLY_HIT, GET_OBJ_CL_HPS(obj), (int *) obj->obj_flags.bitvector, TRUE);
		}
	}

	userClans.push_back(userClan);
}

//Return true if player has a warrant for any tower clan
bool Character::HasTowerWarrant()
{
	for(int i = 0;i < NumClans();++i)
	{
		if(IsTowerClan(i) && IS_SET_AR(GET_WARRANTS(this), i))
			return true;
	}

	return false;
}

/* This checks to see if this is wanted by ch's clan */
bool Character::wantedByPlayer(Character *ch)
{
	UserClan *userClan;
	Clan *clan;

	for(auto userClanIter = ch->userClans.begin();userClanIter != ch->userClans.end();++userClanIter)
	{
		userClan = (*userClanIter);
		if( (clan = ClanUtil::getClan(userClan->getClanId())) && clan->GetWarrant() &&
		IS_SET_AR(GET_WARRANTS(this), clan->GetWarrant()->vnum))
			return true;
	}

	return false;
}

bool Character::wantedByClan(short clanId)
{
	Clan *clan;

	if(!(clan = ClanUtil::getClan(clanId)) || !(clan->GetWarrant()))
		return false;

	if(IS_SET_AR(GET_WARRANTS(this), clan->GetWarrant()->vnum))
		return true;
	return false;
}

Clan::Clan()
{
	this->Name.erase();
	this->vnum			= 0;
	this->warrant		= -1;
	this->secret		= false;
	this->hidden_level	= 0;
	this->bonus_skill	= 0;
	this->bonus_skill_val = 0;
	this->bonus_skill_rank = 0;
}

Clan::Clan(bool AddToList)
{
	this->Name.erase();
	this->vnum			= 0;
	this->warrant		= -1;
	this->secret		= false;
	this->hidden_level	= 0;
	this->bonus_skill	= 0;
	this->bonus_skill_val = 0;
	this->bonus_skill_rank = 0;

	ADD_END_NODE(ClanList, ClanEnd, this, Prev, Next);
}

Clan::Clan(std::string n, int v)
{
	Clan* C;//For the loop below.
	this->Name			= n;
	this->vnum			= v;
	this->warrant		= -1;
	this->secret		= false;
	this->hidden_level	= 0;
	this->bonus_skill	= 0;
	this->bonus_skill_val = 0;
	this->bonus_skill_rank = 0;

	//Find where in the list we will add this. (ordered by vnum)
	for(C = ClanList;C;C = C->Next)
	{
		if( vnum < C->vnum )
		{
			if( C == ClanList )
				ADD_FIRST_NODE(ClanList, ClanEnd, this, Prev, Next);
			else
			{
				this->Prev = C->Prev;
				C->Prev->Next = this;
				C->Prev = this;
				this->Next = C;
			}
			break;
		}
	}
	if( !C )
		ADD_END_NODE(ClanList, ClanEnd, this, Prev, Next);
}

Clan::~Clan()
{}

void Character::SetWarrant(Character *warranter, int clan, bool hide)
{
	Clan *c;
	if(!(c = ClanUtil::getClan(clan)) || !c->GetWarrant())
		return;

	SET_BIT_AR(GET_WARRANTS(this), c->GetWarrant()->vnum);
	Act("$N has been warranted.", FALSE, warranter, 0, this, TO_CHAR);

	if(!hide)
		this->send("You have been warranted by %s!\r\n", c->GetWarrant()->Name.c_str());
}
void Character::RemoveWarrant(Character *pardoner, int num)
{
	Clan *c;
	if(!(c = ClanUtil::getClan(num)) || !c->GetWarrant())
		return;

	REMOVE_BIT_AR(GET_WARRANTS(this), c->GetWarrant()->vnum);
	Act("$N has been pardoned.", FALSE, pardoner, 0, this, TO_CHAR);
	this->send("You have been pardoned by %s!\r\n", c->GetWarrant()->Name.c_str());
}

void MySQLSaveQuest(const std::string &playername, const Quest *quest, bool update)
{
	std::stringstream Query;
	sql::Query MyQuery;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);

	if(index == NULL)
		return;

	if(update)
		Query	<< "UPDATE quests  SET value = " << quest->var
				<< " WHERE user_id = " << index->id
				<< " AND quest_name = '" << sql::escapeString(quest->name) << "'";
	else
		Query	<< "INSERT INTO quests (user_id, quest_name, value) VALUES ('"
				<< index->id << "', '"
				<< sql::escapeString(quest->name) << "', "
				<< quest->var << ")";

	try {
		MyQuery = gameDatabase->sendQuery(Query.str());
	}
	catch( sql::QueryException e ) {
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
	}
}

Quest *MySQLGrabQuest(const std::string &playername, const std::string &questname)
{
	std::string Query;
	sql::Query MyQuery;
	sql::Row MyRow;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);

	if(index == NULL)
		return NULL;

	Query =	"SELECT quest_name, value FROM quests WHERE user_id = '" + MiscUtil::convert<std::string>(index->id) +
		"' AND quest_name = '" + sql::escapeString(questname) + "'";

	try {
		MyQuery = gameDatabase->sendQuery(Query);
	}
	catch( sql::QueryException e ) {
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return NULL;
	}
	if( !MyQuery->numRows() )
	{
		return NULL;
	}
	Quest *quest = new Quest(MyRow[0], atoi(MyRow[1].c_str()));

	return quest;
}

void Clan::RemoveRank(int Rank)
{
	std::vector<std::string>::iterator iter;
	this->FindRankIterator(Rank, iter);
	this->RankNames.erase(iter);
}

void Clan::GetPlayerList(std::vector< std::list<std::string> > &Players, std::list<std::string> &Council)
{
	char clanstr[16];
	sql::Query MyQuery;
	sql::Row MyRow;
	sprintf(clanstr, "%d", this->vnum);

	std::stringstream Query;
	
	Query << " SELECT "
			 "   users.username,"
		     "   userClan.rank,"
			 "   userClan.is_council,"
		     "   userClan.user_id "
			 " FROM userClan "
			 " LEFT JOIN users ON userClan.user_id=users.user_id "
			 " WHERE userClan.clan_id=" << clanstr << " "
	    <<   " AND users.username IS NOT NULL "
		<<	 " ORDER BY userClan.rank";

	try {
		MyQuery = gameDatabase->sendQuery(Query.str());
	}
	catch( sql::QueryException e ) {
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return;
	}

	for(unsigned int i = 0;i < MyQuery->numRows();++i)
	{
		MyRow = MyQuery->getRow();
		if( atoi(MyRow[1].c_str()) < 1 )
			continue;
		if((size_t) atoi(MyRow[1].c_str()) >= Players.size())
		{
			Players.resize(atoi(MyRow[1].c_str()) + 1);
		}
		Players[atoi(MyRow[1].c_str()) - 1].push_back(MyRow[0]);
		if(atoi(MyRow[2].c_str()))
			Council.push_back(MyRow[0]);
	}
}

void Clan::AddRank(std::string n, int loc)
{
	std::vector<std::string>::iterator iter;

	this->FindRankIterator(loc, iter);
	this->RankNames.insert(iter, n);

	//RankNames.resize(RankNames.size() + 1);
	//RankNames[RankNames.size() - 1] = n;
}

Warrant *Clan::GetWarrant()
{
	std::list<Warrant *>::iterator iter;

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		if( (*iter)->vnum == this->warrant)
			return (*iter);
	}

	return NULL;
}

Warrant *WarrantByVnum(int vnum)
{
	std::list<Warrant *>::iterator iter;

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		if( (*iter)->vnum == vnum)
			return (*iter);
	}

	return NULL;
}

Warrant *WarrantByVnum(std::list<Warrant *> List, int vnum)
{
	std::list<Warrant *>::iterator iter;

	for(iter = List.begin();iter != List.end();++iter)
	{
		if( (*iter)->vnum == vnum)
			return (*iter);
	}

	return NULL;
}


std::string Clan::GetRankName( int r )
{
	return( (--r >= 0 && r < RankNames.size()) ? RankNames[ r ] : "" );
}

void Clan::FindRankIterator(int Rank, std::vector<std::string>::iterator &iter)
{
	std::vector<std::string>::iterator i;

	for(i = this->RankNames.begin(), --Rank;i != this->RankNames.end() && Rank;Rank--, ++i);

	iter = i;
}

void BootClans()
{

	std::stringstream sql;

	sql << "SELECT * FROM clan";

	sql::Query clanQuery = gameDatabase->sendQuery(sql.str());

	while(clanQuery->hasNextRow()) {

		sql::Row row = clanQuery->getRow();

		Clan *clan = new Clan(row["name"], row.getInt("id"));

		clan->hidden_level = row["hidden_level"] == "NULL" ? 0 : row.getInt("hidden_level");
		clan->warrant = row["warrant_id"] == "NULL" ? 0 : row.getInt("warrant_id");
		clan->secret = row["is_secret"] == "NULL" ? 0 : row.getInt("is_secret");
		clan->bonus_skill = row["bonus_skill_id"] == "NULL" ? 0 : row.getInt("bonus_skill_id");
		clan->bonus_skill_val = row["bonus_skill_modifier"] == "NULL" ? 0 : row.getInt("bonus_skill_modifier");
		clan->bonus_skill_rank = row["bonus_skill_rank"] == "NULL" ? 0 : row.getInt("bonus_skill_rank");

		sql.str("");

		sql << "SELECT * FROM clanRank WHERE clan_id=" << clan->vnum;

		sql::Query rankQuery = gameDatabase->sendQuery(sql.str());

		while(rankQuery->hasNextRow()) {

			sql::Row rankRow = rankQuery->getRow();

			clan->AddRank(rankRow.getString("name"), rankRow.getInt("rank_number"));
		}
	}

	Log("Booted %d clans.", NumClans());
}

/* Returns true if the person is in a tower clan other than Gaidin */
bool Character::TOWER_MEMBER()
{

	if ( this->getUserClan( 17 ) )
		return true;

	return false;
}

/* Returns the total questpoints from every clan that a person has earned */
int Character::TotalQP()
{
	int total = 0;

	for(auto userClanIter = userClans.begin();userClanIter != userClans.end();++userClanIter)
	{
		total += (*userClanIter)->getQuestPoints();
	}

	return total;
}

/* This just returns the first clan that a player shares with another player */
int GetSharedClan( Character *p1, Character *p2 )
{
	Clan * clan;

	for(clan = ClanList;clan;clan = clan->Next)
	{
		if ( p1->getUserClan( clan->vnum ) && p2->getUserClan( clan->vnum ) )
			return clan->vnum;
	}
	return 0;
}

Quest *Character::GetQuest(const std::string &QuestName )
{
	for(std::list<Quest *>::iterator quest = this->quests.begin();quest != this->quests.end();++quest)
	{
		if( !str_cmp( QuestName, (*quest)->name ) )
			return (*quest);
	}
	return 0;
}

Quest::Quest()
{
	name.erase();
	var = 0;
}
Quest::Quest(const std::string &QuestName, const short value)
{
	this->name = QuestName;
	this->var = value;
}
Quest::Quest(Character *ch, const std::string &QuestName, const short value)
{
	this->name = QuestName;
	this->var = value;

	if(ch)
		ch->quests.push_back(this);
}

bool Character::isInClan(const int clanId)
{
	return getUserClan(clanId) != NULL;
}

UserClan *Character::getUserClan(short clanId)
{
	return ClanUtil::getUserClanFromList(userClans, clanId);
}

//Galnor: June 02, 2014 - Requested by Bollesedun - Treat all Wolfbrothers as normal humans until rank 5.
bool Character::hasWolfbrotherBonuses()
{
	UserClan *userClan = this->getUserClan(CLAN_WOLFBROTHER);

	return userClan != NULL && userClan->getRank() >= 5;
}