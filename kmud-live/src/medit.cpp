/************************************************************************
 * OasisOLC - medit.c						v1.5	*
 * Copyright 1996 Harvey Gilpin.					*
 ************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "spells.h"
#include "utils.h"
#include "db.h"
#include "shop.h"
#include "olc.h"
#include "handler.h"
#include "clans.h"
#include "mobs.h"
#include "zones.h"
#include "Descriptor.h"

#include "ClanUtil.h"
#include "rooms/RoomSector.h"

/*
 * Set this to 1 for debugging logs in medit_save_internally.
 */
#if 0
#define DEBUG
#endif

/*
 * Set this to 1 as a last resort to save mobiles.
 */
#if 0
#define I_CRASH
#endif

/*-------------------------------------------------------------------*/

/*
 * External variable declarations.
 */
extern class Shop *shop_index;
extern Descriptor *descriptor_list;
extern Character *character_list;
extern struct attack_hit_type attack_hit_text[];
extern const char *action_bits[];
extern const char *affected_bits[];
extern const char *position_types[];
extern const char *genders[];
extern const char *aggros[];
extern const char *class_types[];
extern const char *pc_race_types[];
extern Index *obj_index;
extern int top_shop;
#if defined(OASIS_MPROG)
extern const char *mobprog_types[];
#endif

/*-------------------------------------------------------------------*/

/*
 * Handy internal macros.
 */

#define S_KEEPER(shop) ((shop)->keeper)

/*-------------------------------------------------------------------*/

/*
 * Function prototypes.
 */
void medit_clan(Descriptor *d);
void medit_parse(Descriptor *d, char *arg);
void medit_disp_menu(Descriptor *d);
void medit_setup_new(Descriptor *d);
void medit_setup_existing(Descriptor *d, int rmob_num);
void medit_save_internally(Descriptor *d);
void init_mobile(Character *mob);
void medit_disp_positions(Descriptor *d);
void medit_disp_mob_flags(Descriptor *d);
void medit_disp_aff_flags(Descriptor *d);
void medit_disp_attack_types(Descriptor *d);
void medit_race(Descriptor *d);
void medit_class(Descriptor *d);
#if defined(OASIS_MPROG)
void medit_disp_mprog(Descriptor *d);
void medit_change_mprog(Descriptor *d);
void medit_food_menu(Description *d);
const char *medit_get_mprog_type(struct mob_prog_data *mprog);
#endif

/*-------------------------------------------------------------------*\
  utility functions
\*-------------------------------------------------------------------*/

void medit_setup_new(Descriptor *d)
{
	Character *mob;

	/*
	 * Allocate a scratch mobile structure.
	 */

	mob = new Character(CharMob);

	init_mobile(mob);
	mob->proto = true;

	GET_MOB_RNUM(mob) = -1;
	/*
	 * Set up some default strings.
	 */

	GET_ALIAS(mob) = str_dup("mob unfinished");
	GET_SDESC(mob) = str_dup("the unfinished mob");
	GET_LDESC(mob) = str_dup("An unfinished mob stands here.\r\n");
	GET_DDESC(mob) = str_dup("It looks unfinished.\r\n");

#if defined(OASIS_MPROG)

	OLC_MPROGL(d) = NULL;
	OLC_MPROG(d) = NULL;
#endif

	OLC_MOB(d) = mob;
	OLC_VAL(d) = 0;  /* Has changed flag. (It hasn't so far, we just made it.) */

	medit_disp_menu(d);
}

/*-------------------------------------------------------------------*/

void medit_setup_existing(Descriptor *d, int rmob_num)
{
	Character *mob;

	//Allocate a scratch mobile structure.

	mob = new Character(CharMob);

	MobManager::GetManager().CopyPrototype(mob, MobManager::GetManager().GetPrototype(rmob_num));

	OLC_MOB(d) = mob;
	medit_disp_menu(d);
}

/*-------------------------------------------------------------------*/
/*
 * Ideally, this function should be in db.c, but I'll put it here for
 * portability.
 */
void init_mobile(Character *mob)
{
	GET_HIT(mob) = GET_MANA(mob) = 1;
	GET_MAX_MANA(mob) = GET_MAX_MOVE(mob) = 100;
	GET_NDD(mob) = GET_SDD(mob) = 1;
	GET_WEIGHT(mob) = 200;
	GET_HEIGHT(mob) = 198;

	mob->real_abils.str = mob->real_abils.intel = mob->real_abils.wis = 20;
	mob->aff_abils = mob->real_abils;

	SET_BITK(MOB_FLAGS(mob), Q_BIT(MOB_ISNPC));
}

/*-------------------------------------------------------------------*/

#define ZCMD zone_table[zone].cmd[cmd_no]

/*
 * Save new/edited mob to memory.
 */
void medit_save_internally(Descriptor *d)
{
	int rmob_num;
	Character *mob;
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d));

	/*
	 * Mob exists? Just update it.
	 */

	if ((rmob_num = MobManager::GetManager().RealMobile(OLC_NUM(d))) != -1)
	{
		MobManager::GetManager().CopyPrototype(MobManager::GetManager().GetPrototype(rmob_num), OLC_MOB(d));
		MobManager::GetManager().UpdateLiveMobilesFromPrototype( rmob_num );
	}
	//Mob does not exist, we have to add it.
	else
	{
		MobManager::GetManager().AddPrototype(OLC_MOB(d),OLC_NUM(d));
		OLC_MOB(d) = NULL;
	}

	olc_add_to_save_list(zone->getVnum(), OLC_SAVE_MOB);
}

/**************************************************************************
 Menu functions
 **************************************************************************/


void MeditWarrantsMenu(Descriptor *d)
{
	std::list<Warrant *>::iterator iter;
	d->send("----Current Warrants----\r\n\n");

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		if(IS_SET_AR(d->olc->mob->points.warrants, (*iter)->vnum))
			d->send("  %s%d. %s%s\r\n", grn, (*iter)->vnum, yel, (*iter)->Name.c_str());
	}

	d->send("\r\n%sN%s) Add Warrant    %sD%s) Delete Warrant\r\n"
	        "%sQ%s) Exit Warrant Menu\r\n"
	        "Choose an option : ",
	        grn, nrm, grn, nrm,
	        grn, nrm);

	OLC_MODE(d) = MEDIT_WARRANTS;
}

void MeditDispNsects(Descriptor *d)
{
	Character *mob = OLC_MOB(d);
	int i = 0, c = 0;


	d->send("Setup for room sectors in which this MOB cannot wander to.\r\n");
	for (auto sectorIter = RoomSector::getStartIterator(); sectorIter != RoomSector::getEndIterator();++sectorIter)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, (*sectorIter)->getValue(), nrm, (*sectorIter)->getStandardName().c_str(), !(++c % 2) ? "\r\n" : "");
	}

	d->send("\r\nCurrently Set: ");
	for (auto sectorIter = RoomSector::getStartIterator(); sectorIter != RoomSector::getEndIterator();++sectorIter)
	{
		RoomSector *sector = (*sectorIter);
		if(IS_SET(mob->MobData->nsects, Q_BIT(sector->getValue())))
		{
			d->send(sector->getValue() ? ", " : " ");
			d->send("%s%s%s", cyn, sector->getStandardName().c_str(), nrm);
		}
	}
	d->send("\r\n\n%sQ%s) Quit", grn, nrm);

	OLC_MODE(d) = MEDIT_NSECTS;
}

