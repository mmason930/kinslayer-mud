/* ************************************************************************
*   File: spec_procs.c                                  Part of CircleMUD *
*  Usage: implementation of special procedures for mobiles/objects/rooms  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "poker.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "dg_event.h"
#include "screen.h"
#include "clans.h"
#include "weaves.h"

#include "MiscUtil.h"
#include "StringUtil.h"
#include "ClanUtil.h"
#include "rooms/Room.h"

#include "commands/infrastructure/CommandUtil.h"
#include "commands/infrastructure/CommandInfo.h"

/*   external vars  */

extern Character *character_list;
extern Descriptor *descriptor_list;
extern struct Index *obj_index;
extern struct GameTime time_info;
extern const struct int_app_type int_app[];
extern char *class_types[];

/* Spell voids */

void fireball(Character * ch);
void cast_fireball(Character * ch);
void add_follower(Character * ch, Character * leader);

int cmd_yell;
/* local functions */
const char *how_good(int percent);
void list_skills(Character * ch);
void npc_steal(Character * ch, Character * victim);
SPECIAL(guild);
SPECIAL(mayor);
SPECIAL(snake);
SPECIAL(thief);
SPECIAL(magic_user);
SPECIAL(guild_guard);
SPECIAL(puff);
SPECIAL(fido);
SPECIAL(janitor);
SPECIAL(cityguard);
SPECIAL(pet_shops);
SPECIAL(bank);
SPECIAL(taim);
SPECIAL(angry);
SPECIAL(call_gate);
SPECIAL(yurian);
SPECIAL(PokerDealer);

#define R_ROOM(zone, num) (real_room(((zone)*100)+(num)))
/* ********************************************************************
*  Special procedures for mobiles                                     *
******************************************************************** */

Character *find_npc_by_name(Character * chAtChar, const char *pszName, int iLen)
{
	Character *ch;

	for (ch = chAtChar->in_room->people; ch; ch = ch->next_in_room)
		if (IS_NPC(ch))
			if (!strncmp(pszName, ch->player.short_descr, iLen))
				return (ch);

	return NULL;
}

// const int CLASS_WARRIOR = 0;
// const int CLASS_THIEF = 1;
// const int CLASS_RANGER = 2;
// const int CLASS_CHANNELER = 3;
// const int CLASS_FADE = 4;
// const int CLASS_DREADLORD = 5;
// const int CLASS_BLADEMASTER = 6;
// const int CLASS_GREYMAN = 7;
// const int CLASS_DRAGHKAR = 8;
// const int CLASS_DREADGUARD = 9;
// const int CLASS_OGIER = 10;

//Use like this: skill_cost[MY_CLASS][SKILL_CLASS]  -- Galnor/3/11/2004
int skill_cost[NUM_CLASSES][NUM_CLASSES] =
    {
        //W   T   R   C   F   DL   B   G   DK  DG  O
        { 1,  3,  2,  0,  0,  0,   0,  0,  0,  0,  0}, //Warriors
        { 3,  1,  2,  0,  0,  0,   0,  0,  0,  0,  0}, //Thieves
        { 2,  2,  1,  0,  0,  0,   0,  0,  0,  0,  0}, //Rangers
        { 3,  2,  2,  1,  0,  0,   0,  0,  0,  1,  0}, //Channelers
        { 1,  2,  1,  0,  1,  0,   0,  0,  0,  0,  0}, //Fades
        { 2,  2,  2,  1,  0,  1,   0,  0,  0,  0,  0}, //Dreadlords
        { 1,  2,  1,  0,  0,  0,   1,  0,  0,  0,  0}, //Blademasters
        { 2,  1,  2,  0,  0,  0,   0,  1,  0,  0,  0}, //Greymen
        { 0,  0,  0,  0,  0,  0,   0,  0,  1,  0,  0}, //Draghkar
        { 3,  2,  2,  1,  0,  0,   0,  0,  0,  1,  0}, //Dreadguards
		{ 1,  2,  1,  0,  0,  0,   0,  0,  0,  0,  1}  //Ogier

    };

