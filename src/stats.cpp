
#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "comm.h"
#include "db.h"
#include "constants.h"
#include "weaves.h"
#include "stats.h"

extern Character * character_list;

// CLASS STAT
Stat::Stat(const std::string _Name, const std::string _Abbrev, const int _ID)
{
    Name    = _Name;
    Abbrev  = _Abbrev;
    StatID  = _ID;
}

const int Stat::GetID()
{
    return this->StatID;
}
const std::string Stat::getName()
{
    return this->Name;
}
const std::string Stat::GetAbbrev()
{
    return this->Abbrev;
}

// CLASS STATGROUP
StatGroup::StatGroup()
{
    for( std::list< Stat >::iterator S = StatManager::GetManager().StatList.begin();
    S != StatManager::GetManager().StatList.end();++S)
    {
        std::vector< byte> Rolls(26);
        ( this->StatRolls[ (*S).GetID() ] ) = Rolls;
    }
}

StatGroup::StatGroup( StatGroup *source )
{
    if(!source) return;

    this->chclass = source->chclass;
    this->race = source->race;
    this->sex = source->sex;

    this->StatRolls.insert( source->StatRolls.begin(), source->StatRolls.end() );
}

StatGroup::StatGroup(const byte _class, const byte _race, const byte _sex)
{
    this->chclass   = _class;
    this->race      = _race;
    this->sex       = _sex;
}

// STATMANAGER
StatManager::StatManager()
{
    this->StatList.push_back( Stat("Strength"       , "Str", STAT_STR ) );
    this->StatList.push_back( Stat("Intelligence"   , "Int", STAT_INT ) );
    this->StatList.push_back( Stat("Wisdom"         , "Wis", STAT_WIS ) );
    this->StatList.push_back( Stat("Dexterity"      , "Dex", STAT_DEX ) );
    this->StatList.push_back( Stat("Constitution"   , "Con", STAT_CON ) );
}

StatManager::~StatManager()
{
	Self = NULL;
}
void StatManager::Free()
{
	if( Self != NULL )
		delete (Self);
}

//Randomly roll a player's stats
bool StatManager::RollStats(Character *ch) //Roll ch's stats.
{
    if(!ch) return false;

    StatGroup *Group;
    if( !(Group = this->FindStatGroup(ch->player.chclass, ch->player.race, ch->player.sex)) )
    {
        MudLog(BRF, MAX(GET_INVIS_LEV(ch), LVL_APPR), TRUE,
        "StatManager::RollStats() Unable to find relating stat table for player %s",
        ch->player.name.c_str());
        return false;
    }
    else
    {
        for(std::map<int, std::vector<byte> >::iterator Batch = Group->StatRolls.begin();Batch != Group->StatRolls.end();++Batch)
        {
            int nr, roll;

            for(nr = MiscUtil::random(1, 100), roll = 0;roll < (int)( (*Batch).second ).size();++roll)
            {
                if( (nr -= ( (*Batch).second )[roll]) <= 0)
                {
                    break;
                }
            }
            //Invalid table setup. Bad!
            if(roll == (int)( (*Batch).second ).size())
            {
                Stat *stat = StatManager::GetManager().StatByID( (*Batch).first);
                MudLog(BRF, MAX(GET_INVIS_LEV(ch), LVL_APPR), TRUE,
                "Invalid stat table setup when player %s rolled stat %s.", GET_NAME(ch),
                stat->getName().c_str());
                return false;
            }

            byte* statToAlter;
            switch( (*Batch).first )
            {
            case STAT_STR:
                statToAlter = &ch->aff_abils.str;
                break;
            case STAT_INT:
                statToAlter = &ch->aff_abils.intel;
                break;
            case STAT_WIS:
                statToAlter = &ch->aff_abils.wis;
                break;
            case STAT_DEX:
                statToAlter = &ch->aff_abils.dex;
                break;
            case STAT_CON:
                statToAlter = &ch->aff_abils.con;
                break;
            default:
                statToAlter = 0;
                break;
            }
            if(!statToAlter)
                continue;
            *statToAlter = roll;
        }
		ch->real_abils.str		= ch->GetStr();
		ch->real_abils.intel	= ch->GetInt();
		ch->real_abils.wis		= ch->GetWis();
		ch->real_abils.dex		= ch->GetDex();
		ch->real_abils.con		= ch->GetCon();
		return true;
	}
}
//Find statgroup which corresponds with class, race, and sex.
StatGroup *StatManager::FindStatGroup(const byte chclass, const byte race, const byte sex)
{
	for(std::list<StatGroup>::iterator SG = this->StatGroups.begin();
	SG != this->StatGroups.end();++SG)
	{
		if( (*SG).chclass == chclass && (*SG).race == race && (*SG).sex == sex)
		{
			return &(*SG);
		}
	}
	return 0;
}
StatManager* StatManager::Self = 0;