void MeditDispAssists(Descriptor *d)
{
	Character *mob = OLC_MOB(d);
	int rnum = 0, count = 1;
	std::list<int>::iterator ai;

	d->send("  Mobile Assists\r\n");
	for(ai = mob->MobData->assists.begin();ai != mob->MobData->assists.end();++ai, ++count)
	{
		if( (rnum = MobManager::GetManager().RealMobile((*ai))) != -1)
			d->send("%d. %s%s%s[%s%d%s]\r\n", count, cyn,
			MobManager::GetManager().GetPrototype(rnum)->player.short_descr,
			nrm, grn, MobManager::GetManager().GetIndex(rnum)->vnum, nrm);
	}
	d->send("%sN%s) New Assists      %sD%s) Delete Assist\r\n"
	        "%sQ%s) Quit\r\n", cyn, nrm, cyn, nrm, cyn, nrm);

}

void medit_food_menu(Descriptor *d)
{
	if(!(OLC_MOB(d)->MobData->Food))
	{
		OLC_MOB(d)->MobData->Food = new FoodUnit;
		OLC_MOB(d)->MobData->Food->vnum = -1;
		OLC_MOB(d)->MobData->Food->quantity = 0;
	}

	d->send("%sA%s) Food Item Vnum   : %s%d\r\n"
	        "%sB%s) Food Quantity    : %s%d\r\n"
	        "%sD%s) Delete Food Unit\r\n"
	        "%sS%s) Save Food Unit\r\n",
	        cyn, nrm, cyn, OLC_MOB(d)->MobData->Food->vnum,
	        cyn, nrm, cyn, OLC_MOB(d)->MobData->Food->quantity,
	        cyn, nrm, cyn, nrm);

}

void medit_skin_menu(Descriptor *d)
{
	if(!(OLC_MOB(d)->MobData->Skin))
	{
		OLC_MOB(d)->MobData->Skin = new SkinningUnit;
		OLC_MOB(d)->MobData->Skin->vnum = -1;
		OLC_MOB(d)->MobData->Skin->required = 0;
		OLC_MOB(d)->MobData->Skin->skinned = false;
	}

	d->send("%sA%s) Skin Item Vnum   : %s%d\r\n"
	        "%sB%s) Skinning Requirement   : %s%d\r\n"
	        "%sD%s) Delete Skin Unit\r\n"
	        "%sS%s) Save Skin Unit\r\n",
	        cyn, nrm, cyn, OLC_MOB(d)->MobData->Skin->vnum,
	        cyn, nrm, cyn, OLC_MOB(d)->MobData->Skin->required,
	        cyn, nrm, cyn, nrm);

}

/*
 * Display positions. (sitting, standing, etc)
 */

void medit_disp_positions(Descriptor *d)
{
	int i;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");
#endif

	for (i = 0; *position_types[i] != '\n';++i)
	{
		d->send("%s%2d%s) %s\r\n", grn, i, nrm, position_types[i]);
	}

	d->send("Enter position number : ");
}

void medit_display_aggros(Descriptor *d)
{

	int i = 0;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");

#endif


	for(i = 0;i < NUM_AGGROS;++i)
		d->send("%d) %s%s%s\r\n", i + 1, grn, aggros[i], nrm);

	d->send("Current aggros:");
	for(i = 0;i < NUM_AGGROS;++i)
	{
		if(GET_AGGRO(OLC_MOB(d), i))
		{
			if(i)
				d->send(" ,");

			d->send("%s%s%s", cyn, aggros[i], nrm);
		}
	}
}


void medit_race(Descriptor *d)
{
	int i;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");

#endif

	for (i = 0; *pc_race_types[i] != '\n'; i++)
	{
		d->send("%s%2d%s) %s\r\n", grn, i, nrm, pc_race_types[i]);
	}

	d->send("Enter a race :");
}

void medit_class(Descriptor *d)
{
	int i;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");

#endif

	for (i = 0; *class_types[i] != '\n'; ++i)
	{
		d->send("%s%2d%s) %s\r\n", grn, i, nrm, class_types[i]);
	}

	d->send("Enter a race :");
}

void medit_clan(Descriptor *d)
{
	Clan *clan;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");

#endif

	for (clan = ClanList;clan;clan = clan->Next)
	{
		d->send("%s%2d%s) %s\r\n", grn, clan->vnum, nrm, clan->Name.c_str());
	}

	d->send("Enter a clan(0 for none, Q to quit) :");
}

/*-------------------------------------------------------------------*/

#if defined(OASIS_MPROG)
/*
 * Get the type of MobProg.
 */
const char *medit_get_mprog_type(struct mob_prog_data *mprog)
{
	switch (mprog->type)
	{

		case IN_FILE_PROG:
			return ">in_file_prog";

		case Act_PROG:
			return ">act_prog";

		case SPEECH_PROG:
			return ">speech_prog";

		case RAND_PROG:
			return ">rand_prog";

		case FIGHT_PROG:
			return ">fight_prog";

		case HITPRCNT_PROG:
			return ">hitprcnt_prog";

		case DEATH_PROG:
			return ">death_prog";

		case ENTRY_PROG:
			return ">entry_prog";

		case GREET_PROG:
			return ">greet_prog";

		case ALL_GREET_PROG:
			return ">all_greet_prog";

		case GIVE_PROG:
			return ">give_prog";

		case BRIBE_PROG:
			return ">bribe_prog";

	}

	return ">ERROR_PROG";
}

/*-------------------------------------------------------------------*/

/*
 * Display the MobProgs.
 */
void medit_disp_mprog(Descriptor *d)
{
	mob_prog_data *mprog = OLC_MPROGL(d);

	OLC_MTOTAL(d) = 1;

#if defined(CLEAR_SCREEN)

	d->send("^[[H^[[J");
#endif

	while (mprog)
	{
		d->character"%d) %s %s\r\n", OLC_MTOTAL(d), medit_get_mprog_type(mprog),
		(mprog->arglist ? mprog->arglist : "NONE"));
		++OLC_MTOTAL(d);
		mprog = mprog->next;
	}

	d->character"%d) Create New Mob Prog\r\n"
	"%d) Purge Mob Prog\r\n"
	"Enter number to edit [0 to exit]:  ",
	OLC_MTOTAL(d), OLC_MTOTAL(d) + 1);

	OLC_MODE(d) = MEDIT_MPROG;
              }

              /*-------------------------------------------------------------------*/

              /*
               * Change the MobProgs.
               */
              void medit_change_mprog(Descriptor *d)
              {

#if defined(CLEAR_SCREEN)
	              d->send("^[[H^[[J");
#endif

	              d->character	"1) Type: %s\r\n"
	              "2) Args: %s\r\n"
	              "3) Commands:\r\n%s\r\n\r\n"
	              "Enter number to edit [0 to exit]: ",
	              medit_get_mprog_type(OLC_MPROG(d)),
	              (OLC_MPROG(d)->arglist ? OLC_MPROG(d)->arglist: "NONE"),
	              (OLC_MPROG(d)->comlist ? OLC_MPROG(d)->comlist : "NONE"));
	              OLC_MODE(d) = MEDIT_CHANGE_MPROG;
                            }

                            /*-------------------------------------------------------------------*/

                            /*
                             * Change the MobProg type.
                             */
                            void medit_disp_mprog_types(Descriptor *d)
                            {
	                            int i;

	                            get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	                            d->send("^[[H^[[J");
#endif

	                            for (i = 0; i < NUM_PROGS-1;++i)
	                            {
		                            d->character"%s%2d%s) %s\r\n", grn, i, nrm, mobprog_types[i]);
	                            }

	                            d->send("Enter mob prog type : ");
	                            OLC_MODE(d) = MEDIT_MPROG_TYPE;
}
#endif