//Function put to use by Galnor on 3-11-2004
int calc_price(Character *ch, int number)
{
	Weave* weave = WeaveManager::GetManager().GetWeave(number);

	//Galnor 07/23/2009 - Cost for trade skills. 1 for remorts, 2 otherwise.
	if( weave->getAttribute("Class") == "Trade" )
//		return (IS_BLADEMASTER(ch) || IS_DREADLORD(ch) || IS_FADE(ch) || IS_GREYMAN(ch)) ? (1) : (2);
		return 1;

	int iSkillClass = weave->getPrimaryClass();

	if( iSkillClass < 0 || iSkillClass > NUM_CLASSES ) return 0;

	if( weave->isElement() )
	{
		int curr_level = ch->GetSkillLevel(weave->getVnum());
		switch( (curr_level) + 1 ) //Level we wish to rise to
		{
		case 1: return 1;
		case 2: return 2;
		case 3: return 2;
		case 4: return 3;
		case 5: return 3;
		case 6: return 3;
		case 7: return 4;
		case 8: return 4;
		case 9: return 4;
		case 10: return 4;
		default:return 1000;//Should never get here.
		}
	}

	return (skill_cost[(int) GET_CLASS(ch)][iSkillClass]);


}

//NPC damage. Used mainly for special MOBs who we don't want to run through the main damage function//
void dmg(Character *ch, Character *vict, int low, int high, int same_room)
{
	int dam;
	dam = MiscUtil::random(low, high);

	if(ch->in_room != vict->in_room && same_room)
		return;

	GET_HIT(vict) -= dam;

	if(GET_HIT(vict) <= 0)
		vict->Die(ch);

}


int spell_sort_info[MAX_SKILLS+1];

const char *how_good(int percent)
{
	if (percent < 0)
		return " (error)";
	if (percent == 0)
		return " (not learned)";
	if (percent <= 10)
		return " (awful)";
	if (percent <= 20)
		return " (bad)";
	if (percent <= 40)
		return " (poor)";
	if (percent <= 55)
		return " (average)";
	if (percent <= 70)
		return " (fair)";
	if (percent <= 80)
		return " (good)";
	if (percent <= 85)
		return " (very good)";
	return " (superb)";
}

#define LEARNED(ch) 99

void list_skills(Character * ch, Character *teacher)
{
	int i, sortpos, has_pracs;
	std::string HowGood, PracType;
	Weave* weave;

	*buf = 0;
	if(!IS_NPC(ch))
	{
		has_pracs = MAX(0, ch->PlayerData->skillpracs) + MAX(0, ch->PlayerData->tradepracs);

		if(IS_CHANNELER(ch) || IS_DREADLORD(ch) || IS_DREADGUARD(ch))
			has_pracs += MAX(0, ch->PlayerData->spellpracs);

		if(!has_pracs)
			strcpy(buf, "You have no practice sessions remaining.\r\n");
		else
		{
			sprintf(buf, "You have %d practice session%s remaining, ",
				ch->PlayerData->skillpracs, (ch->PlayerData->skillpracs == 1 ? "" : "s"));

			if(IS_CHANNELER(ch) || IS_DREADLORD(ch) || IS_DREADGUARD(ch))
			{
				sprintf(buf + strlen(buf), "%d spell practice%s remaining, ",
					ch->PlayerData->spellpracs, (ch->PlayerData->spellpracs == 1 ? "" : "s"));
			}
			sprintf(buf + strlen(buf), "and %d trade practice%s remaining.\r\n",
				ch->PlayerData->tradepracs, (ch->PlayerData->tradepracs == 1 ? "" : "s"));
		}
	}

	sprintf(buf + strlen(buf), "You are skilled in the following:\r\n");
	strcpy(buf2, buf);

	std::list<int> WeaveVnums = WeaveManager::GetManager().GetListOfWeaveVnums();
	for ( std::list<int>::iterator vI = WeaveVnums.begin();vI != WeaveVnums.end();++vI)
	{
		sortpos = i = (*vI);
		Weave* weave = WeaveManager::GetManager().GetWeave(i);

		if( weave == NULL ) continue;

		if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
		{
			strcat(buf2, "**OVERFLOW**\r\n");
			break;
		}

		if( (GET_SKILL(ch, i) <= 0 && !teacher) || !ch->CanPractice(weave))
			continue;

		if( teacher && teacher->CanTeach( weave ) == false )
			continue;

		if(weave->levels())
			sprintf(HowGood, "(Level %d)", ch->GetSkillLevel(i));
		else
			HowGood = how_good(GET_SKILL(ch, i));

		char costStr[256];
		sprintf(costStr, "(Cost: %d)", calc_price(ch, i));

		if( weave->isElement() )
			sprintf(buf2 + strlen(buf2), "%-17s %10s, (Level %d)\r\n",
				weave->getName().c_str(), costStr, ch->GetSkillLevel(i));
		else
			sprintf(buf2 + strlen(buf2), "%-17s %10s, (%d%%), %s\r\n",
				weave->getName().c_str(), costStr, GET_SKILL(ch, i), HowGood.c_str());
	}

	page_string(ch->desc, buf2, 1);
}

