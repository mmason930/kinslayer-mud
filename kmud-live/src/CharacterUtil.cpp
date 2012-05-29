#include <string>
#include <vector>
#include <list>

#include "CharacterUtil.h"
#include "StringUtil.h"
#include "MiscUtil.h"
#include "structs.h"
#include "db.h"
#include "utils.h"

extern Character *character_list;

Character *CharacterUtil::loadCharacter(const int userId)
{
	Character *character = NULL;

	std::stringstream queryBuffer;
	sql::Query query;
	sql::Row row;
	char f1[128], f2[128], f3[128], f4[128], f5[128];

	queryBuffer << "SELECT"
				   " user_password,"
				   " poofin,"
				   " poofout,"
				   " level,"
				   " maxhit,"
				   " hit,"
				   " maxmove,"
				   " move,"
				   " maxmana,"
				   " mana,"
				   " shadow,"
				   " maxshadow,"
				   " race,"
				   " chclass,"
				   " birth,"
				   " weight,"
				   " height,"
				   " sex,"
				   " invis,"
				   " weave,"
				   " taint,"
				   " death_wait,"
				   " warnings,"
				   " master_weapon,"
				   " strain,"
				   " slew,"
				   " whois_extra,"
				   " last_logon,"
				   " warrants,"
				   " gold,"
				   " bank_gold,"
				   " str,"
				   " dex,"
				   " intel,"
				   " wis,"
				   " con,"
				   " luck,"
				   " exp,"
				   " played,"
				   " restat,"
				   " pk_deaths,"
				   " mob_deaths,"
				   " mount,"
				   " loadroom,"
				   " pracs,"
				   " spell_pracs,"
				   " freeze,"
				   " bad_passwords,"
				   " hunger,"
				   " thirst,"
				   " drunk,"
				   " plr,"
				   " prf,"
				   " newb,"
				   " reset,"
				   " username,"
				   " user_id,"
				   " title,"
				   " description,"
				   " pks,"
				   " pw_updated,"
				   " trade_pracs,"
				   " last_logout,"
				   " account_id "
				   "FROM users "
				   "WHERE user_id = " << userId;

	try {query = gameDatabase->sendQuery(queryBuffer.str());}
	catch( sql::QueryException e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "CharacterUtil::loadCharacter : %s", e.getMessage().c_str());
		return NULL;
	}
	if( query->numRows() <= 0 )
	{
		return NULL;
	}
	
	character = new Character(CharPlayer);

	row = query->getRow();
	character->player.passwd = row["user_password"];
	character->points.poofin = row["poofin"];
	character->points.poofout = row["poofout"];
	character->player.level = atoi(row["level"].c_str());
	character->points.max_hit = atoi(row["maxhit"].c_str());
	character->points.hit = atoi(row["hit"].c_str());
	character->points.max_move = atoi(row["maxmove"].c_str());
	character->points.move = atoi(row["move"].c_str());
	character->points.max_mana = atoi(row["maxmana"].c_str());
	character->points.mana = atoi(row["mana"].c_str());
	character->points.shadow_points = atoi(row["shadow"].c_str());
	character->points.max_shadow_points = atoi(row["maxshadow"].c_str());
	character->player.race = atoi(row["race"].c_str());
	character->player.chclass = atoi(row["chclass"].c_str());
	character->player.time.birth.setTime(atol(row["birth"].c_str()));
	character->player.weight = atoi(row["weight"].c_str());
	character->player.height = atoi(row["height"].c_str());
	character->player.sex = atoi(row["sex"].c_str());
	character->points.invis = atoi(row["invis"].c_str());
	character->points.weave = atoi(row["weave"].c_str());
	character->points.taint = atoi(row["taint"].c_str());
	character->points.death_wait = atoi(row["death_wait"].c_str());
	character->points.warning = atoi(row["warnings"].c_str());
	character->points.master_weapon = atoi(row["master_weapon"].c_str());
	character->points.strain = atoi(row["strain"].c_str());
	character->points.slew = row["slew"];
	character->points.whois_extra = row["whois_extra"];
	character->points.last_logon = row.getTimestamp("last_logon");
	character->points.last_logout = row.getTimestamp("last_logout");

	if(sscanf(row["warrants"].c_str(), "%s %s %s %s", f1, f2, f3, f4) == 4)
	{
		character->points.warrants[0] = asciiflag_conv(f1);
		character->points.warrants[1] = asciiflag_conv(f2);
		character->points.warrants[2] = asciiflag_conv(f3);
		character->points.warrants[3] = asciiflag_conv(f4);
	}

	character->points.gold = atoi(row["gold"].c_str());
	character->points.bank_gold = atoi(row["bank_gold"].c_str());

	character->real_abils.str = atoi(row["str"].c_str());
	character->SetStr(character->real_abils.str);
	character->real_abils.dex = atoi(row["dex"].c_str());
	character->SetDex(character->real_abils.dex);
	character->real_abils.intel = atoi(row["intel"].c_str());
	character->SetInt(character->real_abils.intel);
	character->real_abils.wis = atoi(row["wis"].c_str());
	character->SetWis(character->real_abils.wis);
	character->real_abils.con = atoi(row["con"].c_str());
	character->SetCon(character->real_abils.con);
	character->real_abils.luck = atoi(row["luck"].c_str());
	character->SetLuck(character->real_abils.luck);

	character->points.exp = atoi(row["exp"].c_str());
	character->player.time.played = atol(row["played"].c_str());
	character->restat_time.setTime(atol(row["restat"].c_str()));
	character->pk_deaths = atoi(row["pk_deaths"].c_str());
	character->mob_deaths = atoi(row["mob_deaths"].c_str());
	character->PlayerData->mount_save = atoi(row["mount"].c_str());
	character->PlayerData->load_room = atoi(row["loadroom"].c_str());
	character->PlayerData->skillpracs = atoi(row["pracs"].c_str());
	character->PlayerData->spellpracs = atoi(row["spell_pracs"].c_str());
	character->PlayerData->freeze_level = atoi(row["freeze_level"].c_str());
	character->PlayerData->bad_pws = atoi(row["bad_passwords"].c_str());
	character->PlayerData->conditions[ FULL ] = atoi(row["hunger"].c_str());
	character->PlayerData->conditions[ THIRST ] = atoi(row["thirst"].c_str());
	character->PlayerData->conditions[ DRUNK ] = atoi(row["drunk"].c_str());
	character->player.act = atoi(row["plr"].c_str());

	if(sscanf(row["prf"].c_str(), "%s %s %s %s %s", f1, f2, f3, f4, f5) == 5)
	{
		PRF_FLAGS(character)[0] = asciiflag_conv(f1);
		PRF_FLAGS(character)[1] = asciiflag_conv(f2);
		PRF_FLAGS(character)[2] = asciiflag_conv(f3);
		PRF_FLAGS(character)[3] = asciiflag_conv(f4);
		PRF_FLAGS(character)[4] = asciiflag_conv(f5);
	}
	if(sscanf(row["newb"].c_str(), "%s %s %s %s", f1, f2, f3, f4) == 4)
	{
		NEWB_FLAGS(character)[0] = asciiflag_conv(f1);
		NEWB_FLAGS(character)[1] = asciiflag_conv(f2);
		NEWB_FLAGS(character)[2] = asciiflag_conv(f3);
		NEWB_FLAGS(character)[3] = asciiflag_conv(f4);
	}
	character->reset_time = atol(row["reset"].c_str());
	character->player.name = row["username"];
	character->player.idnum = atoi(row["user_id"].c_str());
	character->player.title = row["title"];
	character->player.description = new char[EXDSCR_LENGTH];
	strncpy(character->player.description, row["description"].c_str(), (EXDSCR_LENGTH)-1);
	character->player.description[EXDSCR_LENGTH-1] = '\0';
	character->pks = atoi(row["pks"].c_str());
	character->PasswordUpdated( (row["pw_updated"] == "1") );
	character->PlayerData->tradepracs = atoi(row["trade_pracs"].c_str());
	character->PlayerData->account_id = atoi(row["account_id"].c_str());

	character->LoadSkills();
	character->LoadIgnores();
	character->LoadAliases();
	character->LoadClans();
	character->LoadTrophies();
	character->LoadHitRolls();
	character->LoadManaRolls();
	character->LoadQuests();

	character->PlayerData->userDisabledCommands = Character::loadUserDisabledCommands(character->player.idnum);
	return character;
}