/*-------------------------------------------------------------------*/

/*
 * Display the gender of the mobile.
 */
void medit_disp_sex(Descriptor *d)
{
	int i;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");
#endif

	for (i = 0; i < NUM_GENDERS; ++i)
	{
		d->send("%s%2d%s) %s\r\n", grn, i, nrm, genders[i]);
	}

	d->send("Enter gender number : ");
}

/*-------------------------------------------------------------------*/

/*
 * Display attack types menu.
 */
void medit_disp_attack_types(Descriptor *d)
{
	int i;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");
#endif

	for (i = 0; i < NUM_ATTACK_TYPES; ++i)
	{
		d->send("%s%2d%s) %s\r\n", grn, i, nrm, attack_hit_text[i].singular);
	}

	d->send("Enter attack type : ");
}

/*-------------------------------------------------------------------*/

/*
 * Display mob-flags menu.
 */
void medit_disp_mob_flags(Descriptor *d)
{
	int i, columns = 0;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");
#endif

	for (i = 0; i < NUM_MOB_FLAGS;++i)
	{
		d->send("%s%2d%s) %-20.20s  %s", grn, i + 1, nrm, action_bits[i],
		        !(++columns % 2) ? "\r\n" : "");
	}

	sprintbit(MOB_FLAGS(OLC_MOB(d)), (const char **) action_bits, buf1);
	d->send("\r\nCurrent flags : %s%s%s\r\nEnter mob flags (0 to quit) : ",
	        cyn, buf1, nrm);
}

/*
 * Display affection flags menu.
 */
void medit_disp_aff_flags(Descriptor *d)
{
	int i, columns = 0;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (i = 0; i < NUM_AFF_FLAGS;++i)
	{
		d->send("%s%2d%s) %-20.20s  %s", grn, i + 1, nrm, affected_bits[i+1],
		        !(++columns % 2) ? "\r\n" : "");
	}

	sprintbitarray(AFF_FLAGS(OLC_MOB(d)), (const char **) affected_bits, AF_ARRAY_MAX, buf1);
	d->send("\r\nCurrent flags   : %s%s%s\r\nEnter aff flags (0 to quit) : ",
	        cyn, buf1, nrm);
}

/*-------------------------------------------------------------------*/

/*
 * Display main menu.
 */