SPECIAL(angry)
{
	Character *cook = (Character *) me;

	if(cmd)
		return FALSE;

	if(GET_POS(cook) == POS_STANDING)
	{

		switch(MiscUtil::random(1, 10))
		{

			case 1:
				Act("$n cuts some meat, and while doing it cuts $s finger.", FALSE, cook, 0, 0, TO_ROOM);

			case 2:
			case 3:
				Act("$n screams and throws a knife at the wall.", FALSE, cook, 0, 0, TO_ROOM);
				break;

			case 7:
				Act("$n mutters about a hard days work.", FALSE, cook, 0, 0, TO_ROOM);

			case 10:
			{
				char sYell[64];
				strcpy(sYell, "yell Fresh meat! Some with blood, but I'll take care of that!");
				CommandUtil::get()->interpretCommand(cook, sYell);
				break;
			}
		}
	}

	if(time_info.hours == 1 && GET_POS(cook) != POS_SLEEPING)
	{
		Act("$n says 'Time to sleep... FINALLY!'", FALSE, cook, 0, 0, TO_ROOM);
		Act("$n unfolds a bed and instantly drops down asleep.", FALSE, cook, 0, 0, TO_ROOM);
		GET_POS(cook) = POS_SLEEPING;
		return FALSE;
	}

	else if (time_info.hours == 4 && GET_POS(cook) != POS_STANDING)
	{
		GET_POS(cook) = POS_STANDING;
		Act("$n groans as $e wakes up and gets back to work.", FALSE, ch, 0, 0, TO_ROOM);
		return FALSE;
	}

	return 0;
}

int black_tower_weave[11] =
    {
        0,
        SPELL_NIGHT_VISION,
        SPELL_SHIELD,
        SPELL_HASTE,
        SPELL_LOCATE_LIFE,
        SPELL_TORNADO,
        SPELL_RESTORE,
        SPELL_GATE,
        -1,
        -1,
        SPELL_BALEFIRE,
    };

bool Character::CanPractice( const int spell_vnum )
{
	Weave* weave = WeaveManager::GetManager().GetWeave(spell_vnum);
	return (weave && CanPractice( weave ));
}
bool Character::CanPractice( Weave* weave )
{
	if( weave->isWeave() )
	{
		std::string sAir = weave->getAttribute("Air"),
					sEarth = weave->getAttribute("Earth"),
					sFire = weave->getAttribute("Fire"),
					sSpirit = weave->getAttribute("Spirit"),
					sWater = weave->getAttribute("Water");

		//Has the channeler yet to unlock the weave with the required elements?
		if( getSkill(WeaveManager::GetManager().GetWeaveVnum("Fire")) < MiscUtil::convert<int>(sFire)    ||
			getSkill(WeaveManager::GetManager().GetWeaveVnum("Earth")) < MiscUtil::convert<int>(sEarth)  ||
			getSkill(WeaveManager::GetManager().GetWeaveVnum("Spirit")) < MiscUtil::convert<int>(sSpirit)   ||
			getSkill(WeaveManager::GetManager().GetWeaveVnum("Air")) < MiscUtil::convert<int>(sAir) ||
			getSkill(WeaveManager::GetManager().GetWeaveVnum("Water")) < MiscUtil::convert<int>(sWater)
		)
		{
			return false;
		}
	}
	//All race/class based restrictions are to be hard coded.
	if( weave->disabled() )
		return false;
	if( !this->ChannelingAbility() && (weave->isWeave() || weave->isElement()) )
		return false;
	if( !IS_FADE(this) && (weave->getName() == "Fade" || weave->getName() == "Fear" || weave->getName() == "Compel" || weave->getName() == "Shadow Rage") )
		return false;
	if( !IS_GREYMAN(this) && weave->getName() == "Effusion" )
		return false;
	if( !IS_BLADEMASTER(this) && (weave->getName() == "Hamstring" || weave->getName() == "Whirlwind"))
		return false;
	if( !IS_OGIER(this) && weave->getName() == "Pulverize" )
		return false;
	if( !CAN_RIDE(this) && weave->getName() == "Charge" )
		return false;
	if( CAN_RIDE(this) && weave->getName() == "Skewer" )
		return false;
	if( !this->AES_SEDAI() && weave->getName() == "Bond" )
		return false;
	return true;
}