Character *CharacterUtil::loadCharacter(const std::string &username)
{
	PlayerIndex *playerIndex = CharacterUtil::getPlayerIndexByUserName(username);
	if(playerIndex != NULL)
	{
		return loadCharacter(playerIndex->id);
	}
	return NULL;
}

void CharacterUtil::changeUserPassword(sql::Connection connection, const int userId, const std::string &userPassword)
{
	std::stringstream sql;

	sql << " UPDATE users"
		<< " SET user_password = " << sql::escapeQuoteString(userPassword)
		<< " WHERE user_id = " << userId;

	connection->sendRawQuery(sql.str());
}

Character *CharacterUtil::getOnlineCharacterById(const int userId)
{
	Character *character = NULL;

	for(character = character_list;character;character = character->next)
	{
		if(!character->IsPurged() && character->player.idnum == userId)
			return character;
	}

	return NULL;
}


PlayerIndex *CharacterUtil::getPlayerIndexByUserName(const std::string &userName)
{
	for(auto iter = PlayerTable.begin();iter != PlayerTable.end();++iter)
	{
		if(!str_cmp((*iter)->name, userName))
			return (*iter);
	}
	return NULL;
}

PlayerIndex *CharacterUtil::getPlayerIndexByUserId(const int userId)
{
	for(auto iter = PlayerTable.begin();iter != PlayerTable.end();++iter)
	{
		if((*iter)->id == userId)
			return (*iter);
	}
	return NULL;
}
