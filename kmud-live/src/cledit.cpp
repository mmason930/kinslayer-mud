/*****************************************************
 *		Kinslayer's Clan Edit System.				 *
 *		   By Galnor on 4-16-2005					 *
 *****************************************************/
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "utils.h"
#include "db.h"
#include "constants.h"
#include "olc.h"
#include "shop.h"
#include "clans.h"
#include "weaves.h"
#include "Descriptor.h"

extern Descriptor *descriptor_list;
extern Clan *ClanList, *ClanEnd;

ACMD(do_cledit)
{
	Descriptor *d;
	char buf1[MAX_STRING_LENGTH];

	OneArgument(argument, buf1);

	for(d = descriptor_list;d;d = d->next)
	{
		if(STATE(d) == CON_CLEDIT && d->olc && d->olc->clan && d->olc->clan->vnum == atoi(buf1))
		{
			ch->Send("%s is currently editing data for %s.\r\n", d->character ? GET_NAME(d->character) : "Someone",
			         d->olc->clan->Name.c_str());
			return;
		}
	}

	if (GET_LEVEL(ch) < LVL_GRGOD && !(IS_AWESOME(ch) || IS_QUEEN(ch) || IS_GAY(ch)))
	{
		ch->Send("You can't edit the clans.\r\n");
		return;
	}

	d = ch->desc;

	if(isdigit(*buf1))
	{
		d->olc = new OLC();
		OLC_ZONE(d) = 0;

		if(!GetRealClan(atoi(buf1)))
			CleditSetupNew(d, atoi(buf1));
		else
			CleditSetupExisting(d, atoi(buf1));

		STATE(d) = CON_CLEDIT;
		Act("$n begins using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
		SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_WRITING));

		MudLog(BRF, LVL_IMMORT, TRUE,  "OLC: %s starts editing the clans.", GET_NAME(ch));
	}
	else
	{
		d->Send("Which clan is it you wish to edit?\r\n");
	}
}

/*-------------------------------------------------------------------*/

void Clan::Copy(Clan *source)
{
	*this = *source;
}

void CleditSetupExisting(Descriptor *d, int vnum)
{
	Clan *c;

	if(!(c = GetRealClan(vnum)))
	{
		MudLog(BRF, LVL_APPR, TRUE,
		       "SYSERR: CleditSetupExisting() : Clan with virtual number of %d does not exist.", vnum);
		cleanup_olc(d, CLEANUP_ALL);
		return;
	}

	OLC_CLAN(d) = new Clan();
	OLC_CLAN(d)->Copy(c);
	CleditDispMenu(d);
}

void CleditSetupNew(Descriptor *d, int vnum)
{
	OLC_CLAN(d) = new Clan();
	OLC_CLAN(d)->Name = "New Clan";
	OLC_CLAN(d)->vnum = vnum;
	CleditDispMenu(d);
}

/******************************************************************************/

void CleditSaveInternally(Descriptor *d)
{
	Clan *c, *tmpNext, *tmpPrev;

	if(!(c = GetRealClan(OLC_CLAN(d)->vnum)))
	{
		c = new Clan(true);
	}

	tmpNext = c->Next;
	tmpPrev = c->Prev;
	c->Copy(OLC_CLAN(d));
	c->Next = tmpNext;
	c->Prev = tmpPrev;
}