SPECIAL(guild)
{
	int skill_num, percent, price;
	Character *teacher = (Character *) me;

	if (IS_NPC(ch) || !CMD_IS(ch, "practice"))
		return 0;

	if(IS_CHANNELER(ch) && IS_CHANNELER(teacher) && GET_SEX(ch) != GET_SEX(teacher))
		return 0;

	skip_spaces(&argument);

	if (!*argument)
	{
		list_skills(ch, teacher);
		return 1;
	}
	//If we get here, the player is trying to practice something.

	argument = (char*)StringUtil::cap(StringUtil::allLower(argument));//This is the skill name
	Weave* weave = WeaveManager::GetManager().GetWeave(argument);

	if(!weave || !ch->CanPractice(weave) || ((teacher) && (!teacher->CanTeach(weave))))
	{
		teacher->InterpCommand("say I do not teach that skill here.");
		return 1;
	}

	price = calc_price(ch, weave->getVnum());

	if( (ch->PlayerData->tradepracs - price < 0 && weave->classSet("Trade")) )
	{
		ch->send("You do not seem to be able to practice now.\r\n");
		return 1;
	}
	else if( weave->classSet("Trade") && GET_SKILL(ch, weave->getVnum()) > 0 )
	{
		ch->send("You need to use this skill to get better at it!\r\n");
		return 1;
	}
	else if ( (ch->PlayerData->skillpracs - price < 0 && !weave->isWeave() && !weave->isElement() && !weave->classSet("Trade")) )
	{
		ch->send("You do not seem to be able to practice now.\r\n");
		return 1;
	}

	if (GET_SKILL(ch, weave->getVnum()) >= LEARNED(ch) || weave->isElement() && ch->GetSkillLevel(weave->getVnum()) >= 10)
	{
		ch->send("You are already learned in that area.\r\n");
		return 1;
	}

	if((weave->isWeave() || weave->isElement()) && (ch->ChannelingAbility()))
	{
		if(ch->PlayerData->spellpracs >= price)
			ch->PlayerData->spellpracs -= price;
		else
		{
			ch->send("You cannot practice spells right now.\r\n");
			return 1;
		}
	}
	else if( weave->classSet("Trade") )
		ch->PlayerData->tradepracs -= price;
	else
		ch->PlayerData->skillpracs -= price;

	ch->send("You practice for a while. ");

	percent = GET_SKILL(ch, weave->getVnum());

	if( !IS_NPC(ch) && ch->userClans.empty() == false )
	{
		Clan *clan;
		for(auto userClanIter = ch->userClans.begin();userClanIter != ch->userClans.end();++userClanIter)
		{
			UserClan *userClan = (*userClanIter);
			clan = ClanUtil::getClan(userClan->getClanId());
			if(userClan->getRank() >= clan->bonus_skill_rank && weave->getVnum() == clan->bonus_skill)
				SET_SKILL(ch, weave->getVnum(), GET_SKILL(ch, weave->getVnum()) - clan->bonus_skill_val);
		}
	}
	
	percent += ch->SkillAdd(weave->getVnum());

	if( !IS_NPC(ch) && ch->userClans.empty() == false )
	{
		Clan *clan;
		for(auto userClanIter = ch->userClans.begin();userClanIter != ch->userClans.end();++userClanIter)
		{
			UserClan *userClan = (*userClanIter);
			clan = ClanUtil::getClan(userClan->getClanId());
			if(userClan->getRank() >= clan->bonus_skill_rank && weave->getVnum() == clan->bonus_skill)
				SET_SKILL(ch, weave->getVnum(), GET_SKILL(ch, weave->getVnum()) + clan->bonus_skill_val);
		}
	}

	SET_SKILL(ch, weave->getVnum(), MIN(LEARNED(ch), percent));

	if( weave->isElement() )
		ch->send("Your %s skill level is now %d.\r\n", weave->getName().c_str(), ch->GetSkillLevel(weave->getVnum()));
	else if (GET_SKILL(ch, weave->getVnum()) >= LEARNED(ch))
		ch->send("You are now learned in that area.\r\n");
	else
		ch->send("Your %s skill is now practiced to %d%%.\r\n", weave->getName().c_str(), GET_SKILL(ch, weave->getVnum()));

	return 1;
}


/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */

#define PET_PRICE(pet) (GET_LEVEL(pet) * 300)

void PokerAutoComplete( void* Data )
{
	PokerTable* Table = (PokerTable*) Data;

	Table->EmptyTurnCycle();
	Table->PrintBoard(0,0);
	Table->NextGameStep();
}