//Gain access to the current manager if one does not exist, or a new one otherwise.
StatManager& StatManager::GetManager()
{
	if(!Self)
		Self = new StatManager();
	return (*Self);
}


Stat* StatManager::StatByID( const int ID )
{
    std::list<Stat>::iterator S;
    for(S = this->StatList.begin();S != this->StatList.end();++S)
    {
        if( (*S).GetID() == ID)
            return &(*S);
    }
    return 0;
}

void StatManager::save()
{
	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempStatGroup");

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempStatGroupStat");

	gameDatabase->sendRawQuery("CREATE TABLE tempStatGroup LIKE statGroup");

	gameDatabase->sendRawQuery("CREATE TABLE tempStatGroupStat LIKE statGroupStat");

	sql::BatchInsertStatement statGroupStatBatchInsertStatement(gameDatabase, "tempStatGroupStat", 100000);

	statGroupStatBatchInsertStatement.addField("stat_group_id");
	
	statGroupStatBatchInsertStatement.addField("stat_type");
	
	statGroupStatBatchInsertStatement.addField("stat_roll");
	
	statGroupStatBatchInsertStatement.addField("probability");

	statGroupStatBatchInsertStatement.start();

	for(std::list<StatGroup>::iterator iter = StatGroups.begin();iter != StatGroups.end();++iter) {

		StatGroup *statGroup = &(*iter);

		std::stringstream sql;

		sql << "INSERT INTO tempStatGroup("
			<< "  `race`,"
			<< "  `chclass`,"
			<< "  `sex`"
			<< ") VALUES ("
			<< (int)statGroup->race << ","
			<< (int)statGroup->chclass << ","
			<< (int)statGroup->sex
			<< ");";

		gameDatabase->sendRawQuery(sql.str());

		int stat_group_id = gameDatabase->lastInsertID();

		std::map< int, std::vector<byte> >::iterator statGroupStatIter = statGroup->StatRolls.begin();

		while(statGroupStatIter != statGroup->StatRolls.end()) {

			int statRollIndex = 0;

			int statType = (*statGroupStatIter).first;

			while(statRollIndex < (*statGroupStatIter).second.size()) {

				statGroupStatBatchInsertStatement.beginEntry();

				statGroupStatBatchInsertStatement.putInt(stat_group_id);

				statGroupStatBatchInsertStatement.putInt(statType);

				statGroupStatBatchInsertStatement.putInt(statRollIndex);

				statGroupStatBatchInsertStatement.putInt((*statGroupStatIter).second[statRollIndex]);

				statGroupStatBatchInsertStatement.endEntry();

				++statRollIndex;
			}

			++statGroupStatIter;
		}
	}

	statGroupStatBatchInsertStatement.finish();

	gameDatabase->sendRawQuery(
		"RENAME TABLE statGroup TO statGroupOld, tempStatGroup TO statGroup, statGroupOld TO tempStatGroup,"
		"             statGroupStat TO statGroupStatOld, tempStatGroupStat TO statGroupStat, statGroupStatOld TO tempStatGroupStat"
	);

	gameDatabase->sendRawQuery("DROP TABLE tempStatGroup");

	gameDatabase->sendRawQuery("DROP TABLE tempStatGroupStat");
}

void StatManager::Boot()
{
	sql::Query statGroupQuery = gameDatabase->sendQuery("SELECT * FROM statGroup ORDER BY id");

	sql::Query statGroupStatQuery = gameDatabase->sendQuery("SELECT * FROM statGroupStat ORDER BY stat_group_id,stat_type,stat_roll");

	while(statGroupQuery->hasNextRow())
	{
		std::list<sql::Row> statGroupStatRowList;

		sql::Row statGroupRow = statGroupQuery->getRow();

		StatGroup statGroup(statGroupRow.getInt("chclass"), statGroupRow.getInt("race"), statGroupRow.getInt("sex"));

		while(statGroupStatQuery->hasNextRow()) {

			sql::Row statGroupStatRow = statGroupStatQuery->peekRow();

			int statGroupId = statGroupRow.getInt("id");

			int statGroupStat_statGroupId = statGroupStatRow.getInt("stat_group_id");

			if(statGroupId == statGroupStat_statGroupId) {

				statGroupStatQuery->skipRow();
				statGroup.StatRolls[statGroupStatRow.getInt("stat_type")].push_back(statGroupStatRow.getInt("probability"));
			}
			else if(statGroupStat_statGroupId < statGroupId) {

				statGroupStatQuery->skipRow();
				continue;
			}
			else {

				break;
			}
		}

		StatGroups.push_back(statGroup);
	}

    Log("Booted %d Stat Groups.", (int)this->StatGroups.size());
}