void medit_disp_menu(Descriptor *d)
{
	Character *mob;
	char aggro_list[1024], nsects[1024];
	char kitnum[20];
	int i = 0;
	std::string FoodString, SkinString, WarrantStr;
	std::list<Warrant *>::iterator iter;

	mob = OLC_MOB(d);
	get_char_cols(d->character);


	aggro_list[0] = '\0';
	for(i = 0;aggros[i] && aggros[i] != "\n";++i)
	{
		if(GET_AGGRO(mob, i))
		{
			sprintf(aggro_list + strlen(aggro_list), "%s ", aggros[i]);
		}
	}
	*nsects = '\0';
	for (auto sectorIter = RoomSector::getStartIterator(); sectorIter != RoomSector::getEndIterator();++sectorIter)
	{
		if(IS_SET(mob->MobData->nsects, Q_BIT(i)))
			sprintf(nsects + strlen(nsects), "%s ", (*sectorIter)->getStandardName().c_str());
	}

	if(mob->MobData->primary_kit)
		sprintf(kitnum, "%d", mob->MobData->primary_kit->vnum);
	else
		strcpy(kitnum, "-1");

	if(!mob->MobData->Food)
		FoodString = std::string(cyn) + "None";
	else
		sprintf(FoodString, "%sQuantity: %s%d%s, Food Item: %s%s%s [%s%d%s]", cyn, grn,
		         mob->MobData->Food->quantity, cyn, grn, real_object(mob->MobData->Food->vnum) != -1 ?
		         obj_proto[real_object(mob->MobData->Food->vnum)]->short_description : "Invalid Vnum!", cyn, grn,
		         mob->MobData->Food->vnum, cyn);

	if(!mob->MobData->Skin)
		SkinString = std::string(cyn) + "None";
	else
		sprintf(SkinString, "%sRequirement: %s%d%s, Skin Item: %s%s%s [%s%d%s]", cyn, grn,
		         mob->MobData->Skin->required, cyn, grn, real_object(mob->MobData->Skin->vnum) != -1 ?
		         obj_proto[real_object(mob->MobData->Skin->vnum)]->short_description : "Invalid Vnum!", cyn, grn,
		         mob->MobData->Skin->vnum, cyn);

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		if(IS_SET_AR(mob->points.warrants, (*iter)->vnum))
		{
			if(WarrantStr.size())
				WarrantStr += " ";
			WarrantStr += (*iter)->Name;
		}
	}

	std::string clanName = "<None>";
	UserClan *userClan = mob->userClans.empty() ? NULL : mob->userClans.front();
	if(mob->userClans.empty() == false)
	{
		Clan *clan = ClanUtil::getClan(userClan->getClanId());
		if(clan != NULL)
			clanName = clan->Name;
	}

	d->send(

#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif

	    "-- Mob Number:  [%s%d%s]\r\n"
	    "%s1%s)  Sex: %s%-7.7s%s	     %s2%s) Alias: %s%s\r\n"
	    "%s3%s)  S-Desc: %s%s\r\n"
	    "%s4%s)  L-Desc:-\r\n%s%s"
	    "%s5%s)  D-Desc:-\r\n%s%s"
	    "%s6%s)  Level:       [%s%4d%s], %s7%s ) Offensive:    [%s%9d%s]\r\n"
	    "%s8%s)  Dodge:       [%s%4d%s], %s9%s ) Parry:        [%s%9d%s]\r\n"
	    "%s10%s) NumDamDice:  [%s%4d%s], %s11%s) SizeDamDice:  [%s%9d%s]\r\n"
	    "%s12%s) Num HP Dice: [%s%4d%s], %s13%s) Size HP Dice: [%s%9d%s], %s14%s) HP Bonus:  [%s%5d%s]\r\n"
	    "%s15%s) Damroll:     [%s%4d%s], %s16%s) Exp:          [%s%9d%s], %s17%s) Money:     [%s%dG, %dS, %dC%s]\r\n",

		cyn, OLC_NUM(d), nrm,
		grn, nrm, yel, genders[(int)GET_SEX(mob)], nrm,
		grn, nrm, yel, GET_ALIAS(mob).c_str(),
		grn, nrm, yel, GET_SDESC(mob),
		grn, nrm, yel, GET_LDESC(mob),
		grn, nrm, yel, GET_DDESC(mob),
		grn, nrm, cyn, GET_LEVEL(mob), nrm,
		grn, nrm, cyn, GET_OB(mob), nrm,
		grn, nrm, cyn, GET_DB(mob), nrm,
		grn, nrm, cyn, GET_PB(mob), nrm,
		grn, nrm, cyn, GET_NDD(mob), nrm,
		grn, nrm, cyn, GET_SDD(mob), nrm,
		grn, nrm, cyn, (int)GET_HIT(mob), nrm,
		grn, nrm, cyn, (int)GET_MANA(mob), nrm,
		grn, nrm, cyn, (int)GET_MOVE(mob), nrm,
		grn, nrm, cyn, (int)GET_DAMROLL(mob), nrm,
		grn, nrm, cyn, (int)GET_EXP(mob), nrm,
		grn, nrm, cyn, CalcGold(mob->points.gold), CalcSilver(mob->points.gold), CalcCopper(mob->points.gold), nrm

	);

	sprintbit(MOB_FLAGS(mob), (const char **) action_bits, buf1);
	sprintbitarray( (int *) AFF_FLAGS(mob), (const char **) affected_bits, AF_ARRAY_MAX, buf2);
	d->send(

	    "%sA%s) Position  : %s%s\r\n"
	    "%sB%s) Default   : %s%s\r\n"
	    "%sC%s) Attack    : %s%s\r\n"
	    "%sD%s) NPC Flags : %s%s\r\n"
	    "%sE%s) AFF Flags : %s%s\r\n"
	    "%sF%s) Mob Kit   : %s%s%s  [ %s%s%s ]\r\n"
	    "%sG%s) Enter Msg : %s%s\r\n"
	    "%sH%s) Exit Msg  : %s%s\r\n"
	    "%sI%s) Mob Race  : %s%s\r\n"
	    "%sV%s) Mob Moves : %s%d\r\n"
		"%sX%s) Move Regen: %s%d%s\r\n"
	    "%sK%s) Mob Clan  : %s%s\r\n"
	    "%sL%s) Mob Rank  : %s%d\r\n"
	    "%sM%s) Mob Aggros: %s%s\r\n"
	    "%sN%s) Mob Class : %s%s\r\n"
	    "%sO%s) Delete    : %s%s\r\n"
	    "%sU%s) !Move Sect: %s%s\r\n"
	    "%sT%s) Assists\r\n"
	    "%sR%s) Food Unit : %s\r\n"
		"%sY%s) Skin Unit : %s\r\n"
	    "%sW%s) Warrants  : %s%s\r\n"
#if defined(OASIS_MPROG)
	    "%sP%s) Mob Progs : %s%s\r\n"
#endif
		"%sJ%s) JavaScript: %s%s\r\n"
	    "%sQ%s) Quit\r\n"
	    "Enter choice : ",

	    grn, nrm, yel, position_types[(int)GET_POS(mob)],
	    grn, nrm, yel, position_types[(int)mob->MobData->default_pos],
	    grn, nrm, yel, attack_hit_text[GET_ATTACK(mob)].singular,
	    grn, nrm, cyn, buf1,
	    grn, nrm, cyn, buf2,
	    grn, nrm, cyn, mob->MobData->primary_kit ? mob->MobData->primary_kit->Name.c_str() : "Not set.", cyn, grn, kitnum, cyn, /* kits */
	    grn, nrm, cyn, mob->player.ArriveMessage ? mob->player.ArriveMessage : "Default Message",
	    grn, nrm, cyn, mob->player.LeaveMessage ? mob->player.LeaveMessage : "Default Message",
	    grn, nrm, cyn, pc_race_types[(int) GET_RACE(mob)],
	    grn, nrm, cyn, (int)GET_MAX_MOVE(mob),
		grn, nrm, cyn, (int)mob->MobData->moves_per_tic, (mob->MobData->moves_per_tic == -1 ? "(calculated dynamically)" : ""),
	    grn, nrm, cyn, clanName.c_str(),
	    grn, nrm, cyn, userClan ? ((int)userClan->getRank()) : 0,
	    grn, nrm, cyn, *aggro_list ? aggro_list : "NONE",
	    grn, nrm, cyn, class_types[(int) GET_CLASS(mob)],
	    grn, nrm, cyn, mob->MobData->deleted ? "Yes" : "No",
	    grn, nrm, cyn, *nsects ? nsects : "ALL SECTORS ALLOWED",
	    grn, nrm,
	    grn, nrm, FoodString.c_str(),
		grn, nrm, SkinString.c_str(),
	    grn, nrm, cyn, WarrantStr.c_str(),
		grn, nrm, cyn, (mob->js_scripts->size() ? "Set." : "Not Set."),
	    grn, nrm
	);

	//	sprintf(aggro_list, "");
	*aggro_list = '\0';

	OLC_MODE(d) = MEDIT_MAIN_MENU;
}

/************************************************************************
 *			The GARGANTAUN event handler			*
 ************************************************************************/