//A delayed print of the poker board.
void PokerPrint( void* Data )
{
	PokerTable* Table = (PokerTable*) Data;

	Table->PrintBoard( 0, 0 );
}
void PokerShowCards( void* Data )
{
	PokerTable* Table = (PokerTable*) Data;
	bool shown = false;

/***
	if( !Table->GetLastToShow() && Table->GetLastToCall() )
	{
		Table->SetLastToShow( Table->GetLastToCall() );//Only this person HAS to show.
		Table->GetLastToCall()->ShowHandToTable();
		shown = true;
	}
***/
	if( !shown )
	{
		std::list<PokerPlayer*>::iterator P;
		for(P = Table->GetPlayers().begin();P != Table->GetPlayers().end();++P)
		{
			if( !(*P)->Folded() )
			{
				if( !Table->GetLastToShow() )
				{
					break;
				}
				else if( (*P) == Table->GetLastToShow() )
				{//Find the next one.
					while( (++P) != Table->GetPlayers().end() && (*P)->Folded());

					if( P == Table->GetPlayers().end() )
					{
						Table->EndGame();
						return;
					}
					break;
				}
			}
		}
		if( P != Table->GetPlayers().end() )
		{
			(*P)->ShowHandToTable();
			Table->SetLastToShow( (*P) );
		}
	}

	add_event( 26, PokerShowCards, (void*)Table );
}
//A delayed dealing of players.
void PokerDeal( void* Data )
{
	PokerTable* Table = (PokerTable*) Data;

	if( Table->GetPlayers().size() <= 1 )
	{
		if( Table->GetPlayers().size() == 1 )
			Table->CloseTable();
		delete Table;
		return;
	}

	Table->SetupDeck();
	Table->ClearCards();
	Table->DealCards();

	PokerPlayer* Dealer;
	if( Table->DealerIsGone )
	{
		Dealer = Table->GetDealer();
		Table->DealerIsGone = false;
	}
	else
		Dealer = Table->SetNextDealer();
	Table->SetupTurns( Dealer, false );
	Table->PerformBlinds();

	Table->PrintBoard( 0, 0 );//No wait.

	Table->SetState( PREFLOP );
}

//A delayed message saying the cards are being shuffled.
void PokerShuffle( void* Data )
{
	PokerTable* Table = (PokerTable*) Data;

	if( Table->GetPlayers().size() <= 1 )
	{
		if( Table->GetPlayers().size() == 1 )
			Table->CloseTable();
		delete Table;
		return;
	}

	Table->SendToTable("The dealer shuffles the cards...\r\n");
	add_event(16, PokerDeal, (void*)Table);
}

//Galnor - Ugh, ugly pet shops REALLY need a make over.
SPECIAL(pet_shops)
{
	return 0;
}

/* ********************************************************************
*  Special procedures for objects                                     *
******************************************************************** */