void CleditSaveToDisk(Descriptor *d)
{
	
	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempClan");
	gameDatabase->sendRawQuery("CREATE TABLE tempClan LIKE clan");

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempClanRank");
	gameDatabase->sendRawQuery("CREATE TABLE tempClanRank LIKE clanRank");

	sql::BatchInsertStatement clanBatchInsertStatement(gameDatabase, "tempClan", 10000);
	sql::BatchInsertStatement clanRankBatchInsertStatement(gameDatabase, "tempClanRank", 10000);

	clanBatchInsertStatement.addField("id");
	clanBatchInsertStatement.addField("name");
	clanBatchInsertStatement.addField("is_secret");
	clanBatchInsertStatement.addField("warrant_id");
	clanBatchInsertStatement.addField("hidden_level");
	clanBatchInsertStatement.addField("bonus_skill_id");
	clanBatchInsertStatement.addField("bonus_skill_modifier");
	clanBatchInsertStatement.addField("bonus_skill_rank");

	clanRankBatchInsertStatement.addField("clan_id");
	clanRankBatchInsertStatement.addField("rank_number");
	clanRankBatchInsertStatement.addField("name");

	clanBatchInsertStatement.start();

	clanRankBatchInsertStatement.start();

	for(Clan *clan = ClanList;clan;clan = clan->Next) {

		clanBatchInsertStatement.beginEntry();

		clanBatchInsertStatement.putInt(clan->vnum);
		clanBatchInsertStatement.putString(clan->Name);
		clanBatchInsertStatement.putInt(clan->secret);
		clanBatchInsertStatement.putInt(clan->warrant);
		clanBatchInsertStatement.putInt(clan->hidden_level);
		clanBatchInsertStatement.putInt(clan->bonus_skill);
		clanBatchInsertStatement.putInt(clan->bonus_skill_val);
		clanBatchInsertStatement.putInt(clan->bonus_skill_rank);

		for(int rankIndex = 0;rankIndex < clan->RankNames.size();++rankIndex) {
			
			clanRankBatchInsertStatement.beginEntry();

			clanRankBatchInsertStatement.putInt(clan->vnum);
			clanRankBatchInsertStatement.putInt(rankIndex+1);
			clanRankBatchInsertStatement.putString(clan->RankNames[rankIndex]);

			clanRankBatchInsertStatement.endEntry();
		}

		clanBatchInsertStatement.endEntry();
	}

	clanBatchInsertStatement.finish();

	clanRankBatchInsertStatement.finish();

	gameDatabase->sendRawQuery("RENAME TABLE tempClan TO tempClanNew, clan TO tempClan, tempClanNew TO clan");
	gameDatabase->sendRawQuery("RENAME TABLE tempClanRank TO tempClanRankNew, clanRank TO tempClanRank, tempClanRankNew TO clanRank");

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempClan");
	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempClanRank");

	d->Send("Clan data saved successfully.\r\n");
}

void CleditDispMenu(Descriptor *d)
{
	get_char_cols(d->character);
	d->Send("%sClan #%s%d\r\n"
	        "%s1%s)  Name:          %s%s\r\n"
	        "%s2%s)  Secret:        %s%s\r\n"
	        "%s3%s)  Warrant:       %s%s\r\n"
	        "%s4%s)  Hidden Level:  %s%d\r\n"
			"%s5%s)  Bonus Skill:   %s%s  %d  %d\r\n"
	        "%s6%s)  Ranks\r\n"
	        "%s7%s)  Delete\r\n"
	        "%sQ%s)  Exit\r\n",
	        nrm, cyn, OLC_CLAN(d)->vnum,
	        grn, nrm, cyn, OLC_CLAN(d)->Name.c_str(),
	        grn, nrm, cyn, CHECK_VAR(OLC_CLAN(d)->secret),
	        grn, nrm, cyn, d->olc->clan->GetWarrant() ? d->olc->clan->GetWarrant()->Name.c_str() : "None",
	        grn, nrm, cyn, d->olc->clan->hidden_level,
			grn, nrm, cyn, WeaveManager::GetManager().GetWeaveName(OLC_CLAN(d)->bonus_skill).c_str(), d->olc->clan->bonus_skill_val, d->olc->clan->bonus_skill_rank,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm);
	OLC_MODE(d) = CLEDIT_MENU;
}

void CleditRankMenu(Descriptor *d)
{
	int i = 0;
	get_char_cols(d->character);

	d->Send("\r\nRanks for %s\r\n", OLC_CLAN(d)->Name.c_str());

	for(i = 0;i < (int) OLC_CLAN(d)->RankNames.size();++i)
	{
		d->Send("    %s%d%s. %s%s\r\n", grn, i + 1, nrm, yel, OLC_CLAN(d)->RankNames[i].c_str());
	}
	d->Send("\r\n%sI%s) %sInsert Rank Name\r\n"
	        "%sD%s) %sDelete Rank Name\r\n"
	        "%sE%s) %sEdit Rank Name\r\n"
	        "%sQ%s) %sExit Rank Menu\r\n"
	        "%sChoose an option : ",
	        grn, nrm, cyn,
	        grn, nrm, cyn,
	        grn, nrm, cyn,
	        grn, nrm, cyn,
	        nrm);

	OLC_MODE(d) = CLEDIT_RANK_MENU;
}

void CleditDisplayWarrants(Descriptor *d)
{
	std::list<Warrant *>::iterator iter;

	get_char_cols(d->character);
	d->Send("Warrants:\r\n");

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		d->Send("%d. %s\r\n", (*iter)->vnum, (*iter)->Name.c_str());
	}
}