void medit_parse(Descriptor *d, char *arg)
{
	int i;
	std::list<int>::iterator ai;

	if (OLC_MODE(d) > MEDIT_NUMERICAL_RESPONSE && OLC_MODE(d) != MEDIT_ARRIVE && OLC_MODE(d) != MEDIT_EXIT)
	{
		if (!*arg || (!isdigit(arg[0]) && ((*arg == '-') && (!isdigit(arg[1])))))
		{
			d->send("Field must be numerical, try again : ");
			return;
		}
	}

	switch (OLC_MODE(d))
	{
			/*-------------------------------------------------------------------*/
		case MEDIT_CONFIRM_SAVESTRING:
			/*
			 * Ensure mob has MOB_ISNPC set or things will go pair shaped.
			 */

			SET_BITK(MOB_FLAGS(OLC_MOB(d)), Q_BIT(MOB_ISNPC));
			switch (*arg)
			{

				case 'y':
				case 'Y':
					/*
					 * Save the mob in memory and to disk.
					 */

					d->send("Saving mobile to memory.\r\n");
					AddOlcLog( d->character, "mob", d->olc->mob->getVnum() );
					medit_save_internally(d);
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE,
					       "OLC: %s edits mob %d", GET_NAME(d->character), OLC_NUM(d));
					/* FALL THROUGH */

				case 'n':
				case 'N':
					cleanup_olc(d, CLEANUP_ALL);
					return;

				default:
					d->send("Invalid choice!\r\n");
					d->send("Do you wish to save the mobile? : ");
					return;
			}

			break;

			/*-------------------------------------------------------------------*/
		case MEDIT_MAIN_MENU:
			i = 0;

			//This is for menu options that are numbers so we can go above 10
			if(isdigit(*arg))
			{
				switch(atoi(arg))
				{
					case 1:
						OLC_MODE(d) = MEDIT_SEX;
						medit_disp_sex(d);
						return;
					case 2:
						OLC_MODE(d) = MEDIT_ALIAS;
						--i;
						break;
					case 3:
						OLC_MODE(d) = MEDIT_S_DESC;
						--i;
						break;
					case 4:
						OLC_MODE(d) = MEDIT_L_DESC;
						--i;
						break;
					case 5:
						OLC_MODE(d) = MEDIT_D_DESC;
						d->send("Enter mob description: (/s saves /h for help)\r\n\r\n");
						d->backstr = NULL;

						if (OLC_MOB(d)->player.description)
						{
							d->send(OLC_MOB(d)->player.description);
							d->backstr = str_dup(OLC_MOB(d)->player.description);
						}

						d->str = &OLC_MOB(d)->player.description;
						d->max_str = MAX_MOB_DESC;
						OLC_VAL(d) = 1;
						return;
					case 6:
						OLC_MODE(d) = MEDIT_LEVEL;
						++i;
						break;
					case 7:
						OLC_MODE(d) = MEDIT_OB;
						++i;
						break;
					case 8:
						OLC_MODE(d) = MEDIT_DB;
						++i;
						break;
					case 9:
						OLC_MODE(d) = MEDIT_PB;
						++i;
						break;
					case 10:
						OLC_MODE(d) = MEDIT_NDD;
						++i;
						break;
					case 11:
						OLC_MODE(d) = MEDIT_SDD;
						++i;
						break;
					case 12:
						OLC_MODE(d) = MEDIT_NUM_HP_DICE;
						++i;
						break;
					case 13:
						OLC_MODE(d) = MEDIT_SIZE_HP_DICE;
						++i;
						break;
					case 14:
						OLC_MODE(d) = MEDIT_ADD_HP;
						++i;
						break;
					case 15:
						OLC_MODE(d) = MEDIT_DAMROLL;
						++i;
						break;
					case 16:
						OLC_MODE(d) = MEDIT_EXP;
						++i;
						break;
					case 17:
						OLC_MODE(d) = MEDIT_GOLD;
						++i;
						break;
					default:
						medit_disp_menu(d);
						return;

				}
			}

			else
			{
				switch (UPPER(*arg))
				{
					case 'Q':
						if (OLC_VAL(d))
						{	/* Anything been changed? */
							d->send("Do you wish to save the changes to the mobile? (y/n) : ");
							OLC_MODE(d) = MEDIT_CONFIRM_SAVESTRING;
						}

						else
							cleanup_olc(d, CLEANUP_ALL);

						return;

					case 'A':
						OLC_MODE(d) = MEDIT_POS;
						medit_disp_positions(d);
						return;
					case 'B':
						OLC_MODE(d) = MEDIT_DEFAULT_POS;
						medit_disp_positions(d);
						return;
					case 'C':
						OLC_MODE(d) = MEDIT_ATTACK;
						medit_disp_attack_types(d);
						return;
					case 'D':
						OLC_MODE(d) = MEDIT_NPC_FLAGS;
						medit_disp_mob_flags(d);
						return;
					case 'E':
						OLC_MODE(d) = MEDIT_AFF_FLAGS;
						medit_disp_aff_flags(d);
						return;
					case 'F':
						OLC_MODE(d) = MEDIT_KITS;
						d->send("Enter the number of a kit for this mob (0 for nothing) : ");
						return;
					case 'G':
						OLC_MODE(d) = MEDIT_ARRIVE;
						d->send("Enter the special entrance message (press enter for default) : ");
						return;
					case 'H':
						OLC_MODE(d) = MEDIT_EXIT;
						d->send("Enter the special exit message (press enter for default) : ");
						return;
					case 'I':
						OLC_MODE(d) = MEDIT_RACE;
						medit_race(d);
						return;
					case 'V':
						OLC_MODE(d) = MEDIT_MOVES;
						++i;
						break;
					case 'K':
						OLC_MODE(d) = MEDIT_CLANS;
						medit_clan(d);
						return;
					case 'L':
						OLC_MODE(d) = MEDIT_RANK;
						++i;
						break;
					case 'M':
						OLC_MODE(d) = MEDIT_AGGROS;
						medit_display_aggros(d);
						return;
					case 'N':
						OLC_MODE(d) = MEDIT_CLASS;
						medit_class(d);
						return;
					case 'O':
						if(OLC_MOB(d)->MobData->deleted)
						{
							d->send("This MOB will no longer be deleted.\r\n");
							OLC_MOB(d)->MobData->deleted = false;
						}
						else
						{
							d->send("This MOB will be deleted after the next reboot.\r\n");
							OLC_MOB(d)->MobData->deleted = true;
						}
						OLC_VAL(d) = 1;
						medit_disp_menu(d);
						return;
#if defined(OASIS_MPROG)

					case 'P':
						OLC_MODE(d) = MEDIT_MPROG;
						medit_disp_mprog(d);
						return;
#endif

					case 'U':
						OLC_MODE(d) = MEDIT_NSECTS;
						MeditDispNsects(d);
						return;
					case 'T':
						OLC_MODE(d) = MEDIT_ASSISTS;
						MeditDispAssists(d);
						return;
					case 'J':
						OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_MAIN_MENU;
						OLC_MODE(d) = OLC_JSCRIPT_EDIT;
						d->olc->jsScripts = d->olc->mob->js_scripts;
						JScriptDispMenu(d);
						return;
					case 'R':
						OLC_MODE(d) = MEDIT_FOOD_UNIT;
						medit_food_menu(d);
						return;
					case 'Y':
						OLC_MODE(d) = MEDIT_SKIN_UNIT;
						medit_skin_menu(d);
						return;
					case 'W':
						OLC_MODE(d) = MEDIT_WARRANTS;
						MeditWarrantsMenu(d);
						return;
					case 'X':
						OLC_MODE(d) = MEDIT_MOVES_PER_TIC;
						d->send("Enter the number of moves that this mob should regenerate while standing, or -1 to for this to be calculated dynamically : ");
						return;
					default:
						medit_disp_menu(d);
						return;
				}
			}

			if (i != 0)
			{
				d->send(i == 1 ? "\r\nEnter new value : " :
				        i == -1 ? "\r\nEnter new text :\r\n] " :
				        "\r\nOops...:\r\n");

				return;
			}

			break;
		case MEDIT_NSECTS:
			if(toupper(*arg) == 'Q')
			{
				break;
			}
			else if(!RoomSector::getEnumByValue(atoi(arg)))
			{
				d->send("That number is out of range. Try again(Q to quit) : ");
				return;
			}
			TOGGLE_BIT(OLC_MOB(d)->MobData->nsects, Q_BIT(atoi(arg)));
			MeditDispNsects(d);
			return;
		case MEDIT_ASSISTS:
			switch(toupper(*arg))
			{
				case 'N':
					d->send("Enter the virtual number of the mobile to have added to the assist list :");
					OLC_MODE(d) = MEDIT_NEW_ASSIST;
					return;
				case 'D':
					if(OLC_MOB(d)->MobData->assists.size() <= 0)
					{
						d->send("There mob assists are empty already.\r\n");
					}
					else
					{
						d->send("Enter the number on the list to have removed :");
						OLC_MODE(d) = MEDIT_DELETE_ASSIST;
					}
					return;
				case 'Q':
					break;
				default:
					d->send("Invalid option.\r\nTry again : ");
					return;
			}
			break;
		case MEDIT_NEW_ASSIST:
			if(toupper(*arg) == 'Q' || !atoi(arg))
			{
				OLC_MODE(d) = MEDIT_ASSISTS;
				MeditDispAssists(d);
				return;
			}
			if(MobManager::GetManager().RealMobile(atoi(arg)) == -1)
			{
				d->send("Invalid mobile. Try again('Q' to quit) :");
			}
			else
			{
				for(ai = OLC_MOB(d)->MobData->assists.begin();ai != OLC_MOB(d)->MobData->assists.end();++ai)
				{
					if(atoi(arg) == (*ai))
					{
						d->send("That mobile is already on the assist list.\r\n Try again('Q' to quit) : ");
						return;
					}
				}
				OLC_MOB(d)->MobData->assists.push_back(atoi(arg));
				OLC_MODE(d) = MEDIT_ASSISTS;
				MeditDispAssists(d);
			}
			return;
		case MEDIT_DELETE_ASSIST:
			if(toupper(*arg) == 'Q' || !atoi(arg))
			{
				OLC_MODE(d) = MEDIT_ASSISTS;
				MeditDispAssists(d);
				return;
			}
			for(i = 1, ai = OLC_MOB(d)->MobData->assists.begin();ai != OLC_MOB(d)->MobData->assists.end();++ai, ++i)
			{
				if(i == atoi(arg))
				{
					OLC_MOB(d)->MobData->assists.erase(ai);
					OLC_MODE(d) = MEDIT_ASSISTS;
					MeditDispAssists(d);
					return;
				}
			}
			d->send("Invalid number. Try again('Q' to quit) :");
			return;
		case MEDIT_WARRANTS:
			switch(toupper(*arg))
			{
				case 'N':
					if(!(Warrants.size()))
					{
						d->send("There are no warrants made. Ask a 104+ to add one for you.\r\n");
						return;
					}
					d->send("Enter the virtual number of the warrant you wish to add(-1 to quit) : ");
					OLC_MODE(d) = MEDIT_WARRANTS_NEW;
					return;
				case 'D':
					d->send("Enter the warrant you wish to remove(-1 to quit) : ");
					OLC_MODE(d) = MEDIT_WARRANTS_DELETE;
					return;
				case 'Q':
					break;
			}
			break;
		case MEDIT_WARRANTS_NEW:
			if(atoi(arg) == -1)
			{
				MeditWarrantsMenu(d);
				return;
			}
			if(!(WarrantByVnum(atoi(arg))))
			{
				d->send("That warrant does not exit. Try another : ");
				return;
			}
			SET_BIT_AR(d->olc->mob->points.warrants, atoi(arg));
			MeditWarrantsMenu(d);
			return;
		case MEDIT_WARRANTS_DELETE:
			if(atoi(arg) == -1)
			{
				MeditWarrantsMenu(d);
				return;
			}
			if(!(IS_SET_AR(d->olc->mob->points.warrants, atoi(arg))))
			{
				d->send("This MOB does not have that warrant set.\r\n");
				break;
			}
			REMOVE_BIT_AR(d->olc->mob->points.warrants, atoi(arg));
			MeditWarrantsMenu(d);
			return;
			/*-------------------------------------------------------------------*/
		case OLC_JSCRIPT_EDIT:
			if( JScriptParse(d, arg) )
				return;
			break;
			/*-------------------------------------------------------------------*/
		case MEDIT_ALIAS:
			if (!GET_ALIAS(OLC_MOB(d)).empty())
				GET_ALIAS(OLC_MOB(d)).erase();

			GET_ALIAS(OLC_MOB(d)) = str_dup((arg && *arg) ? arg : "undefined");
			break;
			/*-------------------------------------------------------------------*/
		case MEDIT_S_DESC:
			if (GET_SDESC(OLC_MOB(d)))
				delete[] (GET_SDESC(OLC_MOB(d)));
			GET_SDESC(OLC_MOB(d)) = str_dup((arg && *arg) ? arg : "undefined");
			break;
			/*-------------------------------------------------------------------*/
		case MEDIT_L_DESC:
			if (GET_LDESC(OLC_MOB(d)))
				delete[] (GET_LDESC(OLC_MOB(d)));

			if (arg && *arg)
			{
				strcpy(buf, arg);
				strcat(buf, "\r\n");
				GET_LDESC(OLC_MOB(d)) = str_dup(buf);
			}

			else
				GET_LDESC(OLC_MOB(d)) = str_dup("undefined");
			break;

		case MEDIT_ARRIVE:
			if (OLC_MOB(d)->player.ArriveMessage)
				delete[] (OLC_MOB(d)->player.ArriveMessage);

			if (arg && *arg)
				OLC_MOB(d)->player.ArriveMessage = str_dup(arg);
			else
				OLC_MOB(d)->player.ArriveMessage = NULL;
			break;
		case MEDIT_EXIT:
			if (OLC_MOB(d)->player.LeaveMessage)
				delete[] (OLC_MOB(d)->player.LeaveMessage);

			if (arg && *arg)
				OLC_MOB(d)->player.LeaveMessage = str_dup(arg);
			else
				OLC_MOB(d)->player.LeaveMessage = NULL;
			break;
			/*-------------------------------------------------------------------*/
		case MEDIT_D_DESC:
			/*
			 * We should never get here.
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: medit_parse(): Reached D_DESC case!");
			d->send("Oops...\r\n");
			break;
			/*-------------------------------------------------------------------*/
#if defined(OASIS_MPROG)

		case MEDIT_MPROG_COMLIST:
			/*
			 * We should never get here, but if we do, bail out.
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog("SYSERR: OLC: medit_parse(): Reached MPROG_COMLIST case!",
			       BRF, LVL_BUILDER, TRUE);
			break;
#endif
			/*-------------------------------------------------------------------*/
		case MEDIT_NPC_FLAGS:
			if ((i = atoi(arg)) == 0)
				break;

			else if (!((i < 0) || (i > NUM_MOB_FLAGS)))
				TOGGLE_BIT(MOB_FLAGS(OLC_MOB(d)), Q_BIT((i - 1)));

			medit_disp_mob_flags(d);
			return;
			/*-------------------------------------------------------------------*/
		case MEDIT_AFF_FLAGS:
			if ((i = atoi(arg)) == 0)
				break;

			else if (!((i < 0) || (i > NUM_AFF_FLAGS)))
				TOGGLE_BIT_AR(AFF_FLAGS(OLC_MOB(d)), (i));

			medit_disp_aff_flags(d);
			return;
		case MEDIT_KITS:
			if ( (i = atoi(arg)) == 0)
			{
				OLC_MOB(d)->MobData->primary_kit = NULL;
				break;
			}
			else
			{
				if ( (OLC_MOB(d)->MobData->primary_kit = KitManager::GetManager().GetKitByVnum(i)) == NULL)
					d->send("That kit does not exist!  What kit? (0 for nothing) : ");
				else
					break;
			}
			return;
		case MEDIT_AGGROS:
			i = atoi(arg);

			if(i <= 0 || i > NUM_AGGROS)
				break;
			else
			{
				--i;

				if(GET_AGGRO(OLC_MOB(d), i))
					REMOVE_BIT(d->olc->mob->MobData->aggro, Q_BIT(i));
				else
					SET_BITK(d->olc->mob->MobData->aggro, Q_BIT(i));

				medit_display_aggros(d);
				return;
			}
			break;
		case MEDIT_CLANS:
			i = atoi(arg);

			if (i > NumClans())
			{
				d->sendRaw("That clan does not exist.\r\n");
				return;
			}
			else if(i == 0)
			{
				d->sendRaw("No clan assigned.\r\n");
				ClanUtil::freeUserClans(OLC_MOB(d)->userClans);
				break;
			}
			else if(toupper(*arg) == 'Q')
			{
				break;
			}
			else
			{
				if(!ClanUtil::getClan(i))
				{
					d->send("That clan does not exit.\r\n");
					return;
				}
				else if(OLC_MOB(d)->userClans.empty() == false)
				{
					OLC_MOB(d)->userClans.front()->setClanId(i);
				}
				else
				{
					UserClan *userClan = UserClan::setupNewInstance(OLC_MOB(d)->getUserId(), i);
					OLC_MOB(d)->userClans.push_back(userClan);
				}
				break;
			}
		case MEDIT_FOOD_UNIT:
			if(!*arg)
				break;
			switch(toupper(*arg))
			{
				case 'A':
					OLC_MODE(d) = MEDIT_FOOD_VNUM;
					d->send("Enter the vnum of the item that will be butchered from this MOB's corpse, 'Q' to quit: ");
					return;
				case 'B':
					d->send("Enter the quantity of this item that can be butchered from this MOB, 'Q' to quit : ");
					OLC_MODE(d) = MEDIT_FOOD_QUANTITY;
					return;
				case 'D':
					if(OLC_MOB(d)->MobData->Food)
					{
						delete OLC_MOB(d)->MobData->Food;
						OLC_MOB(d)->MobData->Food = NULL;
					}
					d->send("Food unit deleted.\r\n");
					break;
				case 'S':
					OLC_MODE(d) = MEDIT_MAIN_MENU;
					d->send("Food unit saved!\r\n");
					break;
				default:
					d->send("Invalid option.\r\n");
					medit_food_menu(d);
					return;
			}
			break;
		case MEDIT_FOOD_VNUM:
			i = real_object(atoi(arg));
			if(toupper(*arg) == 'Q')
			{
				OLC_MODE(d) = MEDIT_FOOD_UNIT;
				medit_food_menu(d);
				return;
			}
			else if(i == -1)
				d->send("Invalid item number!\r\n");
			else if (obj_proto[i]->getType() != ITEM_FOOD)
				d->send("Invalid food type: Must be a food item.\r\n");
			else
			{
				d->send("Item set to %d.\r\n", atoi(arg));
				OLC_MOB(d)->MobData->Food->vnum = atoi(arg);
				OLC_MODE(d) = MEDIT_FOOD_UNIT;
				medit_food_menu(d);
			}
			return;
		case MEDIT_FOOD_QUANTITY:
			i = atoi(arg);
			if(toupper(*arg) == 'Q')
			{
				OLC_MODE(d) = MEDIT_FOOD_UNIT;
				medit_food_menu(d);
				return;
			}
			else if(i < 0)
				d->send("Quantity must be zero or above.\r\n");
			else
			{
				d->send("Quantity set to %d.\r\n", atoi(arg));
				OLC_MOB(d)->MobData->Food->quantity = atoi(arg);
				OLC_MODE(d) = MEDIT_FOOD_UNIT;
				medit_food_menu(d);
			}
			return;
		case MEDIT_SKIN_UNIT:
			if(!*arg)
				break;
			switch(toupper(*arg))
			{
				case 'A':
					OLC_MODE(d) = MEDIT_SKIN_VNUM;
					d->send("Enter the vnum of the item that will be skinned from this MOB's corpse, 'Q' to quit: ");
					return;
				case 'B':
					d->send("Enter the Skinning rating needed to skin this MOB, 'Q' to quit : ");
					OLC_MODE(d) = MEDIT_SKIN_REQUIRED;
					return;
				case 'D':
					if(OLC_MOB(d)->MobData->Skin)
					{
						delete OLC_MOB(d)->MobData->Skin;
						OLC_MOB(d)->MobData->Skin = NULL;
					}
					d->send("Skin unit deleted.\r\n");
					break;
				case 'S':
					OLC_MODE(d) = MEDIT_MAIN_MENU;
					d->send("Skin unit saved!\r\n");
					break;
				default:
					d->send("Invalid option.\r\n");
					medit_skin_menu(d);
					return;
			}
			break;
		case MEDIT_SKIN_VNUM:
			i = real_object(atoi(arg));
			if(toupper(*arg) == 'Q')
			{
				OLC_MODE(d) = MEDIT_SKIN_UNIT;
				medit_skin_menu(d);
				return;
			}
			else if(i == -1)
				d->send("Invalid item number!\r\n");
			else if (obj_proto[i]->getType() != ITEM_SKIN)
				d->send("Invalid skin type: Must be a skin item.\r\n");
			else
			{
				d->send("Item set to %d.\r\n", atoi(arg));
				OLC_MOB(d)->MobData->Skin->vnum = atoi(arg);
				OLC_MODE(d) = MEDIT_SKIN_UNIT;
				medit_skin_menu(d);
			}
			return;
		case MEDIT_SKIN_REQUIRED:
			i = atoi(arg);
			if(toupper(*arg) == 'Q')
			{
				OLC_MODE(d) = MEDIT_SKIN_UNIT;
				medit_skin_menu(d);
				return;
			}
			else if(i < 0)
				d->send("Skinning requirement must be zero or above.\r\n");
			else
			{
				d->send("Skinning requirement set to %d.\r\n", atoi(arg));
				OLC_MOB(d)->MobData->Skin->required = atoi(arg);
				OLC_MODE(d) = MEDIT_SKIN_UNIT;
				medit_skin_menu(d);
			}
			return;
			/*-------------------------------------------------------------------*/
#if defined(OASIS_MPROG)

		case MEDIT_MPROG:
			if ((i = atoi(arg)) == 0)
				medit_disp_menu(d);

			else if (i == OLC_MTOTAL(d))
			{
				struct mob_prog_data *temp;
				CREATE(temp, struct mob_prog_data, 1);
				temp->next = OLC_MPROGL(d);
				temp->type = -1;
				temp->arglist = NULL;
				temp->comlist = NULL;
				OLC_MPROG(d) = temp;
				OLC_MPROGL(d) = temp;
				OLC_MODE(d) = MEDIT_CHANGE_MPROG;
				medit_change_mprog (d);
			}
			else if (i < OLC_MTOTAL(d))
			{
				struct mob_prog_data *temp;
				int x = 1;

				for (temp = OLC_MPROGL(d); temp && x < i; temp = temp->next)
					x++;

				OLC_MPROG(d) = temp;
				OLC_MODE(d) = MEDIT_CHANGE_MPROG;
				medit_change_mprog (d);
			}
			else if (i == OLC_MTOTAL(d) + 1)
			{
				d->send("Which mob prog do you want to purge? ");
				OLC_MODE(d) = MEDIT_PURGE_MPROG;
			}
			else
				medit_disp_menu(d);
			return;
		case MEDIT_PURGE_MPROG:
			if ((i = atoi(arg)) > 0 && i < OLC_MTOTAL(d))
			{
				struct mob_prog_data *temp;
				int x = 1;

				for (temp = OLC_MPROGL(d); temp && x < i; temp = temp->next)
					++x;

				OLC_MPROG(d) = temp;
				REMOVE_FROM_LIST(OLC_MPROG(d), OLC_MPROGL(d), next);
				delete[] (OLC_MPROG(d)->arglist);
				delete[] (OLC_MPROG(d)->comlist);
				delete[] (OLC_MPROG(d));
				OLC_MPROG(d) = NULL;
				OLC_VAL(d) = 1;
			}

			medit_disp_mprog(d);
			return;
		case MEDIT_CHANGE_MPROG:
			{
				if ((i = atoi(arg)) == 1)
					medit_disp_mprog_types(d);
				else if (i == 2)
				{
					d->send ("Enter new arg list: ");
					OLC_MODE(d) = MEDIT_MPROG_ARGS;
				}
				else if (i == 3)
				{
					d->send("Enter new mob prog commands:\r\n");

					/*
					* Pass control to modify.c for typing.
					*/

					OLC_MODE(d) = MEDIT_MPROG_COMLIST;
					d->backstr = NULL;

					if (OLC_MPROG(d)->comlist)
					{
						d->send((OLC_MPROG(d)->comlist);
						        d->backstr = str_dup(OLC_MPROG(d)->comlist);
					        }
					        d->str = &OLC_MPROG(d)->comlist;
					d->max_str = MAX_STRING_LENGTH;
					OLC_VAL(d) = 1;
				}
				else
					medit_disp_mprog(d);
				return;
#endif

				/*-------------------------------------------------------------------*/

				/*
				 * Numerical responses.
				 */

#if defined(OASIS_MPROG)
				/*
				David Klasinc suggests for MEDIT_MPROG_TYPE:
				switch (atoi(arg)) {
					case 0: OLC_MPROG(d)->type = 0; break;
					case 1: OLC_MPROG(d)->type = 1; break;
					case 2: OLC_MPROG(d)->type = 2; break;
					case 3: OLC_MPROG(d)->type = 4; break;
					case 4: OLC_MPROG(d)->type = 8; break;
					case 5: OLC_MPROG(d)->type = 16; break;
					case 6: OLC_MPROG(d)->type = 32; break;
					case 7: OLC_MPROG(d)->type = 64; break;
					case 8: OLC_MPROG(d)->type = 128; break;
					case 9: OLC_MPROG(d)->type = 256; break;
					case 10: OLC_MPROG(d)->type = 512; break;
					case 11: OLC_MPROG(d)->type = 1024; break;
					default: OLC_MPROG(d)->type = -1; break;
					}
				*/

			case MEDIT_MPROG_TYPE:
				OLC_MPROG(d)->type = (1 << MAX(0, MIN(atoi(arg), NUM_PROGS - 1)));
				OLC_VAL(d) = 1;
				medit_change_mprog(d);
				return;
			case MEDIT_MPROG_ARGS:
				OLC_MPROG(d)->arglist = str_dup(arg);
				OLC_VAL(d) = 1;
				medit_change_mprog(d);
				return;
#endif

			case MEDIT_SEX:
				GET_SEX(OLC_MOB(d)) = MAX(0, MIN(NUM_GENDERS - 1, atoi(arg)));
				break;
			case MEDIT_OB:
				GET_OB(OLC_MOB(d)) = MAX(-200, MIN(10000, atoi(arg)));
				break;
			case MEDIT_DB:
				GET_DB(OLC_MOB(d)) = MAX(-200, MIN(10000, atoi(arg)));
				break;
			case MEDIT_DAMROLL:
				GET_DAMROLL(OLC_MOB(d)) = MAX(0, MIN(50, atoi(arg)));
				break;
			case MEDIT_NDD:
				GET_NDD(OLC_MOB(d)) = MAX(0, MIN(30, atoi(arg)));
				break;
			case MEDIT_SDD:
				GET_SDD(OLC_MOB(d)) = MAX(0, MIN(127, atoi(arg)));
				break;
			case MEDIT_MOVES_PER_TIC:
				OLC_MOB(d)->MobData->moves_per_tic = MAX(-1, MIN(999, atoi(arg)));
				break;
			case MEDIT_NUM_HP_DICE:
				GET_HIT(OLC_MOB(d)) = MAX(0, MIN(30, atoi(arg)));
				break;
			case MEDIT_SIZE_HP_DICE:
				GET_MANA(OLC_MOB(d)) = MAX(0, MIN(1000, atoi(arg)));
				break;
			case MEDIT_ADD_HP:
				GET_MOVE(OLC_MOB(d)) = MAX(0, MIN(30000, atoi(arg)));
				break;
			case MEDIT_PB:
				GET_PB(OLC_MOB(d)) = MAX(-200, MIN(10000, atoi(arg)));
				break;
			case MEDIT_MOVES:
				GET_MAX_MOVE(OLC_MOB(d)) = MAX(0, MIN(3000, atoi(arg)));
				break;
			case MEDIT_EXP:
				GET_EXP(OLC_MOB(d)) = MAX(0, atoi(arg));
				break;
			case MEDIT_GOLD:
				OLC_MOB(d)->points.gold = MAX(0, atoi(arg));
				break;
			case MEDIT_POS:
				GET_POS(OLC_MOB(d)) = MAX(0, MIN(NUM_POSITIONS - 1, atoi(arg)));
				break;
			case MEDIT_DEFAULT_POS:
				OLC_MOB(d)->MobData->default_pos = MAX(0, MIN(NUM_POSITIONS - 1, atoi(arg)));
				break;
			case MEDIT_ATTACK:
				GET_ATTACK(OLC_MOB(d)) = MAX(0, MIN(NUM_ATTACK_TYPES - 1, atoi(arg)));
				break;
			case MEDIT_LEVEL:
				GET_LEVEL(OLC_MOB(d)) = MAX(1, MIN(100, atoi(arg)));
				break;
			case MEDIT_RACE:
				GET_RACE(OLC_MOB(d)) = MAX(0, MIN(NUM_RACES, atoi(arg)));
				break;
			case MEDIT_RANK:
				//if(OLC_MOB(d)->clans)
				//	OLC_MOB(d)->rank = MAX(0, MIN(10, atoi(arg)));
				break;
			case MEDIT_CLASS:
				GET_CLASS(OLC_MOB(d)) = MAX(0, MIN(NUM_CLASSES, atoi(arg)));
				break;
				/*-------------------------------------------------------------------*/
			default:
				/*
				* We should never get here.
				*/
				MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: medit_parse(): Reached default case of %d!", OLC_MODE(d));
				d->send("Oops...\r\n");
				cleanup_olc(d, CLEANUP_ALL);
				return;
			}
			/*-------------------------------------------------------------------*/

			/*
			* END OF CASE
			* If we get here, we have probably changed something, and now want to
			* return to main menu.  Use OLC_VAL as a 'has changed' flag
			*/

			OLC_VAL(d) = 1;
			medit_disp_menu(d);
	}
	/*
	 * End of medit_parse(), thank god.
	 */