SPECIAL(PokerDealer)
{
		Character *Dealer = (Character *) me, *Victim;
		PokerTable *Table = ch->in_room->PTable;
		char arg [MAX_INPUT_LENGTH], line[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

		strcpy(line, argument);

		TwoArguments(argument, arg, arg2);

		if(!cmd)
			return 1;

		if(!str_cmp(cmd, "setup"))
		{
			if(Table)
			{
				if( !Table->NumOfPlayers() )
					delete Table;
				else
				{
					Dealer->InterpCommand("tell %s, there is a game currently in play.", GET_NAME(ch));
					return 1;
				}
			}
			Table = new PokerTable(ch->in_room);

			Table->AddPlayer( ch );

			ch->send("You sit down at the poker table and begin setting up.\r\n");
			Act("$n sits down at a poker table and begins setting up a game.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS( ch ) = POS_SITTING;
			return 1;
		}
		if( !Table )
			return 0;
		if(!str_cmp(cmd, "join"))
		{
			if(!str_cmp(arg, "table"))
			{
				if(!Table)
					ch->send("You must setup a table first, because there are none currently in play.\r\n");
				else if(Table->IsBanned( ch ))
					ch->send("You are banned from joining this table.\r\n");
				else if(Table->HasStarted())
					ch->send("The game has already begun.\r\n");
				else if(ch->PokerData)
					ch->send("You are already at a table.\r\n");
				else
				{
					Table->AddPlayer( ch );
					ch->send("You sit down at the poker table.\r\n");
					Act("$n joins the poker table.", TRUE, ch, 0, 0, TO_ROOM);
					GET_POS( ch ) = POS_SITTING;
				}
				return 1;
			}
		}
		else if(!str_cmp(cmd, "boot"))
		{
			if( Table->TableHost() != ch->PokerData )
				ch->send("Only the host, %s, can boot players.\r\n", Table->TableHost()->PlayerName().c_str());
			else if(!*arg)
				ch->send("Boot who?\r\n");
			else if( !(Victim = get_char_room_vis(ch, arg)) )
				ch->send("That player is not in this room.\r\n");
			else if( Table->HasStarted() )
				ch->send("You cannot ban players now that the game has begun.\r\n");
			else if( !Victim->PokerData || Victim->PokerData->Table != Table )
				ch->send("That player is not at this table.\r\n");
			else
			{
				Victim->send("You have been booted from the table by %s.\r\n", GET_NAME(ch));
				Table->RemovePlayer( Victim );
				Table->SendToTable("%s has been booted from the table.\r\n", GET_NAME(Victim));
			}
			return 1;
		}
		else if(!str_cmp(cmd, "ban"))
		{
			if( Table->TableHost() != ch->PokerData )
				ch->send("Only the host, %s, can ban players.\r\n", Table->TableHost()->PlayerName().c_str());
			else if( Table->IsBanned( arg ) )
				ch->send("That player is already banned.\r\n");
			else if( Table->HasStarted() )
				ch->send("You cannot ban players now that the game has begun.\r\n");
			else if( (Victim = get_char_room_vis(ch, arg)) && Victim->PokerData
			&& Victim->PokerData->Table == Table)
			{
				Victim->send("You have been banned from the table by %s.\r\n", GET_NAME(ch));
				Table->RemovePlayer( Victim );
				Table->SendToTable("%s has been banned from the table.\r\n", GET_NAME(Victim));
				Table->AddBan( Victim );//Better name format.
			}
			else
			{
				Table->AddBan( arg );//Just ban by typed name.
				Table->SendToTable( "%s has been banned from coming to the table.\r\n", arg);
			}
			return 1;
		}
		else if(!str_cmp(cmd, "unban"))
		{
			if( Table->TableHost() != ch->PokerData )
				ch->send("Only the host, %s, can unban players.\r\n", Table->TableHost()->PlayerName().c_str());
			else if( !Table->IsBanned( arg ) )
				ch->send("That player is not currently banned.\r\n");
			else if( (Victim = get_char_room_vis(ch, arg)) && Victim->PokerData
			&& Victim->PokerData->Table == Table)
			{
				Victim->send("Your ban from the poker table has been lifted by %s.\r\n", GET_NAME(ch));
				Table->RemoveBan( Victim );
				Table->SendToTable("%s's ban has been lifted by %s.\r\n", GET_NAME( Victim ), GET_NAME( ch ));
			}
			else
			{
				Table->RemoveBan( arg );//Just ban by typed name.
				Table->SendToTable( "%s's ban has been lifted by %s.\r\n", StringUtil::cap(StringUtil::allLower(arg)), GET_NAME(ch));
			}
			return 1;
		}
		else if(!str_cmp(cmd, "watch"))
		{
			if( !Table )
			{
				ch->send("Someone first needs to setup the poker table.\r\n");
				return 1;
			}
			if( Table->IsWatching( ch ) )
			{
				ch->send("You are already watching this table.\r\n");
				return 1;
			}
			if(!ch->PokerData)
			{
				ch->send("You sit down at the table and watch the game.\r\n");
				Act("$n sits down at the table and watches the game.", TRUE, ch, 0, 0, TO_ROOM);
				Table->AddWatcher( ch );
			}
			else//This person is set as a player. Remove player data.
			{
				Table->RemovePlayer( ch );
				ch->send("You revoke your position as a player and begin watching the game.\r\n");
				Act("$n revokes $s position as a player and begins watching the game.", TRUE, ch, 0, 0, TO_ROOM);
				Table->AddWatcher( ch );
			}
			GET_POS(ch) = POS_SITTING;
			return 1;
		}
		else if(!str_cmp(cmd, "forfeit"))
		{
			if(!ch->PokerData)
			{
				ch->send("You are not even at the table.\r\n");
				return 1;
			}
			if(!Table->HasStarted())
			{
				ch->send("The game has not yet begun. Simply 'stand'\r\n");
				return 1;
			}
			if(!ch->PokerData->Folded())
			{
				ch->send("You must be folded in order to forfeit.\r\n");
				return 1;
			}
			ch->send("You forfeit your chips and leave the table.\r\n");
			Table->SendToTable("%s has forfeited, surrendering all chips.\r\n", GET_NAME(ch));
			Table->RemovePlayer( ch );
			return 1;
		}
		else if(!str_cmp(cmd, "banlist"))
		{
			Table->PrintBanList();
			return 1;
		}
		else if(!str_cmp(cmd, "list"))
		{
			Table->PrintPlayers(ch);
			return 1;
		}

		if( !ch->PokerData && !Table->IsWatching(ch) )
			return 0;

		else if(!str_cmp(cmd, "stand"))
		{
			if(GET_POS( ch ) < POS_RESTING)
				return 0;
			if(!Table || (!ch->PokerData && !Table->IsWatching(ch)) )
				return 0;
			if(Table->IsWatching(ch))
			{
				ch->send("You stop watching the poker game and stand up.\r\n");
				Table->RemoveWatcher(ch);
				Table->SendToTable("%s stop watching the game and stands up.\r\n", GET_NAME(ch));
				GET_POS(ch) = POS_STANDING;
				return 1;
			}
			if(Table->HasStarted())
			{
				ch->send("You cannot leave the game now that it's playing. You must forfeit.\r\n");
				return 1;
			}
			Table->RemovePlayer( ch );
			return 1;
		}
		else if( !str_cmp(cmd, "prompt") )
		{
			Table->PrintBoard(ch,0);
			return 1;
		}

		//At this point, we want only poker players.
		if(!ch->PokerData)
			return 0;
		if(!str_cmp(cmd, "stakes"))
		{
			if( ch->PokerData && ch->PokerData == Table->TableHost() )
			{
				if(!*arg || !MiscUtil::isNumber( arg ))
					ch->send("What, in coppers, do you want to set the stakes to?\r\n");
				else
				{
					Table->Stakes( atoi(arg) );
					Table->SendToTable( "The stakes have been changed to %d coppers.\r\n", Table->Stakes() );
				}
			}
			else
				ch->send("Only the table host can set the stakes.\r\n");
			return 1;
		}
		else if(!str_cmp(cmd, "confirm"))
		{
			if(Table->GetState() != CONFIRMING)
			{
				ch->send("Confirm what?\r\n");
				return 1;
			}

			if(!ch->PokerData->Confirmed())
				ch->PokerData->Confirm();
			else
				ch->send("You have already confirmed.\r\n");
			Table->NextGameStep();
			return 1;
		}
		else if(!str_cmp(cmd, "deny"))
		{
			ch->PokerData->Deny();
			if(Table->NumOfPlayers() > 1)
				Table->NextGameStep();
			return 1;
		}
		else if(!str_cmp(cmd, "begin"))
		{
			if( ch->PokerData && ch->PokerData == Table->TableHost() )
			{
				if( Table->NumOfPlayers() <= 1 )
					ch->send("You need at least two players to begin.\r\n");
				else if(Table->HasStarted())
					ch->send("The game has already begun.\r\n");
				else if(!Table->CollectWagers())
				{
					return true;
				}
				else
				{
					Table->NextGameStep();
				}
			}
			else
				ch->send("Only the host, %s, can begin the game.\r\n", Table->TableHost()->PlayerName().c_str());
			return 1;
		}
		else
		{
			return (Table->ProcessGameInput( ch->PokerData, std::string( cmd ), std::string( arg ) ) ? 1 : 0);
		}

	return 0;
}

SPECIAL(bank)
{
	Character *banker = (Character *)me;
	int amount;

	if(GET_RACE(banker) != GET_RACE(ch) || ch->wantedByPlayer(banker))
		return 0;

	if (CMD_IS(ch, "balance"))
	{
		if (GET_BANK_GOLD(ch) > 0)
			ch->send("Your current balance is %d coins.\r\n", GET_BANK_GOLD(ch));
		else
			ch->send("You currently have no money deposited.\r\n");

		return 1;
	}

	else if (CMD_IS(ch, "deposit"))
	{
		if ((amount = atoi(argument)) <= 0)
		{
			ch->send("How much do you want to deposit?\r\n");
			return 1;
		}
		if (ch->points.gold < amount)
		{
			ch->send("You don't have that many coins!\r\n");
			return 1;
		}
		ch->points.gold -= amount;
		GET_BANK_GOLD(ch) += amount;
		ch->send("You deposit %d coins.\r\n", amount);
		Act("$n makes a bank transaction.", TRUE, ch, 0, nullptr, TO_ROOM);
		return 1;
	}

	else if (CMD_IS(ch, "withdraw"))
	{
		if ((amount = atoi(argument)) <= 0)
		{
			ch->send("How much do you want to withdraw?\r\n");
			return 1;
		}

		if (GET_BANK_GOLD(ch) < amount)
		{
			ch->send("You don't have that many coins deposited!\r\n");
			return 1;
		}

		//ADD_GOLD(ammount);
		GET_BANK_GOLD(ch) -= amount;
		ch->points.gold += amount;
		ch->send("You withdraw %d coins.\r\n", amount);
		Act("$n makes a bank transaction.", TRUE, ch, 0, nullptr, TO_ROOM);
		return 1;
	}
	else
		return 0;
}

SPECIAL(yurian)
{
	Character *yurian = (Character *) me;
	Character *vict;

	if(cmd)
		return FALSE;

	if(FIGHTING(yurian))
	{
		vict = FIGHTING(ch);
	}
	else
	{
		return FALSE;
	}

	if (MiscUtil::random(1, 15) > 10)
	{
		hit(yurian, vict, TYPE_UNDEFINED);
	}

	else if(MiscUtil::random(1, 4) == 4)
	{
//		if(MiscUtil::random(1, 3) == 3 && FIGHTING(yurian))
//			yurian->CastSpell(vict, NULL, SPELL_FIREBALL, 0);
	}

	else if(MiscUtil::random(1, 4) == 4)
	{
		Act("Your body is BURNED by a huge blast of flames sent by $n!!!", 1, yurian, 0, vict, TO_VICT);
		Act("$n looks deeply into $N, and shoots a huge blast of flames at him!", 1, yurian, 0, vict, TO_NOTVICT);
		Act("You blast a huge sheet of flame towards $N, scorching his body!.", 1, yurian, 0, vict, TO_CHAR);
		dmg(yurian, vict, 80, 150, TRUE);
	}

	else
	{
		return FALSE;
	}

	return TRUE;
}


SPECIAL(taim)
{

	Character *taim = (Character *) me;
	Character *vict;
	int i = 0;
	char sTaunt[32];
	strcpy(sTaunt, "Fool, you shall die!");

	/* If we're here because of command input or if Taim is not fighting, return the function */
	if (cmd || GET_POS(taim) != POS_FIGHTING)
		return FALSE;

	/* Random chance of Taim hitting the victim again */
	if(MiscUtil::random(1, 5) == 5 && FIGHTING(taim))
	{
		do_say(taim, sTaunt, 0, 0);
		hit(taim, FIGHTING(taim), TYPE_UNDEFINED);
	}

	/* Check for each person inside Taim's room, if they are fighting him, there is a 50% chance he chooses
	 * that person to weave against. If the chance fails, check the next person fighting Taim.
	 */
	for(vict = taim->in_room->people;vict;vict = vict->next_in_room)
	{
		i = MiscUtil::random(1, 2);
		if(i == 2)
			continue;
		if(FIGHTING(vict) != taim)
			continue;
		if(vict != NULL)
			break;
	}

	if(vict == NULL || !vict && FIGHTING(vict) != taim)
		return 0;

//	if(MiscUtil::random(1, 3) == 3)
//		if (taim->CastSpell(vict, NULL, SPELL_FIREBALL, 0) < 0)
//			return FALSE;

//	if(MiscUtil::random(1, 5) == 5)
//		if (taim->CastSpell(vict, NULL, SPELL_FIREBALL, 0) < 0)
//			return FALSE;

//	if(MiscUtil::random(1, 5) == 5)
//		if (taim->CastSpell(vict, NULL, SPELL_FIREBALL, 0) < 0)
//			return FALSE;

	switch (MiscUtil::random(1, 8))
	{
		case 1:
		case 2:
		case 3:
		case 4:
			if (GET_LEVEL(vict) < LVL_IMMORT)
			{
				Act("$n throws a ball of energy, which hits your body!", 1, taim, 0, vict, TO_VICT);
				Act("$n throws a ball of energy at $N, which scorches $s body!", 1, taim, 0, vict, TO_NOTVICT);
				Act("You throw a ball of energy at $N, causing him to scream in pain.", 1, taim, 0, vict, TO_CHAR);
				dmg(taim, vict, 20, 80, TRUE);
			}
			break;

//		case 6:
//			taim->CastSpell(vict, NULL, SPELL_FIREBALL, 0);
//			break;
		case 8:
			if(GET_LEVEL(vict) < LVL_GOD)
			{
				Act("Your body is BURNED by a huge blast of flames sent by $n!!!", 1, taim, 0, vict, TO_VICT);
				Act("$n looks deeply into $N, and shoots a huge blast of flames at him!", 1, taim, 0, vict, TO_NOTVICT);
				Act("You blast a huge sheet of flame towards $N, scorching his body!.", 1, taim, 0, vict, TO_CHAR);
				dmg(taim, vict, 60, 90, TRUE);
			}
			break;
		default:
			return 0;
	}

	return TRUE;

}