void CleditDispBonusSkills(Descriptor *d)
{
	get_char_cols(d->character);
	d->Send("Choose from the following skills, or 0 for none:\r\n\r\n");
	d->Send( WeaveManager::GetManager().ListWeaves( 0, d->character ).c_str() );
}

void CleditParse(Descriptor *d, const std::string &arg)
{
	Clan *c;
	int num = 0;
	std::vector<std::string>::iterator iter;

	switch(OLC_MODE(d))
	{
			/*********************************/
		case CLEDIT_MENU:
			switch(atoi(arg.c_str()))
			{
				case 1:
					OLC_MODE(d) = CLEDIT_NAME;
					d->Send("Enter the new name for this clan: ");
					return;
				case 2:
					TOGGLE_VAR(OLC_CLAN(d)->secret);
					break;
				case 3:
					CleditDisplayWarrants(d);
					OLC_MODE(d) = CLEDIT_WARRANT;
					d->Send("Enter the warrant for this clan(-1 for none) : ");
					return;
				case 4:
					d->Send("Enter the new minimum level required to view this clan: ");
					OLC_MODE(d) = CLEDIT_HIDDEN_LEVEL;
					return;
				case 5:
					CleditDispBonusSkills(d);
					d->Send("Enter the new bonus skill:\r\n");
					OLC_MODE(d) = CLEDIT_BONUS_SKILL;
					return;
				case 6:
					CleditRankMenu(d);
					OLC_MODE(d) = CLEDIT_RANK_MENU;
					return;
				case 7:
					d->Send("Are you SURE that you want to delete this clan? There is no going back.\r\n");
					OLC_MODE(d) = CLEDIT_CONFIRM_DELETE;
					return;
				default:
					if(toupper(arg[0]) == 'Q')
					{
						OLC_MODE(d) = CLEDIT_CONFIRM_SAVE;
						d->Send("Would you like to save your changes to the clans? 'Y' or 'N' : ");
						return;
					}
					d->Send("Invalid option. Try again: ");
					return;
			}
			break;
			/*********************************/
		case CLEDIT_CONFIRM_SAVE:
			switch(toupper(arg[0]))
			{
				case 'Y':
					CleditSaveInternally(d);
					CleditSaveToDisk(d);
					break;
				case 'N':
					d->Send("Exited without saving.\r\n");
					break;
				default:
					d->Send("Invalid option. Enter 'Y' or 'N' : ");
					return;
			}
			cleanup_olc(d, CLEANUP_ALL);
			return;
			/*********************************/
		case CLEDIT_NAME:
			OLC_CLAN(d)->Name = arg;
			break;
		case CLEDIT_WARRANT:
			num = atoi(arg.c_str());
			if(num == -1)
			{
				d->Send("Warrant set to none.\r\n");
				break;
			}
			else if(!(WarrantByVnum(num)))
			{
				d->Send("Invalid warrant. Try again : ");
				return;
			}
			d->olc->clan->warrant = num;
			d->Send("Warrant set to %s.", WarrantByVnum(num)->Name.c_str());
			break;
		case CLEDIT_HIDDEN_LEVEL:
			if(atoi(arg.c_str()) < 0 || atoi(arg.c_str()) > d->character->player.level)
			{
				d->Send("\r\nLevel range must be between your level and 0. Try again: ");
				return;
			}
			OLC_CLAN(d)->hidden_level = atoi(arg.c_str());
			break;
		case CLEDIT_CONFIRM_DELETE:
			switch(toupper(arg[0]))
			{
				case 'Y':
					if((c = GetRealClan(OLC_CLAN(d)->vnum)))
					{
						REMOVE_NODE(ClanList, ClanEnd, c, Prev, Next);
						delete c;
					}
					cleanup_olc(d, CLEANUP_ALL);
					d->Send("This clan has been deleted.\r\n");
					CleditSaveToDisk(d);
					return;
				case 'N':
					d->Send("Clan not deleted.\r\n");
					break;
				default:
					d->Send("You must type 'Y' or 'N'.\r\n");
					return;
			}
			break;
		case CLEDIT_RANK_MENU:
			switch(toupper(arg[0]))
			{
				case 'I':
					d->Send("Where would you like to insert this new rank? : ");
					OLC_MODE(d) = CLEDIT_RANK_INSERT_NUMBER;
					return;
				case 'D':
					d->Send("Which rank would you like to remove? : ");
					OLC_MODE(d) = CLEDIT_RANK_DELETE;
					return;
				case 'E':
					if(!(d->olc->clan->RankNames.size()))
					{
						d->Send("There are no existing ranks. Use the insert option.\r\n");
						return;
					}
					d->Send("Which rank would you like to edit? : ");
					OLC_MODE(d) = CLEDIT_RANK_EDIT_NUMBER;
					return;
				case 'Q':
					break;
				default:
					d->Send("Invalid option.\r\n");
					CleditRankMenu(d);
					return;
			}
			break;
		case CLEDIT_RANK_EDIT_NUMBER:
			num = atoi(arg.c_str());
			if(num <= 0 || num > (int) OLC_CLAN(d)->RankNames.size())
			{
				d->Send("That rank does not exist. Try again : ");
				return;
			}
			//Remove the rank
			d->olc->clan->RemoveRank(num);
			OLC_VAL(d) = num;

			d->Send("Insert the new name for this rank : ");

			OLC_MODE(d) = CLEDIT_RANK_INSERT_NAME;
			return;
		case CLEDIT_RANK_INSERT_NUMBER:
			num = atoi(arg.c_str());
			if(num <= 0 || num > (int) OLC_CLAN(d)->RankNames.size() + 1)
			{
				d->Send("To insert rank before givin number, range must be between 1 and %d. Try again : ",
				        OLC_CLAN(d)->RankNames.size() + 1);
				return;
			}
			OLC_VAL(d) = num;
			d->Send("Insert the name for this rank : ");
			OLC_MODE(d) = CLEDIT_RANK_INSERT_NAME;
			return;
		case CLEDIT_RANK_INSERT_NAME:
			d->olc->clan->FindRankIterator(OLC_VAL(d), iter);
			d->olc->clan->RankNames.insert(iter, arg);
			CleditRankMenu(d);
		case CLEDIT_RANK_DELETE:
			num = atoi(arg.c_str());
			if(num <= 0 || num > (int) OLC_CLAN(d)->RankNames.size())
			{
				d->Send("That rank does not exist!\r\n");
				return;
			}

			d->olc->clan->FindRankIterator(num, iter);

			d->Send("Rank %d has been removed.\r\n", num);
			OLC_CLAN(d)->RankNames.erase(iter);
			CleditRankMenu(d);
			return;
		case CLEDIT_BONUS_SKILL:
			num = atoi(arg.c_str());
			if (num == 0)
			{
				d->Send("Bonus skill set to none.\r\n");
				OLC_CLAN(d)->bonus_skill = 0;
				break;
			}
			if (!WeaveManager::GetManager().GetWeave(num))
			{
				d->Send("Invalid skill, try again : ");
				return;
			}
			d->Send("Bonus skill set to %s.\r\n", WeaveManager::GetManager().GetWeaveName(num).c_str());
			OLC_CLAN(d)->bonus_skill = num;
			OLC_MODE(d) = CLEDIT_BONUS_SKILL_VAL;
			d->Send("Enter the desired practice bonus for the skill: ");
			return;
		case CLEDIT_BONUS_SKILL_VAL:
			num = atoi(arg.c_str());
			if ( num < 0 || num > 99)
			{
				d->Send("The bonus must be between 0 and 99, try again : ");
				return;
			}
			d->Send("Practice bonus set to %d.\r\n", num);
			OLC_CLAN(d)->bonus_skill_val = num;
			OLC_MODE(d) = CLEDIT_BONUS_SKILL_RANK;
			d->Send("Enter the rank at which this bonus will become active: ");
			return;
		case CLEDIT_BONUS_SKILL_RANK:
			num = atoi(arg.c_str());
			if ( num < 0 || num > 10 )
			{
				d->Send("The rank must be between 0 and 10, try again : ");
				return;
			}
			d->Send("Rank set to %d.\r\n", num);
			OLC_CLAN(d)->bonus_skill_rank = num;
			break;
		default:
			d->Send("Invalid Cledit mode of %d. Tell a coder about this!\r\n");
			MudLog(BRF, MAX(GET_INVIS_LEV(d->character), LVL_APPR), TRUE,
			       "Argh! %s entered an invalid Cledit state of %d!", GET_NAME(d->character), OLC_MODE(d));
			cleanup_olc(d, CLEANUP_ALL);
			return;
	}
	CleditDispMenu(d);
}
