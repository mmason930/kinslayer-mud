/************************************************************************
 * OasisOLC - Game configuration / cedit.c                   v2.0	    *
 * Copyright 2002-2003 Kip Potter   (kip_potter@hotmail.com)            *
 * A graphical in-game game configuration utility for OasisOLC.         *
 ************************************************************************/

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../comm.h"
#include "../interpreter.h"
#include "../utils.h"
#include "../db.h"
#include "../constants.h"
#include "olc.h"
#include "../shop.h"
#include "../weaves.h"
#include "../auction.h"
#include "../clans.h"
#include "../js/js.h"

#include "../MiscUtil.h"
#include "../StringUtil.h"
#include "../ClanUtil.h"
#include "../Descriptor.h"
#include "../rooms/Room.h"

extern int circle_restrict;
extern DateTime rebootTime;

/******************************************************************************/
/** External Functions                                                       **/
/******************************************************************************/
void free_config(Config *data);

Config::Config()
{
	memset(&this->play, 0, sizeof(this->play));
	memset(&this->csd, 0, sizeof(this->csd));
	memset(&this->room_nums, 0, sizeof(this->room_nums));
	memset(&this->operation, 0, sizeof(this->operation));
	this->empty_world = false;
}

/******************************************************************************/
/** Internal Functions                                                       **/
/******************************************************************************/
void cedit_disp_menu(Descriptor *d);
void cedit_setup(Descriptor *d);
int  save_config();
void reassign_rooms(void);

extern GameTime time_info;

/******************************************************************************/
/** Routines                                                                 **/
/******************************************************************************/
CommandHandler do_cedit = DEFINE_COMMAND
{
	Descriptor *d;
	char buf1[MAX_STRING_LENGTH];

	/****************************************************************************/
	/** Parse any arguments.                                                   **/
	/****************************************************************************/
	OneArgument(argument, buf1);

	if (GET_LEVEL(ch) < LVL_GRGOD && !PLR_FLAGGED(ch, PLR_WEAVE_EDITOR) && !PLR_FLAGGED(ch, PLR_CHARGE_EDITOR)
		&& !PLR_FLAGGED(ch, PLR_GLOBAL_SCRIPTS))
	{
		ch->send("You can't modify the game configuration.\r\n");
		return;
	}

	d = ch->desc;

	if (!*buf1)
	{
		d->olc = new OLC();
		OLC_ZONE(d) = 0;
		cedit_setup(d);
		STATE(d) = CON_CEDIT;
		Act("$n starts using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
		SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_WRITING));

		MudLog(BRF, LVL_IMMORT, TRUE,
		       "OLC: %s starts editing the game configuration.", GET_NAME(ch));
	}
	else if (str_cmp("save", buf1) != 0)
	{
		ch->send("Yikes!  Stop that, someone will get hurt!\r\n");
		return;
	}
	else
	{

		ch->send("Saving the game configuration.\r\n");
		MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(ch)), TRUE,
		       "OLC: %s saves the game configuration.", GET_NAME(ch));

		Conf->save();
	}
};

/*-------------------------------------------------------------------*/

void cedit_setup(Descriptor *d)
{
	/****************************************************************************/
	/** Create the Config struct.                                         **/
	/****************************************************************************/
	OLC_CONFIG(d) = new Config();

	/****************************************************************************/
	/** Copy the current configuration from the config_info to this one.       **/
	/****************************************************************************/
	/****************************************************************************/
	/** Copy the game play options from the configuration info struct.         **/
	/****************************************************************************/

	OLC_CONFIG(d)->play.level_can_shout			=	CONFIG_LEVEL_CAN_SHOUT;
	OLC_CONFIG(d)->play.tunnel_size				=	CONFIG_TUNNEL_SIZE;
	OLC_CONFIG(d)->play.max_exp_gain			=	CONFIG_MAX_EXP_GAIN;
	OLC_CONFIG(d)->play.max_npc_corpse_time		=	CONFIG_MAX_NPC_CORPSE_TIME;
	OLC_CONFIG(d)->play.max_pc_corpse_time		=	CONFIG_MAX_PC_CORPSE_TIME;
	OLC_CONFIG(d)->play.idle_void				=	CONFIG_IDLE_VOID;
	OLC_CONFIG(d)->play.idle_rent_time			=	CONFIG_IDLE_RENT_TIME;
	OLC_CONFIG(d)->play.load_into_inventory		=	CONFIG_LOAD_INVENTORY;
	OLC_CONFIG(d)->play.max_hit_exp				=	Conf->play.max_hit_exp;
	OLC_CONFIG(d)->play.hit_exp_mult            =   Conf->play.hit_exp_mult;
	OLC_CONFIG(d)->play.ExpMultiplier			=	Conf->play.ExpMultiplier;
	OLC_CONFIG(d)->play.WpGainMultiplier		=	Conf->play.WpGainMultiplier;
	OLC_CONFIG(d)->play.WpLossMultiplier		=	Conf->play.WpLossMultiplier;
	OLC_CONFIG(d)->play.reset_time				=	Conf->play.reset_time;
	OLC_CONFIG(d)->play.restat_time				=	Conf->play.restat_time;

	/****************************************************************************/
	/** Crash Saves                                                            **/
	/****************************************************************************/
	OLC_CONFIG(d)->csd.free_rent				=	CONFIG_FREE_RENT;
	OLC_CONFIG(d)->csd.max_obj_save				=	CONFIG_MAX_OBJ_SAVE;
	OLC_CONFIG(d)->csd.auto_save				=	CONFIG_AUTO_SAVE;
	OLC_CONFIG(d)->csd.autosave_time			=	CONFIG_AUTOSAVE_TIME;

	/****************************************************************************/
	/** Room Numbers                                                           **/
	/****************************************************************************/
	memcpy(&OLC_CONFIG(d)->room_nums, &Conf->room_nums, sizeof(Conf->room_nums));

	/****************************************************************************/
	/** Game Operation                                                         **/
	/****************************************************************************/
	OLC_CONFIG(d)->operation.max_playing        =	CONFIG_MAX_PLAYING;
	OLC_CONFIG(d)->operation.max_bad_pws        =	CONFIG_MAX_BAD_PWS;
	OLC_CONFIG(d)->operation.use_new_socials    =	CONFIG_NEW_SOCIALS;
	OLC_CONFIG(d)->operation.auto_save_olc      =	CONFIG_OLC_SAVE;
	OLC_CONFIG(d)->operation.nameserver_is_slow =	CONFIG_NS_IS_SLOW;

	/****************************************************************************/
	/** Allocate space for the strings.                                        **/
	/****************************************************************************/
	OLC_CONFIG(d)->play.OK       = str_dup(CONFIG_OK);
	OLC_CONFIG(d)->play.NOPERSON = str_dup(CONFIG_NOPERSON);
	OLC_CONFIG(d)->play.NOEFFECT = str_dup(CONFIG_NOEFFECT);
	OLC_CONFIG(d)->play.switch_restriction = Conf->play.switch_restriction;

	if (CONFIG_LOGNAME)
		OLC_CONFIG(d)->operation.LOGNAME     = str_dup(CONFIG_LOGNAME);
	else
		OLC_CONFIG(d)->operation.LOGNAME     = NULL;

	if (CONFIG_WELC_MESSG)
		OLC_CONFIG(d)->operation.WELC_MESSG  = str_dup(CONFIG_WELC_MESSG);
	else
		OLC_CONFIG(d)->operation.WELC_MESSG  = NULL;

	if (CONFIG_START_MESSG)
		OLC_CONFIG(d)->operation.START_MESSG = str_dup(CONFIG_START_MESSG);
	else
		OLC_CONFIG(d)->operation.START_MESSG = NULL;

	if (Conf->operation.NEWBIE_MSG)
		OLC_CONFIG(d)->operation.NEWBIE_MSG = str_dup(Conf->operation.NEWBIE_MSG);
	else
		OLC_CONFIG(d)->operation.NEWBIE_MSG = NULL;

	cedit_disp_menu(d);
}

/******************************************************************************/

void cedit_save_internally(Descriptor *d)
{
	/****************************************************************************/
	/** Copy the data back from the descriptor to the config_info structure.   **/
	/****************************************************************************/
	CONFIG_LEVEL_CAN_SHOUT		= OLC_CONFIG(d)->play.level_can_shout;
	CONFIG_MAX_EXP_GAIN			= OLC_CONFIG(d)->play.max_exp_gain;
	CONFIG_MAX_NPC_CORPSE_TIME	= OLC_CONFIG(d)->play.max_npc_corpse_time;
	CONFIG_MAX_PC_CORPSE_TIME	= OLC_CONFIG(d)->play.max_pc_corpse_time;
	CONFIG_IDLE_VOID			= OLC_CONFIG(d)->play.idle_void;
	CONFIG_IDLE_RENT_TIME		= OLC_CONFIG(d)->play.idle_rent_time;
	CONFIG_LOAD_INVENTORY		= OLC_CONFIG(d)->play.load_into_inventory;
	Conf->play.max_hit_exp		= OLC_CONFIG(d)->play.max_hit_exp;
	Conf->play.hit_exp_mult     = OLC_CONFIG(d)->play.hit_exp_mult;
	Conf->play.ExpMultiplier	= OLC_CONFIG(d)->play.ExpMultiplier;
	Conf->play.WpGainMultiplier	= OLC_CONFIG(d)->play.WpGainMultiplier;
	Conf->play.WpLossMultiplier = OLC_CONFIG(d)->play.WpLossMultiplier;
	Conf->play.reset_time		= OLC_CONFIG(d)->play.reset_time;
	Conf->play.restat_time		= OLC_CONFIG(d)->play.restat_time;
	Conf->play.switch_restriction=OLC_CONFIG(d)->play.switch_restriction;

	/****************************************************************************/
	/** Crash Saves                                                            **/
	/****************************************************************************/
	CONFIG_FREE_RENT			= OLC_CONFIG(d)->csd.free_rent;
	CONFIG_MAX_OBJ_SAVE			= OLC_CONFIG(d)->csd.max_obj_save;
	CONFIG_AUTO_SAVE			= OLC_CONFIG(d)->csd.auto_save;
	CONFIG_AUTOSAVE_TIME		= OLC_CONFIG(d)->csd.autosave_time;

	/****************************************************************************/
	/** Room Numbers                                                           **/
	/****************************************************************************/
	memcpy(&Conf->room_nums, &OLC_CONFIG(d)->room_nums, sizeof(Conf->room_nums));

	/****************************************************************************/
	/** Game Operation                                                         **/
	/****************************************************************************/
	CONFIG_MAX_PLAYING			= OLC_CONFIG(d)->operation.max_playing;
	CONFIG_MAX_BAD_PWS			= OLC_CONFIG(d)->operation.max_bad_pws;
	CONFIG_NEW_SOCIALS			= OLC_CONFIG(d)->operation.use_new_socials;
	CONFIG_NS_IS_SLOW			= OLC_CONFIG(d)->operation.nameserver_is_slow;
	CONFIG_OLC_SAVE				= OLC_CONFIG(d)->operation.auto_save_olc;

	/****************************************************************************/
	/** Allocate space for the strings.                                        **/
	/****************************************************************************/
	if (CONFIG_OK)
		delete [] CONFIG_OK;
	CONFIG_OK				= OLC_CONFIG(d)->play.OK;

	if (CONFIG_NOPERSON)
		delete [] CONFIG_NOPERSON;
	CONFIG_NOPERSON			= OLC_CONFIG(d)->play.NOPERSON;

	if (CONFIG_NOEFFECT)
		delete []CONFIG_NOEFFECT;
	CONFIG_NOEFFECT			= OLC_CONFIG(d)->play.NOEFFECT;

	if (CONFIG_LOGNAME)
		delete CONFIG_LOGNAME;
	if (OLC_CONFIG(d)->operation.LOGNAME)
		CONFIG_LOGNAME		= OLC_CONFIG(d)->operation.LOGNAME;
	else
		CONFIG_LOGNAME		= NULL;

	if (CONFIG_WELC_MESSG)
		delete [] CONFIG_WELC_MESSG;
	if (OLC_CONFIG(d)->operation.WELC_MESSG)
		CONFIG_WELC_MESSG	= OLC_CONFIG(d)->operation.WELC_MESSG;
	else
		CONFIG_WELC_MESSG	= NULL;

	if (CONFIG_START_MESSG)
		delete [] CONFIG_START_MESSG;
	if (OLC_CONFIG(d)->operation.START_MESSG)
		CONFIG_START_MESSG = OLC_CONFIG(d)->operation.START_MESSG;
	else
		CONFIG_START_MESSG = NULL;

	if (Conf->operation.NEWBIE_MSG)
		delete [] Conf->operation.NEWBIE_MSG;
	if (OLC_CONFIG(d)->operation.NEWBIE_MSG)
		Conf->operation.NEWBIE_MSG = OLC_CONFIG(d)->operation.NEWBIE_MSG;
	else
		Conf->operation.NEWBIE_MSG = NULL;

	//add_to_save_list(NOWHERE, SL_CFG);
}

/******************************************************************************/

void Config::RenumberRooms()
{
	this->room_nums.human_start.ptr		= FindRoomByVnum(this->room_nums.human_start.vnum);
	this->room_nums.trolloc_start.ptr	= FindRoomByVnum(this->room_nums.trolloc_start.vnum);
	this->room_nums.aiel_start.ptr		= FindRoomByVnum(this->room_nums.aiel_start.vnum);
	this->room_nums.seanchan_start.ptr	= FindRoomByVnum(this->room_nums.seanchan_start.vnum);
	this->room_nums.immortal_start.ptr	= FindRoomByVnum(this->room_nums.immortal_start.vnum);
	this->room_nums.frozen_start.ptr	= FindRoomByVnum(this->room_nums.frozen_start.vnum);
}

void Config::Load()
{
	char line[2048], tag[32], strip[512];
	int n = 0;

	Tips = new std::list<std::string>();

	//Needs to be placed as part of the configuration...
	CarryWeight.resize(26);
	CarryWeight[0] = 0;
	CarryWeight[1] = 2;
	CarryWeight[2] = 5;
	CarryWeight[3] = 8;
	CarryWeight[4] = 14;
	CarryWeight[5] = 25;
	CarryWeight[6] = 38;
	CarryWeight[7] = 50;
	CarryWeight[8] = 63;
	CarryWeight[9] = 75;
	CarryWeight[10] = 93;
	CarryWeight[11] = 102;
	CarryWeight[12] = 115;
	CarryWeight[13] = 132;
	CarryWeight[14] = 145;
	CarryWeight[15] = 162;
	CarryWeight[16] = 178;
	CarryWeight[17] = 193;
	CarryWeight[18] = 201;
	CarryWeight[19] = 212;
	CarryWeight[20] = 235;
	CarryWeight[21] = 252;
	CarryWeight[22] = 300;
	CarryWeight[23] = 400;
	CarryWeight[24] = 600;
	CarryWeight[25] = 1000;

	DaysPerRealMonth[0]  = 31;
	DaysPerRealMonth[1]  = 28;
	DaysPerRealMonth[2]  = 31;
	DaysPerRealMonth[3]  = 30;
	DaysPerRealMonth[4]  = 31;
	DaysPerRealMonth[5]  = 30;
	DaysPerRealMonth[6]  = 31;
	DaysPerRealMonth[7]  = 31;
	DaysPerRealMonth[8]  = 30;
	DaysPerRealMonth[9]  = 31;
	DaysPerRealMonth[10] = 30;
	DaysPerRealMonth[11] = 31;

	sql::Query query = gameDatabase->sendQuery("SELECT name,value FROM config");

	bool rebootTimeSet = false;
	while(query->hasNextRow()) {

		sql::Row row = query->getRow();

		strcpy(tag, row.getString("name").c_str());
		strcpy(line, row.getString("value").c_str());

		int num = atoi(line);
		double dec = atof(line);

		if(!strcmp(tag, "Reboot"))
		{
			if( sscanf(line, "%d:%d:%d", &Conf->operation.defaultRebootTime.hour, &Conf->operation.defaultRebootTime.minute, &Conf->operation.defaultRebootTime.second) == 3)
			{
				rebootTime = DateTime();
				rebootTime.setHour(Conf->operation.defaultRebootTime.hour);
				rebootTime.setMinute(Conf->operation.defaultRebootTime.minute);
				rebootTime.setSecond(Conf->operation.defaultRebootTime.second);
				rebootTime.moveDayForward();
				rebootTimeSet = true;
			}
		}
		else if(!strcmp(tag, "World"))
			Conf->empty_world = (!strcmp(line,"empty"));
		else if(!strcmp(tag, "Max_Playing"))
			CONFIG_MAX_PLAYING = num;
		else if(!strcmp(tag, "Max_Bad_Pwds"))
			CONFIG_MAX_BAD_PWS = num;
		else if(!strcmp(tag, "NS_Slow"))
			CONFIG_NS_IS_SLOW = num;
		else if(!strcmp(tag, "Auto_Save_OLC"))
			CONFIG_OLC_SAVE = num;
		else if(!strcmp(tag, "Welcome_Msg"))
			CONFIG_WELC_MESSG = str_dup(line);
		else if(!strcmp(tag, "Start_Msg"))
			CONFIG_START_MESSG = str_dup(line);
		else if(!strcmp(tag, "Shout Level"))
			CONFIG_LEVEL_CAN_SHOUT = num;
		else if(!strcmp(tag, "Tunnel Size"))
			CONFIG_TUNNEL_SIZE = num;
		else if(!strcmp(tag, "Max_EXP_Gain"))
			CONFIG_MAX_EXP_GAIN = num;
		else if(!strcmp(tag, "Mob Corpse Time"))
			CONFIG_MAX_NPC_CORPSE_TIME = num;
		else if(!strcmp(tag, "Player Corpse Time"))
			CONFIG_MAX_PC_CORPSE_TIME = num;
		else if(!strcmp(tag, "Switch Restriction"))
		{
			if( !strcmp(line,"Yes") )
				play.switch_restriction=true;
			else if( !strcmp(line,"No") )
				play.switch_restriction=false;
		}
		else if(!strcmp(tag, "BodyPercents"))
		{
			for(n = 0;n < NUM_WEARS && *line;++n)
			{
				BodyPercents.resize(BodyPercents.size() + 1);
				HalfChop(line, strip, line);
				BodyPercents[n] = atoi(strip);
			}
			if(BodyPercents.size() != (unsigned int)NUM_WEARS)
			{
				Log("BodyPercents(%lld) is different size from NUM_WEARS(%lld).", (__int64)BodyPercents.size(),
					(__int64)NUM_WEARS);
			}
		}
		else if(!strcmp(tag, "Idle Time"))
			CONFIG_IDLE_VOID = num;
		else if(!strcmp(tag, "Idle Rent"))
			CONFIG_IDLE_RENT_TIME = num;
		else if(!strcmp(tag, "Load_Inv"))
			CONFIG_LOAD_INVENTORY = num;
		else if(!strcmp(tag, "OK"))
			CONFIG_OK = str_dup(line);
		else if(!strcmp(tag, "NoPerson"))
			CONFIG_NOPERSON = str_dup(line);
		else if(!strcmp(tag, "NoEffect"))
			CONFIG_NOEFFECT = str_dup(line);
		else if(!strcmp(tag, "FreeRent"))
			CONFIG_FREE_RENT = num;
		else if(!strcmp(tag, "MaxObj"))
			CONFIG_MAX_OBJ_SAVE = num;
		else if(!strcmp(tag, "AutoSave"))
			CONFIG_AUTO_SAVE = num;
		else if(!strcmp(tag, "AutoSaveTime"))
			CONFIG_AUTOSAVE_TIME = num;
		else if(!strcmp(tag, "Human Room"))
			Conf->room_nums.human_start.vnum = num;
		else if(!strcmp(tag, "Trolloc Room"))
			Conf->room_nums.trolloc_start.vnum = num;
		else if(!strcmp(tag, "Aiel Room"))
			Conf->room_nums.aiel_start.vnum = num;
		else if(!strcmp(tag, "Seanchan Room"))
			Conf->room_nums.seanchan_start.vnum = num;
		else if(!strcmp(tag, "Imm Room"))
			Conf->room_nums.immortal_start.vnum = num;
		else if(!strcmp(tag, "Frozen Room"))
			Conf->room_nums.frozen_start.vnum = num;
		else if(!strcmp(tag, "Wizlock"))
			circle_restrict = num;
		else if(!strcmp(tag, "Bash"))
		{
			sscanf(line, "%f %f %f %d %f %f %f %f %f %d", &BashData.O_OBMultiplier,
				&BashData.O_BMIMultiplier, &BashData.O_WeaponWeightMultiplier,
				&BashData.D_BaseBonus, &BashData.D_DBMultiplier, &BashData.D_BMIMultiplier,
				&BashData.WeightDifferenceMultiplier, &BashData.LevelDifferenceMultiplier,
				&BashData.WeaponDifferenceMultiplier, &BashData.O_BMIOffset);
		}
		else if(!strcmp(tag, "Charge"))
		{
			int b1, b2, b3;
			sscanf(line, "%f %f %f %d %f %f %f %f %f %d %d %d %d %d %d",
				&ChargeData.O_OBMultiplier, &ChargeData.O_BMIMultiplier,
				&ChargeData.O_WeaponWeightMalus, &ChargeData.D_BaseBonus, &ChargeData.D_DBMultiplier,
				&ChargeData.O_BMIMultiplier, &ChargeData.O_NegativeSway, &ChargeData.O_PositiveSway,
				&ChargeData.Timer, &b1, &b2, &ChargeData.D_PulsesLagOnLand, &ChargeData.O_PulsesLagOnLand,
				&ChargeData.O_PulsesLagOnMiss, &b3);

			ChargeData.CanStartOnFightingVict = (b1?(true):(false));
			ChargeData.CanStartWhileFighting  = (b2?(true):(false));
			ChargeData.CanFinishWhileFighting  = (b2?(true):(false));
		}
		else if(!strcmp(tag, "Precise Strike"))
		{
			sscanf(line, "%f %f %f %d %f %f %f", &PreciseStrikeData.O_OBMultiplier, &PreciseStrikeData.O_OffenseRollFactor, 
				&PreciseStrikeData.O_WeightMultiplier, &PreciseStrikeData.D_BaseDB, &PreciseStrikeData.D_BashedMultiplier, 
				&PreciseStrikeData.D_DBMultiplier, &PreciseStrikeData.D_PBMultiplier);
		}
		else if(!strcmp(tag, "Flee"))
		{
			sscanf(line, "%d %d %d %d %d %hd %d", &FleeData.PulsesPerUnengagedFlee, &FleeData.PulsesPerEngagedFlee,
			       &FleeData.PulsesPerAutoFlee, &FleeData.PulsesRemovedPerTic, &FleeData.PulsesForFirstFlee,
				   &FleeData.MaxFleeLag, &FleeData.PulsesPerExit);
		}
		else if(!strcmp(tag, "Melee"))
		{
			sscanf(line, "%f %f %f %f", &MeleeData.WardMultiplier, &MeleeData.ParrySplit, &MeleeData.DodgeMultiplier, &MeleeData.ParryMultiplier);
		}
		else if(!strcmp(tag, "Taint"))
		{
			sscanf(line, "%d %d %d %d %d %f %f %f %f %f %f %d %d %d %d", &TaintData.TaintForRandomCommand,
			       &TaintData.TaintForRandomChannel, &TaintData.StrainToTaint, &TaintData.SeizeCost,
			       &TaintData.HungerThirstCost, &TaintData.LevelStrainRemoval, &TaintData.WisStrainRemoval,
			       &TaintData.BlackTowerBonus, &TaintData.TimerCost, &TaintData.ManaCost, &TaintData.FailureCost,
			       &TaintData.TaintForRandomSocial, &TaintData.TaintForRandomVoice, &TaintData.TaintForRandomTarget,
			       &TaintData.TaintForSeizeDizziness);
		}
		else if(!strcmp(tag, "Lbs Per Offense"))
			MeleeData.PoundsPerOffense = num;
		else if(!strcmp(tag, "Lbs Per Dodge"))
			MeleeData.PoundsPerDodge = num;
		else if(!strcmp(tag, "Lbs Per Parry"))
			MeleeData.PoundsPerParry = num;
		else if(!strcmp(tag, "Max Hit Exp"))
			Conf->play.max_hit_exp = num;
		else if(!strcmp(tag, "Hit Exp Mult"))
			Conf->play.hit_exp_mult = dec;
		else if(!strcmp(tag, "EXP Mult"))
			Conf->play.ExpMultiplier = dec;
		else if(!strcmp(tag, "WP Gain Mult"))
			Conf->play.WpGainMultiplier = dec;
		else if(!strcmp(tag, "WP Loss Mult"))
			Conf->play.WpLossMultiplier = dec;
		else if(!strcmp(tag, "Tip"))
			Tips->push_back(line);
		else if(!strcmp(tag, "Reset"))
			Conf->play.reset_time = DateTime(num);
		else if(!strcmp(tag, "Restat"))
			Conf->play.restat_time = DateTime(num);
		else if(!strcmp(tag, "Reboots"))
			Conf->NumberOfReboots = num + 1;
		else if(!strcmp(tag, "Newbie Msg"))
			Conf->operation.NEWBIE_MSG = str_dup(line);
		else
			MudLog(CMP, LVL_APPR, TRUE, "Invalid Tag in Config::load(): %s", tag);
	}

	if(rebootTimeSet == false) {
		rebootTime = DateTime();
		rebootTime.moveDayForward();//Default will be 24 hours from boot time.
	}
}

void Config::save()
{
	int i = 0;
	std::list<std::string>::iterator si;

	try {

		gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempConfig");

		gameDatabase->sendRawQuery("CREATE TABLE tempConfig LIKE config");

		std::stringstream sql;

		sql << "INSERT INTO tempConfig(`name`,`value`) VALUES";

		sql << "('Reboot','" << std::setfill('0') << std::setw(2) << Conf->operation.defaultRebootTime.hour << ":" << std::setw(2) << Conf->operation.defaultRebootTime.minute << ":" << std::setw(2) << Conf->operation.defaultRebootTime.second << "'),"
			<< "('World','" << (Conf->empty_world ? "empty" : "full") << "'),"
			<< "('Max_Playing','" << CONFIG_MAX_PLAYING << "'),"
			<< "('Max_Bad_Pwds','" << CONFIG_MAX_BAD_PWS << "'),"
			<< "('NS_Slow','" << CONFIG_NS_IS_SLOW << "'),"
			<< "('Auto_Save_OLC','" << CONFIG_OLC_SAVE << "'),"
			<< "('Welcome_Msg','" << CONFIG_WELC_MESSG << "'),"
			<< "('Start_Msg','" << (Conf->operation.START_MESSG ? Conf->operation.START_MESSG : "") << "'),"
			<< "('Shout Level','" << CONFIG_LEVEL_CAN_SHOUT << "'),"
			<< "('Tunnel Size','" << CONFIG_TUNNEL_SIZE << "'),"
			<< "('Max_EXP_Gain','" << CONFIG_MAX_EXP_GAIN << "'),"
			<< "('Mob Corpse Time','" << CONFIG_MAX_NPC_CORPSE_TIME << "'),"
			<< "('Player Corpse Time','" << CONFIG_MAX_PC_CORPSE_TIME << "'),"
			<< "('Idle Time','" << CONFIG_IDLE_VOID << "'),"
			<< "('Idle Rent','" << CONFIG_IDLE_RENT_TIME << "'),"
			<< "('Load_Inv','" << CONFIG_LOAD_INVENTORY << "'),"
			<< "('OK','" << CONFIG_OK << "'),"
			<< "('NoPerson','" << CONFIG_NOPERSON << "'),"
			<< "('NoEffect','" << CONFIG_NOEFFECT << "'),"
			<< "('FreeRent','" << CONFIG_FREE_RENT << "'),"
			<< "('MaxObj','" << CONFIG_MAX_OBJ_SAVE << "'),"
			<< "('AutoSave','" << CONFIG_AUTO_SAVE << "'),"
			<< "('AutoSaveTime','" << CONFIG_AUTOSAVE_TIME << "'),"
			<< "('Newbie Msg','" << (Conf->operation.NEWBIE_MSG ? Conf->operation.NEWBIE_MSG : "") << "'),"
			<< "('Human Room','" << Conf->room_nums.human_start.vnum << "'),"
			<< "('Trolloc Room','" << Conf->room_nums.trolloc_start.vnum << "'),"
			<< "('Imm Room','" << Conf->room_nums.immortal_start.vnum << "'),"
			<< "('Frozen Room','" << Conf->room_nums.frozen_start.vnum << "'),"
			<< "('Lbs Per Offense','" << MeleeData.PoundsPerOffense << "'),"
			<< "('Lbs Per Dodge','" << MeleeData.PoundsPerDodge << "'),"
			<< "('Lbs Per Parry','" << MeleeData.PoundsPerParry << "'),"
			<< "('Wizlock','" << circle_restrict << "'),"
			<< "('Max Hit Exp','" << Conf->play.max_hit_exp << "'),"
			<< "('Hit Exp Mult','" << Conf->play.hit_exp_mult << "'),"
			<< "('Reboots','" << Conf->GetReboots() << "'),"
			<< "('Restat','" << Conf->play.restat_time.getTime() << "'),"
			<< "('Reset','" << Conf->play.reset_time.getTime() << "'),"
			<< "('EXP Mult','" << Conf->play.ExpMultiplier << "'),"
			<< "('WP Gain Mult','" << Conf->play.WpGainMultiplier << "'),"
			<< "('WP Loss Mult','" << Conf->play.WpLossMultiplier << "'),"
			<< "('Switch Restriction','" << (Conf->play.switch_restriction ? "Yes" : "No") << "'),"
			<< "('Flee','"	<< FleeData.PulsesPerUnengagedFlee << " "
							<< FleeData.PulsesPerEngagedFlee << " "
							<< FleeData.PulsesPerAutoFlee << " "
							<< FleeData.PulsesRemovedPerTic << " "
							<< FleeData.PulsesForFirstFlee << " "
							<< FleeData.MaxFleeLag << " "
							<< FleeData.PulsesPerExit << "'),"
			<< "('Bash','"	<< BashData.O_OBMultiplier << " "
							<< BashData.O_BMIMultiplier << " "
							<< BashData.O_WeaponWeightMultiplier << " "
							<< BashData.D_BaseBonus << " "
							<< BashData.D_DBMultiplier << " "
							<< BashData.D_BMIMultiplier << " "
							<< BashData.WeightDifferenceMultiplier << " "
							<< BashData.LevelDifferenceMultiplier << " "
							<< BashData.WeaponDifferenceMultiplier << " "
							<< BashData.O_BMIOffset << "'),"
			<< "('Charge','"<< ChargeData.O_OBMultiplier << " "
							<< ChargeData.O_BMIMultiplier << " "
							<< ChargeData.O_WeaponWeightMalus << " "
							<< ChargeData.D_BaseBonus << " "
							<< ChargeData.D_DBMultiplier << " "
							<< ChargeData.O_BMIMultiplier << " "
							<< ChargeData.O_NegativeSway << " "
							<< ChargeData.O_PositiveSway << " "
							<< ChargeData.Timer << " "
							<< ChargeData.CanStartOnFightingVict << " "
							<< ChargeData.CanStartWhileFighting << " "
							<< ChargeData.D_PulsesLagOnLand << " "
							<< ChargeData.O_PulsesLagOnLand << " "
							<< ChargeData.O_PulsesLagOnMiss << " "
							<< ChargeData.CanFinishWhileFighting << "'),"
			<< "('Precise Strike','"		<< PreciseStrikeData.O_OBMultiplier << " "
										<< PreciseStrikeData.O_OffenseRollFactor << " "
										<< PreciseStrikeData.O_WeightMultiplier << " "
										<< PreciseStrikeData.D_BaseDB << " "
										<< PreciseStrikeData.D_BashedMultiplier << " "
										<< PreciseStrikeData.D_DBMultiplier << " " 
										<< PreciseStrikeData.D_PBMultiplier << "'),"
			<< "('Melee','"	<< MeleeData.WardMultiplier << " "
							<< MeleeData.ParrySplit << " "
							<< MeleeData.DodgeMultiplier << " "
							<< MeleeData.ParryMultiplier << "'),"
			<< "('Taint','"	<< TaintData.TaintForRandomCommand << " "
							<< TaintData.TaintForRandomChannel << " "
							<< TaintData.StrainToTaint << " "
							<< TaintData.SeizeCost << " "
							<< TaintData.HungerThirstCost << " "
							<< TaintData.LevelStrainRemoval << " "
							<< TaintData.WisStrainRemoval << " "
							<< TaintData.BlackTowerBonus << " "
							<< TaintData.TimerCost << " "
							<< TaintData.ManaCost << " "
							<< TaintData.FailureCost << " "
							<< TaintData.TaintForRandomSocial << " "
							<< TaintData.TaintForRandomVoice << " "
							<< TaintData.TaintForRandomTarget << " "
							<< TaintData.TaintForSeizeDizziness << "'),";

			sql << "('BodyPercents','";

		for(i = 0;i < (int) BodyPercents.size();++i)
		{
			sql << BodyPercents[i];

			if( i + 1 < (int)BodyPercents.size() ) {

				sql << " ";
			}
		}

		sql << "'),";

		int index = 0;
		for(si = Tips->begin();si != Tips->end();++si)
		{
			sql << "('Tip','" << sql::escapeString((*si)) << "')";
			if(index + 1 < (int)Tips->size()) {

				sql << ",";
			}

			++index;
		}

		gameDatabase->sendRawQuery(sql.str());

		gameDatabase->sendRawQuery("RENAME TABLE config TO configOld, tempConfig TO config, configOld TO tempConfig");

		gameDatabase->sendRawQuery("DROP TABLE tempConfig");
	}
	catch(sql::QueryException e) {

		MudLog(BRF, LVL_APPR, TRUE, "Failed to save config: %s", e.getMessage().c_str());
	}
}

/**************************************************************************
 Menu functions
 **************************************************************************/

/*
 * the main menu
 */
void cedit_disp_menu(Descriptor *d)
{
	get_char_cols(d->character);

	/*
	 * Menu header
	 */
	d->send(
	    "OasisOLC MUD Configuration Editor\r\n"
		"%sA%s) Auction Houses\r\n"
	    "%sG%s) Game Play Options\r\n"
		"%sJ%s) Global JavaScripts\r\n"
	    "%sC%s) Crashsave/Rent Options\r\n"
	    "%sR%s) Room Numbers\r\n"
	    "%sO%s) Operation Options\r\n"
	    "%sS%s) Skill and Balance Editor\r\n"
	    "%sT%s) Time Info\r\n"
	    "%sQ%s) Quit\r\n"
	    "Enter your choice : ",

	    grn, nrm,
		grn, nrm,
		grn, nrm,
	    grn, nrm,
	    grn, nrm,
	    grn, nrm,
	    grn, nrm,
//	    grn, nrm,
		grn, nrm,
	    grn, nrm
	);

	OLC_MODE(d) = CEDIT_MAIN_MENU;
}

/*------------------------------------------------------------------*/
void CeditTimeMenu(Descriptor *d)
{
	get_char_cols(d->character);

	d->send("\r\n"
	        "%s1%s)  Seconds per MUD Hour/Tic     : %s%d\r\n"
	        "%sQ%s)  Exit to Main Menu\r\n",
	        grn, nrm, cyn, time_info.SecondsPerTic,
	        grn, nrm);

	OLC_MODE(d) = CEDIT_TIME_MENU;
}

/*-------------------------------------------------------------------*/

void CeditSkillMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n\r\n"
	        "%s1%s)  Bash\r\n"
	        "%s2%s)  Charge\r\n"
	        "%s3%s)  Backstab\r\n"
	        "%s4%s)  Ride\r\n"
	        "%s5%s)  Cover Tracks\r\n"
	        "%s6%s)  Melee\r\n"
	        "%s7%s)  Body Part Hit Percentages\r\n"
	        "%s8%s)  Taint of Saidin\r\n"
	        "%s9%s)  Flee and Fleelag\r\n"
			"%s10%s) Weaves\r\n"
			"%s11%s) Precise Strike\r\n"
	        "%sQ%s)  Exit To The Main Menu\r\n"
	        "Enter your choice : ",
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
	        grn, nrm,
			grn, nrm,
			grn, nrm,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_SKILLS_MENU;
}

void CeditWeaveMenu(Descriptor *d)
{
	get_char_cols(d->character);

	d->send("\r\n\r\n");
	d->send( WeaveManager::GetManager().ListWeaves( 0, d->character ).c_str() );
	d->send("\r\n");
	d->send("%sA%s) Add Skill\r\n", grn, nrm);
	d->send("%sD%s) Delete Skill\r\n", grn, nrm);
	d->send("%sQ%s) Quit\r\nEnter your choice: ", grn, nrm);

	OLC_MODE(d) = CEDIT_WEAVE_MENU;
}

void CeditDisplayWeave(Descriptor* d)
{
	get_char_cols(d->character);

	d->send("\r\n\r\n");
	d->send( d->olc->weave->listAttributes(d, "Vnum").c_str() );
	d->send("\r\n%sA%s) Add Attribute", grn, nrm);
	d->send("\r\n%sD%s) Delete Attribute", grn, nrm);
	d->send("\r\n%sQ%s) Quit\r\nEnter your choice: ", grn, nrm);

	OLC_MODE(d) = CEDIT_WEAVE_VIEW;	
}

void CeditFleeMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("NOTE: 1 PULSE = 1/%d of a second\r\n", PASSES_PER_SEC);
	d->send("\r\n\r\n"
	        "%s1%s)  Unengaged Flee Pulse Increment          : %s%d\r\n"
	        "%s2%s)  Engaged Flee Pulse Increment            : %s%d\r\n"
	        "%s3%s)  Auto Flee Pulse Increment               : %s%d\r\n"
	        "%s4%s)  Pulses Removed Per Tic                  : %s%d\r\n"
	        "%s5%s)  Pulses For First Flee                   : %s%d\r\n"
	        "%s6%s)  Maximum Flee Lag Pulses                 : %s%d\r\n"
			"%s7%s)  Added Pulses For Fewer Exits            : %s%d\r\n"
	        "%sQ%s)  Return to Skill Menu\r\n",
	        grn, nrm, cyn, FleeData.PulsesPerUnengagedFlee,
	        grn, nrm, cyn, FleeData.PulsesPerEngagedFlee,
	        grn, nrm, cyn, FleeData.PulsesPerAutoFlee,
	        grn, nrm, cyn, FleeData.PulsesRemovedPerTic,
	        grn, nrm, cyn, FleeData.PulsesForFirstFlee,
	        grn, nrm, cyn, FleeData.MaxFleeLag,
			grn, nrm, cyn, FleeData.PulsesPerExit,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_FLEE;
}

void CeditTaintMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n\r\n"
	        "%s1%s)  Taint For Random Command          : %s%d\r\n"
	        "%s2%s)  Taint For Random Weave            : %s%d\r\n"
	        "%s3%s)  Taint For Random Social           : %s%d\r\n"
	        "%s4%s)  Taint For Random Voices           : %s%d\r\n"
	        "%s5%s)  Taint For Random Target           : %s%d\r\n"
	        "%s6%s)  Strain to Taint                   : %s%d\r\n"
	        "%s7%s)  Seizing Cost for Strain           : %s%d\r\n"
	        "%s8%s)  Hunger/Thirst Cost                : %s%d\r\n"
	        "%s9%s)  Strain Per Level Removed          : %s%.2f\r\n"
	        "%s10%s) Strain Per Wisdom Removed         : %s%.2f\r\n"
	        "%s11%s) Black Tower Strain %% Removed      : %s%.2f\r\n"
	        "%s12%s) Weave Timer Cost                  : %s%.2f\r\n"
	        "%s13%s) Weave Failure Cost                : %s%.2f\r\n"
	        "%s14%s) Weave's Mana Cost                 : %s%.2f\r\n"
	        "%s15%s) Taint For Seize Dizziness         : %s%d\r\n"
	        "%sQ%s)  Return to Skill Menu\r\n",
	        grn, nrm, cyn, TaintData.TaintForRandomCommand,
	        grn, nrm, cyn, TaintData.TaintForRandomChannel,
	        grn, nrm, cyn, TaintData.TaintForRandomSocial,
	        grn, nrm, cyn, TaintData.TaintForRandomVoice,
	        grn, nrm, cyn, TaintData.TaintForRandomTarget,
	        grn, nrm, cyn, TaintData.StrainToTaint,
	        grn, nrm, cyn, TaintData.SeizeCost,
	        grn, nrm, cyn, TaintData.HungerThirstCost,
	        grn, nrm, cyn, TaintData.LevelStrainRemoval,
	        grn, nrm, cyn, TaintData.WisStrainRemoval,
	        grn, nrm, cyn, TaintData.BlackTowerBonus,
	        grn, nrm, cyn, TaintData.TimerCost,
	        grn, nrm, cyn, TaintData.FailureCost,
	        grn, nrm, cyn, TaintData.ManaCost,
	        grn, nrm, cyn, TaintData.TaintForSeizeDizziness,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_TAINT;
}

void CeditTipMenu(Descriptor *d)
{
	get_char_cols(d->character);
	std::list<std::string>::iterator si;
	int i = 1;

	d->send("Newbie Tip Menu\r\n\n");

	for(i = 1, si = Tips->begin();si != Tips->end();++si, ++i)
	{
		d->send("%s%d%s)  %s%s%s%s\r\n",
		        cyn, i, nrm, grn, bld, (*si).c_str(), nrm);
	}
	d->send("\r\n%sN%s) New Tip   %sD%s) Delete Tip\r\n"
	        "%sQ%s) Quit\r\n", grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE(d) = CEDIT_TIP_MENU;
}

void CeditBodyPercentsMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n"
	        "%s1%s)  Head         : %s%d%%\r\n"
	        "%s2%s)  Shoulders    : %s%d%%\r\n"
	        "%s3%s)  Arms         : %s%d%%\r\n"
	        "%s4%s)  Hands        : %s%d%%\r\n"
	        "%s5%s)  Body         : %s%d%%\r\n"
	        "%s6%s)  Waist        : %s%d%%\r\n"
	        "%s7%s)  Legs         : %s%d%%\r\n"
	        "%s8%s)  Feet         : %s%d%%\r\n"
	        "%sQ%s)  Return to Skills Menu\r\n",
	        grn, nrm, cyn, BodyPercents[WEAR_HEAD],
	        grn, nrm, cyn, BodyPercents[WEAR_SHOULDERS],
	        grn, nrm, cyn, BodyPercents[WEAR_ARMS],
	        grn, nrm, cyn, BodyPercents[WEAR_HANDS],
	        grn, nrm, cyn, BodyPercents[WEAR_BODY],
	        grn, nrm, cyn, BodyPercents[WEAR_WAIST],
	        grn, nrm, cyn, BodyPercents[WEAR_LEGS],
	        grn, nrm, cyn, BodyPercents[WEAR_FEET],
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_BODY_PERCENTS;
}

void CeditChargeMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n"
	        "Current Formula:\r\n"
			"if( ( ((OB * %f) + (BMI * %f) - (Weapon Weight * %f)) * Skill %% * Weapon Skill %%) >\r\n"
			"    ( (%d + (DB * %f)) * Dodge Skill %%))\r\n"
	        "%s1%s)  Offensive Bonus Multiplier                 : %s%f\r\n"
	        "%s2%s)  Offense BMI Multiplier                     : %s%f\r\n"
	        "%s3%s)  Offensive Weapon Weight Malus Multiplier   : %s%f\r\n"
			"%s4%s)  Offensive Negative Sway                    : %s%f\r\n"
			"%s5%s)  Offensive Positive Sway                    : %s%f\r\n"
	        "%s6%s)  Dodge Base Bonus                           : %s%d\r\n"
	        "%s7%s)  Dodge Bonus Multiplier                     : %s%f\r\n"
	        "%s8%s)  Timer                                      : %s%f\r\n"
	        "%s9%s)  Can Start Charging Engaged Victim          : %s%s\r\n"
	        "%sA%s)  Can Start Charging While Engaged           : %s%s\r\n"
	        "%sB%s)  Can Finish Charging While Engaged          : %s%s\r\n"
			"%sC%s)  Victim's Number of Lag Pulses On Land      : %s%d\r\n"
			"%sD%s)  Charger's Number of Lag Pulses On Land     : %s%d\r\n"
			"%sE%s)  Charger's Number of Lag Pulses on Miss     : %s%d\r\n"
	        "%sQ%s)  Exit Charge Editor\r\n",
	        ChargeData.O_OBMultiplier, ChargeData.O_BMIMultiplier, ChargeData.O_WeaponWeightMalus,
	        ChargeData.D_BaseBonus, ChargeData.D_DBMultiplier,
	        cyn, nrm, cyn, ChargeData.O_OBMultiplier,
	        cyn, nrm, cyn, ChargeData.O_BMIMultiplier,
	        cyn, nrm, cyn, ChargeData.O_WeaponWeightMalus,
			cyn, nrm, cyn, ChargeData.O_NegativeSway,
			cyn, nrm, cyn, ChargeData.O_PositiveSway,
	        cyn, nrm, cyn, ChargeData.D_BaseBonus,
	        cyn, nrm, cyn, ChargeData.D_DBMultiplier,
	        cyn, nrm, cyn, ChargeData.Timer,
			cyn, nrm, cyn, (ChargeData.CanStartOnFightingVict ? "Yes" : "No"),
			cyn, nrm, cyn, (ChargeData.CanStartWhileFighting ? "Yes" : "No"),
			cyn, nrm, cyn, (ChargeData.CanFinishWhileFighting ? "Yes" : "No"),
			cyn, nrm, cyn, ChargeData.D_PulsesLagOnLand,
			cyn, nrm, cyn, ChargeData.O_PulsesLagOnLand,
			cyn, nrm, cyn, ChargeData.O_PulsesLagOnMiss,
	        cyn, nrm
	       );
	OLC_MODE(d) = CEDIT_CHARGE;
}

void CeditBashMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n"
	        "Current Formula:\r\n"
	        "if( ( (OB * %f) + (BMI * %f + %d) + (Weapon Weight * %f) * Skill %% * Weapon Skill %%) >\r\n"
			"    ( (%d + (DB * %f)) * Dodge Skill %%))\r\n"
	        "%s1%s)  Offensive Bonus Multiplier           : %s%f\r\n"
	        "%s2%s)  Offensive BMI Multiplier             : %s%f\r\n"
			"%s3%s)  Offensive BMI Offset                 : %s%d\r\n"
	        "%s4%s)  Offensive Weapon Weight Multiplier   : %s%f\r\n"
	        "%s5%s)  Dodge Base Bonus                     : %s%d\r\n"
	        "%s6%s)  Dodge Bonus Multiplier               : %s%f\r\n"
	        "%s7%s)  Weight Difference Failure Multiplier : %s%f\r\n"
	        "%s8%s)  Level Difference Failure Multiplier  : %s%f\r\n"
			"%s9%s)  Weapon Difference Success Multiplier : %s%f\r\n"
	        "%sQ%s)  Exit Bash Editor\r\n",
			BashData.O_OBMultiplier, BashData.O_BMIMultiplier, BashData.O_BMIOffset, BashData.O_WeaponWeightMultiplier,
	        BashData.D_BaseBonus, BashData.D_DBMultiplier,
	        cyn, nrm, cyn, BashData.O_OBMultiplier,
	        cyn, nrm, cyn, BashData.O_BMIMultiplier,
			cyn, nrm, cyn, BashData.O_BMIOffset,
	        cyn, nrm, cyn, BashData.O_WeaponWeightMultiplier,
	        cyn, nrm, cyn, BashData.D_BaseBonus,
	        cyn, nrm, cyn, BashData.D_DBMultiplier,
			cyn, nrm, cyn, BashData.WeightDifferenceMultiplier,
			cyn, nrm, cyn, BashData.LevelDifferenceMultiplier,
			cyn, nrm, cyn, BashData.WeaponDifferenceMultiplier,
	        cyn, nrm

	       );
	OLC_MODE(d) = CEDIT_BASH;
}

void CeditMeleeMenu(Descriptor *d)
{
	get_char_cols(d->character);
	//clear_screen(d);

	d->send("\r\n"
	        "%s1%s)  Ward Multiplier            : %s%.2f\r\n"
	        "%s2%s)  Parry Split                : %s%.2f\r\n"
	        "%s3%s)  Dodge Multiplier           : %s%.2f\r\n"
			"%s4%s)  Parry Multiplier           : %s%.2f\r\n"
	        "%s5%s)  Pounds To Subtract Offense : %s%d\r\n"
	        "%s6%s)  Pounds To Subtract Dodge   : %s%d\r\n"
	        "%s7%s)  Pounds To Subtract Parry   : %s%d\r\n"
	        "%sQ%s)  Return to Skills Menu\r\n",
	        grn, nrm, cyn, MeleeData.WardMultiplier,
	        grn, nrm, cyn, MeleeData.ParrySplit,
	        grn, nrm, cyn, MeleeData.DodgeMultiplier,
			grn, nrm, cyn, MeleeData.ParryMultiplier,
	        grn, nrm, cyn, MeleeData.PoundsPerOffense,
	        grn, nrm, cyn, MeleeData.PoundsPerDodge,
	        grn, nrm, cyn, MeleeData.PoundsPerParry,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_MELEE;
}

void CeditPrecStrikeMenu(Descriptor *d)
{
	get_char_cols(d->character);
	
	d->send("\r\n"
			"%s1%s)  Offensive Bonus Multiplier : %s%f\r\n"
			"%s2%s)  Offensive Roll Factor      : %s%f\r\n"
			"%s3%s)  Weapon Weight Multiplier   : %s%f\r\n"
			"%s4%s)  Base Defensive Bonus       : %s%d\r\n"
			"%s5%s)  Bashed Defense Multiplier  : %s%f\r\n"
			"%s6%s)  Dodge Bonus Multiplier     : %s%f\r\n"
			"%s7%s)  Parry Bonus Multiplier     : %s%f\r\n"
	        "%sQ%s)  Return to Skills Menu\r\n",
			grn, nrm, cyn, PreciseStrikeData.O_OBMultiplier,
			grn, nrm, cyn, PreciseStrikeData.O_OffenseRollFactor,
			grn, nrm, cyn, PreciseStrikeData.O_WeightMultiplier,
			grn, nrm, cyn, PreciseStrikeData.D_BaseDB,
			grn, nrm, cyn, PreciseStrikeData.D_BashedMultiplier,
			grn, nrm, cyn, PreciseStrikeData.D_DBMultiplier,
			grn, nrm, cyn, PreciseStrikeData.D_PBMultiplier,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_PRECSTRIKE;
}
void CeditAuctionMenu(Descriptor *d)
{
	get_char_cols(d->character);

	std::list< Auction * > AList = AuctionManager::GetManager().GetListOfAuctions();

	d->send("The auctions are listed below:\r\n");

	if( !AList.empty() )
	{
		unsigned int i = 1;
		for( std::list< Auction * >::iterator aIter = AList.begin(); aIter != AList.end();++aIter, ++i )
		{
			d->send("%s%5d%s) %s%s%s\r\n", grn, (*aIter)->getVnum(), nrm, cyn, (*aIter)->getName().c_str(), nrm);
		}
	}
	else
		d->send(" <No Auctions Exist> \r\n");
	d->send("\r\n");
	d->send("%sA%s) Add Auction\r\n", grn, nrm);
	d->send("%sE%s) Edit Auction\r\n", grn, nrm);
	d->send("%sD%s) Delete Auction\r\n", grn, nrm);
	d->send("%sQ%s) Quit\r\n", grn, nrm);

	OLC_MODE(d) = CEDIT_AUCTION_MENU;
}

void CeditAuctionEditMenu(Descriptor *d)
{
	get_char_cols(d->character);

	d->send("\r\n");
	d->send("Auction Vnum: %s%d%s\r\n", cyn, d->olc->auction->getVnum(), nrm);
	d->send("%sN%s)ame   : %s%s%s\r\n", grn, nrm, cyn, d->olc->auction->getName().c_str(), nrm);
	d->send("%sR%s)aces  : %s%s%s\r\n", grn, nrm, cyn, d->olc->auction->aRacesStr(d->character).c_str(), nrm);
	d->send("%sC%s)lans  : %s%s%s\r\n", grn, nrm, cyn, d->olc->auction->dClansStr(d->character).c_str(), nrm);
	d->send("%sQ%s)uit", grn, nrm);
	OLC_MODE(d) = CEDIT_AUCTION_EDIT_MENU;
}
void CeditAuctionRaces(Descriptor *d)
{
	get_char_cols(d->character);

	for(unsigned int i = 0;i < NUM_RACES;++i)
		d->send("%s%d%s) %s%s%s\r\n", grn, (i+1), nrm, cyn, pc_race_types[i], nrm);
	d->send("%sQ%s)uit\r\n", grn, nrm);
	d->send("\r\n");
	d->send("Allowed Clans: %s", d->olc->auction->aRacesStr(d->character).c_str());
	OLC_MODE(d) = CEDIT_AUCTION_EDIT_RACES;
}
void CeditAuctionClans(Descriptor *d)
{
	get_char_cols(d->character);
	d->send("\r\n");
	for(Clan *c = ClanList;c;c = c->Next)
		d->send("%s%d%s) %s%s%s\r\n", grn, c->vnum, nrm, cyn, c->Name.c_str(), nrm);
	d->send("\r\n");
	d->send("Allowed Races: %s", d->olc->auction->dClansStr(d->character).c_str());
	d->send("%sQ%s)uit\r\n\r\n", grn, nrm);
	OLC_MODE(d) = CEDIT_AUCTION_EDIT_CLANS;
}
void CeditGlobalScriptMenu(Descriptor *d)
{
	get_char_cols(d->character);
	std::stringstream sBuffer;
	
	for(unsigned int i = 0;i < globalJS_Scripts->size();++i)
	{
		JSTrigger *t = globalJS_Scripts->at(i);
		sBuffer
			<< yel << std::setw(4) << std::right << (i+1) << nrm
			<< ") [" << grn << bld << std::setw(7) << std::right << t->vnum << nrm << "] "
			<< cyn << bld << t->name << nrm << std::endl;
	}
	sBuffer << std::endl;

	sBuffer << bld << yel << "N" << nrm << ") New Global Trigger" << std::endl;
	sBuffer << bld << yel << "D" << nrm << ") Delete Global Trigger" << std::endl;
	sBuffer << bld << yel << "Q" << nrm << ") Save and Quit" << std::endl;

	d->sendRaw( sBuffer.str().c_str() );

	OLC_MODE(d) = CEDIT_GLOBAL_SCRIPT_MENU;
}

void CeditGameOptionsMenu(Descriptor *d)
{
	get_char_cols(d->character);
	std::string ResetString, RestatString;
	//clear_screen(d);

	ResetString = OLC_CONFIG(d)->play.reset_time.toString();
	ResetString.resize(ResetString.size() - 1);
	RestatString = OLC_CONFIG(d)->play.restat_time.toString();
	RestatString.resize(RestatString.size() - 1);

	d->send("\r\n\r\n"
	        "%sA%s) OK Message Text               : %s%s\r\n"
	        "%sB%s) NOPERSON Message Text         : %s%s\r\n"
	        "%sC%s) NOEFFECT Message Text         : %s%s\r\n"
	        "%sD%s) Minimum Level To Shout        : %s%d\r\n"
	        "%sE%s) Tunnel Size                   : %s%d\r\n"
	        "%sF%s) Maximum Experience Gain       : %s%d\r\n"
	        "%sG%s) Max Time for Player Corpse    : %s%d\r\n"
	        "%sH%s) Max Time for MOB Corpse       : %s%d\r\n"
	        "%sI%s) Tics before PC sent to void   : %s%d\r\n"
	        "%sJ%s) Tics before PC is idle rented : %s%d\r\n"
	        "%sK%s) Objects Load Into Inventory   : %s%s\r\n"
	        "%sL%s) Max Hit EXP(level exp / num)  : %s%d\r\n"
			"%sM%s) Hit EXP Multiplier            : %s%3.1f\r\n"
	        "%sN%s) Exp Multiplier                : %s%3.1f\r\n"
			"%sO%s) Weave Point Gain Multiplier   : %s%3.1f\r\n"
			"%sP%s) Weave Point Loss Multiplier   : %s%3.1f\r\n"
	        "%sR%s) Restat Date                   : %s%s\r\n"
	        "%sS%s) Reset Date                    : %s%s\r\n"
	        "%sQ%s) Exit To The Main Menu\r\n"
	        "Enter your choice : ",
	        grn, nrm, cyn, OLC_CONFIG(d)->play.OK,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.NOPERSON,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.NOEFFECT,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.level_can_shout,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.tunnel_size,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.max_exp_gain,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.max_pc_corpse_time,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.max_npc_corpse_time,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.idle_void,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.idle_rent_time,
	        grn, nrm, cyn, CHECK_VAR(OLC_CONFIG(d)->play.load_into_inventory),
	        grn, nrm, cyn, OLC_CONFIG(d)->play.max_hit_exp,
			grn, nrm, cyn, OLC_CONFIG(d)->play.hit_exp_mult,
	        grn, nrm, cyn, OLC_CONFIG(d)->play.ExpMultiplier,
			grn, nrm, cyn, OLC_CONFIG(d)->play.WpGainMultiplier,
			grn, nrm, cyn, OLC_CONFIG(d)->play.WpLossMultiplier,
	        grn, nrm, cyn, RestatString.c_str(),
	        grn, nrm, cyn, ResetString.c_str(),
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_GAME_OPTIONS_MENU;
}

/*-------------------------------------------------------------------*/

void cedit_disp_crash_save_options(Descriptor *d)
{
	get_char_cols(d->character);
	////clear_screen(d);

	d->send("\r\n\r\n"
	        "%sA%s) Free Rent          : %s%s\r\n"
	        "%sB%s) Max Objects Saved  : %s%d\r\n"
	        "%sC%s) Auto Save          : %s%s\r\n"
	        "%sD%s) Auto Save Time     : %s%d minute(s)\r\n"
	        "%sQ%s) Exit To The Main Menu\r\n"
	        "Enter your choice : ",
	        grn, nrm, cyn, CHECK_VAR(OLC_CONFIG(d)->csd.free_rent),
	        grn, nrm, cyn, OLC_CONFIG(d)->csd.max_obj_save,
	        grn, nrm, cyn, CHECK_VAR(OLC_CONFIG(d)->csd.auto_save),
	        grn, nrm, cyn, OLC_CONFIG(d)->csd.autosave_time,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_CRASHSAVE_OPTIONS_MENU;
}

/*-------------------------------------------------------------------*/

void cedit_disp_room_numbers(Descriptor *d)
{
	get_char_cols(d->character);
	////clear_screen(d);

	d->send("\r\n\r\n"
	        "%sA%s) Human Start Room     :	%s%d\r\n"
	        "%sB%s) Trolloc Start Room   :	%s%d\r\n"
	        "%sC%s) Aiel Start Room      :  %s%d\r\n"
	        "%sD%s) Seanchan Start Room  :  %s%d\r\n"
	        "%sE%s) Immortal Start Room	 :	%s%d\r\n"
	        "%sD%s) Frozen Start Room    :	%s%d\r\n"
	        "%sQ%s) Exit To The Main Menu\r\n"
	        "Enter your choice : ",
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.human_start.vnum,
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.trolloc_start.vnum,
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.aiel_start.vnum,
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.seanchan_start.vnum,
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.immortal_start.vnum,
	        grn, nrm, cyn, OLC_CONFIG(d)->room_nums.frozen_start.vnum,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_ROOM_NUMBERS_MENU;
}

/*-------------------------------------------------------------------*/

void cedit_disp_operation_options(Descriptor *d)
{
	get_char_cols(d->character);
	////clear_screen(d);

	d->send("\r\n\r\n"
	        "%sA%s) Logfile Name        : %s%s\r\n"
	        "%sB%s) Max Players         : %s%d\r\n"
	        "%sC%s) Max Bad Pws         : %s%d\r\n"
	        "%sD%s) Name Server Is Slow : %s%s\r\n"
	        "%sE%s) Use new socials file: %s%s\r\n"
	        "%sF%s) OLC autosave to disk: %s%s\r\n"
	        "%sG%s) Welcome Message     : \r\n%s%s\r\n"
	        "%sH%s) Start Message       : \r\n%s%s\r\n"
			"%sN%s) Newbie Message      : \r\n%s%s\r\n"
	        "%sT%s) Newbie Tips\r\n"
	        "%sQ%s) Exit To The Main Menu\r\n"
	        "Enter your choice : ",
	        grn, nrm, cyn, OLC_CONFIG(d)->operation.LOGNAME ? OLC_CONFIG(d)->operation.LOGNAME : "<None>",
	        grn, nrm, cyn, OLC_CONFIG(d)->operation.max_playing,
	        grn, nrm, cyn, OLC_CONFIG(d)->operation.max_bad_pws,
	        grn, nrm, cyn, StringUtil::allUpper(StringUtil::yesNo(OLC_CONFIG(d)->operation.nameserver_is_slow)).c_str(),
	        grn, nrm, cyn, StringUtil::allUpper(StringUtil::yesNo(OLC_CONFIG(d)->operation.use_new_socials)).c_str(),
	        grn, nrm, cyn, StringUtil::allUpper(StringUtil::yesNo(OLC_CONFIG(d)->operation.auto_save_olc)).c_str(),
	        grn, nrm, cyn, OLC_CONFIG(d)->operation.WELC_MESSG ? OLC_CONFIG(d)->operation.WELC_MESSG : "<None>",
	        grn, nrm, cyn, OLC_CONFIG(d)->operation.START_MESSG ? OLC_CONFIG(d)->operation.START_MESSG : "<None>",
			grn, nrm, cyn, d->olc->config->operation.NEWBIE_MSG ? d->olc->config->operation.NEWBIE_MSG : "<None>",
			grn, nrm,
	        grn, nrm
	       );

	OLC_MODE(d) = CEDIT_OPERATION_OPTIONS_MENU;
}

/*-------------------------------------------------------------------*/

std::string GetTip(int n)
{
	std::list<std::string>::iterator iter;

	if(n <= 0 || (unsigned int)n > Tips->size())
		return "";

	for(iter = Tips->begin();iter != Tips->end() && n>1;++iter, --n)
		;

	if(iter == Tips->end())
		return "";
	return (*iter);
}

/**************************************************************************
  The GARGANTAUN event handler
 **************************************************************************/

void cedit_parse(Descriptor *d, char *arg)
{
	//	char *oldtext = NULL;
	delete_doubledollar(arg);
	Weave *weave;
	switch (OLC_MODE(d))
	{
		case CEDIT_CONFIRM_SAVESTRING:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					cedit_save_internally(d);
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE,
					       "OLC: %s has modified the game configuration.", GET_NAME(d->character));
					cleanup_olc(d, CLEANUP_CONFIG);

					if (CONFIG_AUTO_SAVE)
					{
						Conf->save();
						WeaveManager::GetManager().saveWeaves();
						d->send("Game configuration saved to disk.\r\n");
					}
					else
						d->send("Game configuration saved to memory.\r\n");
					return;
				case 'n':
				case 'N':
					d->send("Game configuration not saved to memory.\r\n");
					cleanup_olc(d, CLEANUP_CONFIG);
					return;
				default :
					d->send("\r\nThat is an invalid choice!\r\n");
					d->send("Do you wish to save the configuration? (y/n) : ");
					return;
			}

			/*-------------------------------------------------------------------*/

		case CEDIT_MAIN_MENU:
			switch (toupper(*arg))
			{
				case 'A':
					if( GET_LEVEL(d->character) < LVL_GRGOD )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditAuctionMenu(d);
					OLC_MODE(d) = CEDIT_AUCTION_MENU;
					break;
				case 'G':
					if( GET_LEVEL(d->character) < LVL_GRGOD  && !PLR_FLAGGED(d->character, PLR_CHARGE_EDITOR))
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditGameOptionsMenu(d);
					OLC_MODE(d) = CEDIT_GAME_OPTIONS_MENU;
					break;
				case 'J':
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character, PLR_GLOBAL_SCRIPTS) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditGlobalScriptMenu(d);
					OLC_MODE(d) = CEDIT_GLOBAL_SCRIPT_MENU;
					break;
				case 'C':
					if( GET_LEVEL(d->character) < LVL_GRGOD )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					cedit_disp_crash_save_options(d);
					OLC_MODE(d) = CEDIT_CRASHSAVE_OPTIONS_MENU;
					break;
				case 'R':
					if( GET_LEVEL(d->character) < LVL_GRGOD )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					cedit_disp_room_numbers(d);
					OLC_MODE(d) = CEDIT_ROOM_NUMBERS_MENU;
					break;
				case 'O':
					if( GET_LEVEL(d->character) < LVL_GRGOD )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					cedit_disp_operation_options(d);
					OLC_MODE(d) = CEDIT_OPERATION_OPTIONS_MENU;
					break;
				case 'S':
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character, PLR_WEAVE_EDITOR) &&
						!PLR_FLAGGED(d->character, PLR_CHARGE_EDITOR))
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditSkillMenu(d);
					OLC_MODE(d) = CEDIT_SKILLS_MENU;
					break;
				case 'T':
					if( GET_LEVEL(d->character) < LVL_GRGOD )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditTimeMenu(d);
					OLC_MODE(d) = CEDIT_TIME_MENU;
					break;
				case 'Q':
					d->send("Do you wish to save the configuration? (y/n) : ");
					OLC_MODE(d) = CEDIT_CONFIRM_SAVESTRING;
					break;
				default:
					d->send("That is an invalid choice!\r\n");
					cedit_disp_menu(d);
					break;
			}
			break;

			/*-------------------------------------------------------------------*/
		case CEDIT_AUCTION_MENU:
			if( toupper( *arg ) == 'A' )
			{
				d->send("Enter the vnum of the auction you wish to add: ");
				OLC_MODE(d) = CEDIT_AUCTION_ADD;
			}
			else if( toupper( *arg ) == 'E' )
			{
				d->send("Enter the vnum of the auction you wish to edit: ");
				OLC_MODE(d) = CEDIT_AUCTION_EDIT;
			}
			else if( toupper( *arg ) == 'D' )
			{
				if( AuctionManager::GetManager().NumAuctions() == 0 )
					d->send("There are no auctions to delete.\r\n");
				else
				{
					d->send("Enter the vnum of the auction you wish to delete: ");
					OLC_MODE(d) = CEDIT_AUCTION_DELETE;
				}
			}
			else if( toupper( *arg ) == 'Q' )
			{
				AuctionManager::GetManager().save();
				cedit_disp_menu( d );
			}
			break;
		case CEDIT_AUCTION_ADD:
			if( !MiscUtil::isNumber( arg ) )
			{
				d->send("Your input must be numeric.\r\nTry again: ");
				return;
			}
			if( AuctionManager::GetManager().GetAuction( atoi(arg) ) != NULL )
			{
				d->send("An auction with that vnum already exists. Try editing it.\r\n");
				CeditAuctionMenu(d);
				return;
			}
			d->olc->auction = new Auction( atoi(arg), "" );
			CeditAuctionEditMenu(d);
			break;
		case CEDIT_AUCTION_EDIT:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->send("Your input must be numeric.\r\nTry again: ");
				return;
			}
			Auction *a = AuctionManager::GetManager().GetAuction( atoi(arg) );
			if( (a == NULL) )
			{
				d->send("No auction with that vnum exists. Try adding it.\r\n");
				CeditAuctionMenu(d);
				return;
			}
			d->olc->auction = (a);
			CeditAuctionEditMenu(d);
			break;
		}
		case CEDIT_AUCTION_DELETE:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->send("Your input must be numeric.\r\nTry again: ");
				return;
			}
			Auction *a = AuctionManager::GetManager().GetAuction( atoi(arg) );
			if( a == NULL )
			{
				d->send("That auction does not exist.");
				CeditAuctionMenu(d);
			}
			AuctionManager::GetManager().RemoveAuction( a );
			break;
		}
		case CEDIT_AUCTION_EDIT_MENU:
			switch( toupper( *arg ) )
			{
			case 'N':
				d->send("Enter a new name for the auction: ");
				OLC_MODE(d) = CEDIT_AUCTION_EDIT_NAME;
				break;
			case 'C':
				CeditAuctionClans( d );
				break;
			case 'R':
				CeditAuctionRaces( d );
				break;
			case 'Q':
				if( AuctionManager::GetManager().GetAuction( d->olc->auction->getVnum() ) == NULL )
				{//Auction does not exist in the manager. Let's add it.
					AuctionManager::GetManager().AddAuction( d->olc->auction );
				}
				CeditAuctionMenu( d );
				break;
			}
			break;
		case CEDIT_AUCTION_EDIT_NAME:
			d->olc->auction->setName( arg );
			CeditAuctionEditMenu( d );
			break;
		case CEDIT_AUCTION_EDIT_RACES:
		{
			if( toupper( *arg ) == 'Q' )
			{
				CeditAuctionEditMenu( d );
				return;
			}
			int race = atoi(arg)-1;
			if( race < 0 || race >= NUM_RACES )
			{
				d->send("Invalid race.\r\nTry again: ");
				return;
			}
			if( d->olc->auction->raceIsAllowed(race) )
				d->olc->auction->disallowRace( race );
			else
				d->olc->auction->allowRace( race );
			CeditAuctionRaces(d);
			break;
		}
		case CEDIT_AUCTION_EDIT_CLANS:
		{
			if( toupper( *arg ) == 'Q' )
			{
				CeditAuctionEditMenu( d );
				return;
			}
			Clan *clan = ClanUtil::getClan( atoi(arg) );
			if( clan == NULL )
			{
				d->send("That clan does not exist.\r\nTry again: ");
				return;
			}
			if( d->olc->auction->clanIsAllowed( clan->vnum ) )
				d->olc->auction->disallowClan( clan->vnum );
			else
				d->olc->auction->allowClan( clan->vnum );
			CeditAuctionClans(d);
			break;
		}
			/*-------------------------------------------------------------------*/

		case CEDIT_GLOBAL_SCRIPT_ADD_VNUM:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->send("Input must be numeric.\r\nTry again: ");
				return;
			}
			if( toupper(*arg) == 'Q' )
			{
				CeditGlobalScriptMenu(d);
				return;
			}
			if( !JSManager::get()->triggerExists( atoi(arg) ) )
			{
				d->send("There is no JSTrigger with that vnum.\r\nTry again, or enter 'Q' to quit out: ");
				return;
			}
			if( atoi(arg) < 0 ) {
				d->send("You must specify a trigger, not a library function.\r\n");
				return;
			}
			JSTrigger *t = JSManager::get()->getTrigger( atoi(arg) );
			//It's a valid index. Let's remove it.
			globalJS_Scripts->push_back( t );
			CeditGlobalScriptMenu(d);
			break;
		}
			/*-------------------------------------------------------------------*/
		case CEDIT_GLOBAL_SCRIPT_REMOVE_VNUM:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->send("Input must be numeric.\r\nTry again: ");
				return;
			}
			int pos = atoi(arg)-1;
			if( pos < 0 || pos >= globalJS_Scripts->size() )
			{
				d->send("You must enter a number from the ordered list(NOT the vnum).\r\nTry again: ");
				return;
			}
			//It's a valid index. Let's remove it.
			globalJS_Scripts->erase( globalJS_Scripts->begin()+pos );
			CeditGlobalScriptMenu(d);
			break;
		}
			/*-------------------------------------------------------------------*/
		case CEDIT_GLOBAL_SCRIPT_MENU:
		{
			if( toupper(*arg) == 'N' )
			{
				d->send("Enter the vnum of the script you wish to add: ");
				OLC_MODE(d) = CEDIT_GLOBAL_SCRIPT_ADD_VNUM;
			}
			else if( toupper(*arg) == 'D' )
			{
				if( globalJS_Scripts->size() == 0 ) {
					d->send("There are no global scripts to remove!\r\n");
					return;
				}
				d->send("Which script, from the list, do you wish to remove?");
				OLC_MODE(d) = CEDIT_GLOBAL_SCRIPT_REMOVE_VNUM;
			}
			else if( toupper(*arg) == 'Q' )
			{
				SaveGlobalScripts();
				cedit_disp_menu(d);
			}
			else {
				d->send("Invalid option.\r\nTry again: ");
			}
			break;
		}
			/*-------------------------------------------------------------------*/
		case CEDIT_GAME_OPTIONS_MENU:
			switch (toupper(*arg))
			{
				case 'A':
					d->send("Enter the OK message : ");
					//OLC_MODE(d) = CEDIT_OK;

					if (OLC_CONFIG(d)->play.OK)
					{
						d->send(OLC_CONFIG(d)->play.OK);
						d->backstr = str_dup(OLC_CONFIG(d)->play.OK);
					}

					d->str = &OLC_CONFIG(d)->play.OK;
					d->max_str = 500;

					return;

				case 'B':
					d->send("Enter the NOPERSON message : ");
					//OLC_MODE(d) = CEDIT_NOPERSON;

					if (OLC_CONFIG(d)->play.NOPERSON)
					{
						d->send(OLC_CONFIG(d)->play.NOPERSON);
						d->backstr = str_dup(OLC_CONFIG(d)->play.NOPERSON);
					}

					d->str = &OLC_CONFIG(d)->play.NOPERSON;
					d->max_str = 500;

					return;

				case 'C':
					d->send("Enter the NOEFFECT message : ");
					//OLC_MODE(d) = CEDIT_NOEFFECT;

					if (OLC_CONFIG(d)->play.NOEFFECT)
					{
						d->send(OLC_CONFIG(d)->play.NOEFFECT);
						d->backstr = str_dup(OLC_CONFIG(d)->play.NOEFFECT);
					}

					d->str = &OLC_CONFIG(d)->play.NOEFFECT;
					d->max_str = 500;

					return;
				case 'D':
					d->send("Enter the required level for a player to shout : ");
					OLC_MODE(d) = CEDIT_LEVEL_SHOUT;
					return;
				case 'E':
					d->send("Enter the maximum number of players in a tunnel room : ");
					OLC_MODE(d) = CEDIT_TUNNEL_SIZE;
					return;
				case 'F':
					d->send("Enter the maximum gain of experience per kill for players : ");
					OLC_MODE(d) = CEDIT_MAX_EXP_GAIN;
					return;
				case 'G':
					d->send("Enter the number of tics before PC corpses decompose : ");
					OLC_MODE(d) = CEDIT_MAX_PC_CORPSE_TIME;
					return;
				case 'H':
					d->send("Enter the number of tics before NPC corpses decompose : ");
					OLC_MODE(d) = CEDIT_MAX_NPC_CORPSE_TIME;
					return;
				case 'I':
					d->send("Enter the number of tics before PC's are sent to the void (idle) : ");
					OLC_MODE(d) = CEDIT_IDLE_VOID;
					return;
				case 'J':
					d->send("Enter the number of tics before PC's are automatically rented and forced to quit : ");
					OLC_MODE(d) = CEDIT_IDLE_RENT_TIME;
					return;
				case 'K':
					TOGGLE_VAR(OLC_CONFIG(d)->play.load_into_inventory);
					break;
				case 'L':
					d->send("Enter the new max hit exp gain(level EXP divided by your input) : ");
					OLC_MODE(d) = CEDIT_MAX_HIT_EXP;
					return;
				case 'M':
					d->send("Enter the new hit exp multiplier : ");
					OLC_MODE(d) = CEDIT_HIT_EXP_MULT;
					return;
				case 'N':
					d->send("Enter the new EXP Multipler: ");
					OLC_MODE(d) = CEDIT_EXP_MULTIPLIER;
					return;
				case 'O':
					d->send("Enter the new WP Gain Multiplier: ");
					OLC_MODE(d) = CEDIT_WPGAIN_MULTIPLIER;
					return;
				case 'P':
					d->send("Enter the new WP Loss Multiplier: ");
					OLC_MODE(d) = CEDIT_WPLOSS_MULTIPLIER;
					return;
				case 'R':
					d->send("Enter the date to set for global restat(MONTH-DAY-YEAR): ");
					OLC_MODE(d) = CEDIT_RESTAT;
					return;
				case 'S':
					d->send("Enter the date to set for global reset(MONTH-DAY-YEAR): ");
					OLC_MODE(d) = CEDIT_RESET;
					return;
				case 'Q':
					cedit_disp_menu(d);
					return;
				default:
					d->send("\r\nThat is an invalid choice!\r\n");
					break;
			}

			CeditGameOptionsMenu(d);
			return;

			/*-------------------------------------------------------------------*/

		case CEDIT_CRASHSAVE_OPTIONS_MENU:
			switch (*arg)
			{
				case 'a':
				case 'A':
					TOGGLE_VAR(OLC_CONFIG(d)->csd.free_rent);
					break;
				case 'b':
				case 'B':
					d->send("Enter the maximum number of items players can rent : ");
					OLC_MODE(d) = CEDIT_MAX_OBJ_SAVE;
					return;
				case 'c':
				case 'C':
					TOGGLE_VAR(OLC_CONFIG(d)->csd.auto_save);
					break;
				case 'd':
				case 'D':
					d->send("Enter how often (in minutes) should the MUD save players : ");
					OLC_MODE(d) = CEDIT_AUTOSAVE_TIME;
					return;
				case 'q':
				case 'Q':
					cedit_disp_menu(d);
					return;
				default:
					d->send("\r\nThat is an invalid choice!\r\n");
					break;
			}

			cedit_disp_crash_save_options(d);
			return;

			/*-------------------------------------------------------------------*/

		case CEDIT_ROOM_NUMBERS_MENU:
			switch (toupper(*arg))
			{
				case 'A':
					d->send("Enter the room's vnum where humans should load into : ");
					OLC_MODE(d) = CEDIT_HUMAN_START_ROOM;
					return;
				case 'B':
					d->send("Enter the room's vnum where trollocs should load into : ");
					OLC_MODE(d) = CEDIT_TROLLOC_START_ROOM;
					return;
				case 'C':
					d->send("Enter the room's vnum where aiel should load into : ");
					OLC_MODE(d) = CEDIT_AIEL_START_ROOM;
					return;
				case 'D':
					d->send("Enter the room's vnum where seanchan should load into : ");
					OLC_MODE(d) = CEDIT_SEANCHAN_START_ROOM;
					return;
				case 'E':
					d->send("Enter the room's vnum where immortals should load into : ");
					OLC_MODE(d) = CEDIT_IMMORT_START_ROOM;
					return;
				case 'F':
					d->send("Enter the room's vnum where frozen people should load into : ");
					OLC_MODE(d) = CEDIT_FROZEN_START_ROOM;
					return;
				case 'Q':
					cedit_disp_menu(d);
					return;
				default:
					d->send("\r\nThat is an invalid choice!\r\n");
			}

			cedit_disp_room_numbers(d);
			return;
		/*-------------------------------------------------------------------*/
		case CEDIT_OPERATION_OPTIONS_MENU:
			switch (toupper(*arg))
			{
				case 'A':
					d->send("Enter the name of the logfile : ");
					OLC_MODE(d) = CEDIT_LOGNAME;
					return;
				case 'B':
					d->send("Enter the maximum number of players : ");
					OLC_MODE(d) = CEDIT_MAX_PLAYING;
					return;
				case 'C':
					d->send("Enter the maximum number of password attempts : ");
					OLC_MODE(d) = CEDIT_MAX_BAD_PWS;
					return;
				case 'D':
					TOGGLE_VAR(OLC_CONFIG(d)->operation.use_new_socials);
					d->send( "Please note that using the stock social file will disable AEDIT.\r\n");
					break;
				case 'E':
					TOGGLE_VAR(OLC_CONFIG(d)->operation.auto_save_olc);
					break;
				case 'F':
					OLC_MODE(d) = CEDIT_WELC_MESSG;

					d->send("Enter the new welcome message :\r\n\r\n");

					if (OLC_CONFIG(d)->operation.WELC_MESSG)
					{
						d->send(OLC_CONFIG(d)->operation.WELC_MESSG);
						d->backstr = str_dup(OLC_CONFIG(d)->operation.WELC_MESSG);
					}

					d->str = &OLC_CONFIG(d)->operation.WELC_MESSG;
					d->max_str = 500;
					return;
				case 'G':
					//OLC_MODE(d) = CEDIT_START_MESSG;
					d->send("Enter the new newbie start message :\r\n\r\n");

					if (OLC_CONFIG(d)->operation.START_MESSG)
					{
						d->send(OLC_CONFIG(d)->operation.START_MESSG);
						d->backstr = str_dup(OLC_CONFIG(d)->operation.START_MESSG);
					}

					d->str = &OLC_CONFIG(d)->operation.START_MESSG;
					d->max_str = 500;
					return;
				case 'n':
				case 'N':
					//OLC_MODE(d) = CEDIT_START_MESSG;
					d->send("Enter the new newbie start message :\r\n\r\n");

					if (OLC_CONFIG(d)->operation.NEWBIE_MSG)
					{
						d->send(OLC_CONFIG(d)->operation.NEWBIE_MSG);
						d->backstr = str_dup(OLC_CONFIG(d)->operation.NEWBIE_MSG);
					}

					d->str = &OLC_CONFIG(d)->operation.NEWBIE_MSG;
					d->max_str = 1024;
					return;
				case 't':
				case 'T':
					CeditTipMenu(d);
					return;
				case 'q':
				case 'Q':
					cedit_disp_menu(d);
					return;
				default:
					d->send("\r\nThat is an invalid choice!\r\n");
			}
			cedit_disp_operation_options(d);
			return;

		case CEDIT_SKILLS_MENU:
			switch(atoi(arg))
			{
				case 1:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditBashMenu(d);
					return;
				case 2:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditChargeMenu(d);
					return;
				case 3:
					//BACKSTAB
					return;
				case 4:
					//RIDE
					return;
				case 5:
					//COVER TRACKS
					return;
				case 6:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditMeleeMenu(d);
					return;
				case 7:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditBodyPercentsMenu(d);
					return;
				case 8:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditTaintMenu(d);
					return;
				case 9:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character,PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditFleeMenu(d);
					return;
				case 10:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character, PLR_WEAVE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditWeaveMenu(d);
					return;
				case 11:
					if( GET_LEVEL(d->character) < LVL_GRGOD && !PLR_FLAGGED(d->character, PLR_CHARGE_EDITOR) )
					{
						d->send("You can't edit that!\r\n");
						return;
					}
					CeditPrecStrikeMenu(d);
					return;
				default:
					if(toupper(*arg) == 'Q')
						cedit_disp_menu(d);
					else
						d->send("Invalid option. Try again: ");
					break;
			}
			return;
		case CEDIT_WEAVE_MENU:
			if( toupper(arg[0]) == 'Q' )
			{
				CeditSkillMenu(d);
				return;
			}
			if( toupper(arg[0]) == 'A' )
			{
				d->send("Enter a vnum for the new skill: ");
				OLC_MODE(d) = CEDIT_ADD_WEAVE_VNUM;
				return;
			}
			if( toupper(arg[0]) == 'D' )
			{
				d->send("Enter the vnum of the skill you wish to delete: ");
				OLC_MODE(d) = CEDIT_DELETE_WEAVE;
				return;
			}
			if( !(d->olc->weave = WeaveManager::GetManager().GetWeave( atoi(arg) )) )
				d->send("Weave not found. Try again: ");
			else
				CeditDisplayWeave(d);
			break;
		case CEDIT_ADD_WEAVE_VNUM:
			if( !MiscUtil::isNumber(arg) ) {
				d->send("Your input must be numeric.\r\nTry again: ");
				return;
			} else if( atoi(arg) < 0 ) {
				d->send("Your input must not be negative.\r\nTry again: ");
				return;
			} else if( WeaveManager::GetManager().GetWeave( atoi(arg) ) != NULL ) {
				d->send("A weave with that vnum already exists.\r\nTry again: ");
				return;
			} else {
				d->olc->weave = new Weave();
				d->olc->weave->setAttribute("Vnum", MiscUtil::convert<std::string>(MiscUtil::convert<int>(arg)));
				d->send("Enter a name for this skill: ");
				OLC_MODE(d) = CEDIT_ADD_WEAVE_NAME;
			}
			break;
		case CEDIT_ADD_WEAVE_NAME:
			if( WeaveManager::GetManager().GetWeave(arg) != (NULL) ) {
				d->send("A skill with that name already exists.\r\nPlease enter a name for this skill: ");
				return;
			} else {
				d->olc->weave->setName( arg );
				WeaveManager::GetManager().AddWeave( d->olc->weave );
				d->olc->weave = (NULL);
				d->send("Weave added.\r\n");
				WeaveManager::GetManager().saveWeaves();
				CeditWeaveMenu(d);
			}
			break;
		case CEDIT_DELETE_WEAVE:
			if( !MiscUtil::isNumber( arg ) ) {
				d->send("You must enter a numerical argument.\r\nTry again: ");
				return;
			} else if( (weave = WeaveManager::GetManager().GetWeave(atoi(arg))) == NULL ) {
				d->send("No weave by that vnum.");
				CeditWeaveMenu(d);
			} else {
				WeaveManager::GetManager().RemoveWeave( atoi(arg) );
				WeaveManager::GetManager().saveWeaves();
				d->send("Weave removed.\r\n");
				CeditWeaveMenu(d);
			}
			break;
		case CEDIT_WEAVE_VIEW:
		{
			if( toupper( arg[0] ) == 'Q' )
			{
				CeditWeaveMenu(d);
				return;
			}
			if( toupper( arg[0] ) == 'A' )
			{
				d->send("Enter a name for the attribute: ");
				OLC_MODE(d) = CEDIT_WEAVE_ADD_ATTRIBUTE;
				return;
			}
			if( toupper( arg[0] ) == 'D' )
			{
				d->send("Enter the name of the attribute you wish to delete: ");
				OLC_MODE(d) = CEDIT_WEAVE_DELETE_ATTRIBUTE;
				return;
			}
			//Find the matching attribute.
			std::pair<std::string, std::string> Attribute = d->olc->weave->GetNthAttribute( atoi(arg) );
			if( !Attribute.first.compare("Invalid") )
			{
				d->send("That attribute isn't valid.\r\nTry again: ");
				return;
			}
			d->olc->WeaveAttribute = Attribute.first;
			d->olc->mode = CEDIT_WEAVE_ATTR_EDIT;
			d->send("Enter a new value for this weave's attribute: ");
			break;
		}
		case CEDIT_WEAVE_ADD_ATTRIBUTE:
			if( d->olc->weave->hasAttribute( arg, false ) )
			{
				d->send("That attribute already exists.\r\nTry another name:");
				return;
			}
			d->olc->weave->addAttribute( arg );
			d->send("Attribute has been added.\r\n");
			CeditDisplayWeave( d );
			break;
		case CEDIT_WEAVE_DELETE_ATTRIBUTE:
			if( d->olc->weave->hasAttribute( arg, false ) )
			{
				d->olc->weave->removeAttribute( arg, false );
				d->send("Attribute has been removed.\r\n");
			}
			else
				d->send("Attribute not found.\r\n");
			CeditDisplayWeave(d);
			break;
		case CEDIT_WEAVE_ATTR_EDIT:
			d->olc->weave->setAttribute( d->olc->WeaveAttribute, arg );
			d->olc->WeaveAttribute.clear();
			CeditDisplayWeave(d);
			break;
		case CEDIT_FLEE:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_FLEE_UNENGAGED;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_FLEE_ENGAGED;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_FLEE_AUTO;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_FLEE_REGAIN;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_FLEE_BASE;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_FLEE_MAX_LAG;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_FLEE_EXITS;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again(Q to quit): ");
			}
			d->send("Input new value : ");
			return;
		case CEDIT_FLEE_UNENGAGED:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.PulsesPerUnengagedFlee = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_ENGAGED:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.PulsesPerEngagedFlee = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_AUTO:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.PulsesPerAutoFlee = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_REGAIN:
			if(atoi(arg) <= 0)
			{
				d->send("Value must be greater than zero. Try again : ");
				return;
			}
			FleeData.PulsesRemovedPerTic = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_BASE:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.PulsesForFirstFlee = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_MAX_LAG:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.MaxFleeLag = atoi(arg);
			CeditFleeMenu(d);
			return;
		case CEDIT_FLEE_EXITS:
			if(atoi(arg) < 0)
			{
				d->send("Value must not be negative. Try again : ");
				return;
			}
			FleeData.PulsesPerExit = atoi(arg);
			CeditFleeMenu(d);
			return;
			/************ TAINT VARIABLES ***************/
		case CEDIT_TAINT:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_TAINT_RANDOM_COMMAND;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_TAINT_RANDOM_CHANNEL;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_TAINT_RANDOM_SOCIAL;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_TAINT_RANDOM_VOICE;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_TAINT_RANDOM_TARGET;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_TAINT_STRAIN_TO_TAINT;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_TAINT_SEIZE_COST;
					break;
				case 8:
					OLC_MODE(d) = CEDIT_TAINT_HUNGER_THIRST_COST;
					break;
				case 9:
					OLC_MODE(d) = CEDIT_TAINT_LEVEL_STRAIN_REMOVAL;
					break;
				case 10:
					OLC_MODE(d) = CEDIT_TAINT_WIS_STRAIN_REMOVAL;
					break;
				case 11:
					OLC_MODE(d) = CEDIT_TAINT_BLACK_TOWER_BONUS;
					break;
				case 12:
					OLC_MODE(d) = CEDIT_TAINT_TIMER_COST;
					break;
				case 13:
					OLC_MODE(d) = CEDIT_TAINT_FAILURE_COST;
					break;
				case 14:
					OLC_MODE(d) = CEDIT_TAINT_MANA_COST;
					break;
				case 15:
					OLC_MODE(d) = CEDIT_TAINT_SEIZE_DIZZY;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Insert the new value : ");
			return;
		case CEDIT_TAINT_RANDOM_COMMAND:
			TaintData.TaintForRandomCommand = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_RANDOM_CHANNEL:
			TaintData.TaintForRandomChannel = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_RANDOM_SOCIAL:
			TaintData.TaintForRandomSocial = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_RANDOM_VOICE:
			TaintData.TaintForRandomVoice = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_RANDOM_TARGET:
			TaintData.TaintForRandomTarget = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_STRAIN_TO_TAINT:
			TaintData.StrainToTaint = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_SEIZE_COST:
			TaintData.SeizeCost = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_HUNGER_THIRST_COST:
			TaintData.HungerThirstCost = atoi(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_LEVEL_STRAIN_REMOVAL:
			TaintData.LevelStrainRemoval = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_WIS_STRAIN_REMOVAL:
			TaintData.WisStrainRemoval = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_BLACK_TOWER_BONUS:
			TaintData.BlackTowerBonus = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_TIMER_COST:
			TaintData.TimerCost = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_FAILURE_COST:
			TaintData.FailureCost = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_MANA_COST:
			TaintData.ManaCost = atof(arg);
			CeditTaintMenu(d);
			return;
		case CEDIT_TAINT_SEIZE_DIZZY:
			TaintData.TaintForSeizeDizziness = int(atof(arg));
			CeditTaintMenu(d);
			return;
			/************ MELEE VARIABLES ***************/
		case CEDIT_MELEE:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_MELEE_WARDMULTIPLIER;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_MELEE_PARRYSPLIT;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_MELEE_DODGEMULTIPLIER;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_MELEE_PARRYMULTIPLIER;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_MELEE_P_PER_O;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_MELEE_P_PER_D;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_MELEE_P_PER_P;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Input new value : ");
			return;

		case CEDIT_MELEE_WARDMULTIPLIER:
			MeleeData.WardMultiplier = atof(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_PARRYSPLIT:
			MeleeData.ParrySplit = atof(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_DODGEMULTIPLIER:
			MeleeData.DodgeMultiplier = atof(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_PARRYMULTIPLIER:
			MeleeData.ParryMultiplier = atof(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_P_PER_O:
			MeleeData.PoundsPerOffense = atoi(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_P_PER_D:
			MeleeData.PoundsPerDodge = atoi(arg);
			CeditMeleeMenu(d);
			return;
		case CEDIT_MELEE_P_PER_P:
			MeleeData.PoundsPerParry = atoi(arg);
			CeditMeleeMenu(d);
			return;

			/********* PRECISE STRIKE VARIABLES *********/
		case CEDIT_PRECSTRIKE:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_OBMULT;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_OFACTOR;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_WEIGHTMULT;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_BASEDB;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_BASHEDMULT;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_DBMULT;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_PRECSTRIKE_PBMULT;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Input new value : ");
			return;
		case CEDIT_PRECSTRIKE_OBMULT:
			PreciseStrikeData.O_OBMultiplier = atof(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_OFACTOR:
			PreciseStrikeData.O_OffenseRollFactor = atof(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_WEIGHTMULT:
			PreciseStrikeData.O_WeightMultiplier = atof(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_BASEDB:
			PreciseStrikeData.D_BaseDB = atoi(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_BASHEDMULT:
			PreciseStrikeData.D_BashedMultiplier = atof(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_DBMULT:
			PreciseStrikeData.D_DBMultiplier = atof(arg);
			CeditPrecStrikeMenu(d);
			return;
		case CEDIT_PRECSTRIKE_PBMULT:
			PreciseStrikeData.D_PBMultiplier = atof(arg);
			CeditPrecStrikeMenu(d);
			return;

			/********* BODY HIT PERCENTAGES *************/
		case CEDIT_BODY_PERCENTS:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_HEAD;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_SHOULDERS;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_ARMS;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_HANDS;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_BODY;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_WAIST;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_LEGS;
					break;
				case 8:
					OLC_MODE(d) = CEDIT_BODY_PERCENT_FEET;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Input the new percent to represent how often this body part is hit : ");
			return;
		case CEDIT_BODY_PERCENT_HEAD:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_HEAD] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_SHOULDERS:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_SHOULDERS] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_ARMS:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_ARMS] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_HANDS:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_HANDS] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_BODY:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_BODY] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_WAIST:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_WAIST] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_LEGS:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_LEGS] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
		case CEDIT_BODY_PERCENT_FEET:
			if(atoi(arg) < 0 || atoi(arg) > 100)
			{
				d->send("The percentage must be between 0 and 100.\r\nTry again : ");
				return;
			}
			BodyPercents[WEAR_FEET] = atoi(arg);
			CeditBodyPercentsMenu(d);
			return;
			/************** BASH VARIABLES *****************/

		case CEDIT_BASH:
			switch(atoi(arg))
			{
				case 1:
					OLC_MODE(d) = CEDIT_BASH_OMULTIPLIER;
					break;
				case 2:
					OLC_MODE(d) = CEDIT_BASH_OBMIMULTIPLIER;
					break;
				case 3:
					OLC_MODE(d) = CEDIT_BASH_OBMIOFFSET;
					break;
				case 4:
					OLC_MODE(d) = CEDIT_BASH_OWEAPONMULTIPLIER;
					break;
				case 5:
					OLC_MODE(d) = CEDIT_BASH_DBASEBONUS;
					break;
				case 6:
					OLC_MODE(d) = CEDIT_BASH_DDBMULTIPLIER;
					break;
				case 7:
					OLC_MODE(d) = CEDIT_BASH_WEIGHT_MULTIPLIER;
					break;
				case 8:
					OLC_MODE(d) = CEDIT_BASH_LEVEL_MULTIPLIER;
					break;
				case 9:
					OLC_MODE(d) = CEDIT_BASH_WEAPON_MULTIPLIER;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Enter the new value: ");
			return;
		case CEDIT_CHARGE:
			switch(toupper(*arg))
			{
				case '1':
					OLC_MODE(d) = CEDIT_CHARGE_OMULTIPLIER;
					break;
				case '2':
					OLC_MODE(d) = CEDIT_CHARGE_OBMIMULTIPLIER;
					break;
				case '3':
					OLC_MODE(d) = CEDIT_CHARGE_OWEAPONMALUS;
					break;
				case '4':
					OLC_MODE(d) = CEDIT_CHARGE_ONEGATIVESWAY;
					break;
				case '5':
					OLC_MODE(d) = CEDIT_CHARGE_OPOSITIVESWAY;
					break;
				case '6':
					OLC_MODE(d) = CEDIT_CHARGE_DBASEBONUS;
					break;
				case '7':
					OLC_MODE(d) = CEDIT_CHARGE_DDBMULTIPLIER;
					break;
				case '8':
					OLC_MODE(d) = CEDIT_CHARGE_TIMER;
					break;
				case '9':
					OLC_MODE(d) = CEDIT_CHARGE_CHARGE_ENGAGED_VICT;
					break;
				case 'A':
					OLC_MODE(d) = CEDIT_CHARGE_FINISH_WHILE_ENGAGED;
					break;
				case 'B':
					OLC_MODE(d) = CEDIT_CHARGE_START_WHILE_ENGAGED;
					break;
				case 'C':
					OLC_MODE(d) = CEDIT_CHARGE_DPULSES_LAG_LAND;
					break;
				case 'D':
					OLC_MODE(d) = CEDIT_CHARGE_OPULSES_LAG_LAND;
					break;
				case 'E':
					OLC_MODE(d) = CEDIT_CHARGE_OPULSES_LAG_MISS;
					break;
				default:
					if(toupper(*arg) == 'Q')
					{
						CeditSkillMenu(d);
						return;
					}
					d->send("Invalid option. Try again : ");
					return;
			}
			d->send("Enter the new value: ");
			return;
		case CEDIT_BASH_OMULTIPLIER:
			BashData.O_OBMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_OBMIMULTIPLIER:
			BashData.O_BMIMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_OBMIOFFSET:
			BashData.O_BMIOffset = atoi(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_OWEAPONMULTIPLIER:
			BashData.O_WeaponWeightMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_DBASEBONUS:
			BashData.D_BaseBonus = atoi(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_DDBMULTIPLIER:
			BashData.D_DBMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_DBMIMULTIPLIER:
			BashData.D_BMIMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_WEIGHT_MULTIPLIER:
			BashData.WeightDifferenceMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_LEVEL_MULTIPLIER:
			BashData.LevelDifferenceMultiplier = atof(arg);
			CeditBashMenu(d);
			return;
		case CEDIT_BASH_WEAPON_MULTIPLIER:
			BashData.WeaponDifferenceMultiplier = atof(arg);
			CeditBashMenu(d);
			return;

			/*********** CHARGE MENU **************/
		case CEDIT_CHARGE_OMULTIPLIER:
			ChargeData.O_OBMultiplier = (double)atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_OBMIMULTIPLIER:
			ChargeData.O_BMIMultiplier = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_OWEAPONMALUS:
			ChargeData.O_WeaponWeightMalus = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_ONEGATIVESWAY:
			ChargeData.O_NegativeSway = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_OPOSITIVESWAY:
			ChargeData.O_PositiveSway = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_DBASEBONUS:
			ChargeData.D_BaseBonus = atoi(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_DDBMULTIPLIER:
			ChargeData.D_DBMultiplier = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_TIMER:
			ChargeData.Timer = atof(arg);
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_CHARGE_ENGAGED_VICT:
			if( toupper(*arg) == 'Y' )
				ChargeData.CanStartOnFightingVict = true;
			else if( toupper(*arg) == 'N' )
				ChargeData.CanStartOnFightingVict = false;
			else
			{
				d->send("Invalid option. Please either choose 'Y' or 'N': ");
				return;
			}
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_FINISH_WHILE_ENGAGED:
			if( toupper(*arg) == 'Y' )
				ChargeData.CanStartWhileFighting = true;
			else if( toupper(*arg) == 'N' )
				ChargeData.CanStartWhileFighting = false;
			else
			{
				d->send("Invalid option. Please either choose 'Y' or 'N': ");
				return;
			}
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_START_WHILE_ENGAGED:
			if( toupper(*arg) == 'Y' )
				ChargeData.CanFinishWhileFighting = true;
			else if( toupper(*arg) == 'N' )
				ChargeData.CanFinishWhileFighting = false;
			else
			{
				d->send("Invalid option. Please either choose 'Y' or 'N': ");
				return;
			}
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_DPULSES_LAG_LAND:
			ChargeData.D_PulsesLagOnLand = MAX(0,atoi(arg));
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_OPULSES_LAG_LAND:
			ChargeData.O_PulsesLagOnLand = MAX(0,atoi(arg));
			CeditChargeMenu(d);
			return;
		case CEDIT_CHARGE_OPULSES_LAG_MISS:
			ChargeData.O_PulsesLagOnMiss = MAX(0,atoi(arg));
			CeditChargeMenu(d);
			return;

			/********** NEWBIE TIP MENU ***********/
		case CEDIT_TIP_MENU:
			switch(toupper(*arg))
			{
				case 'N':
					d->send("Enter the new tip: ");
					OLC_MODE(d) = CEDIT_TIP_NEW;
					return;
				case 'D':
					if(!Tips->size())
					{
						d->send("There are no tips to delete!\r\n");
					}
					else
					{
						d->send("Enter the tip number to have deleted: ");
						OLC_MODE(d) = CEDIT_TIP_DELETE;
					}
					return;
				case 'Q':
					cedit_disp_operation_options(d);
					return;
				default:
					d->send("Invalid option. Try again: ");
					return;
			}
		case CEDIT_TIP_NEW:
			if(!*arg)
			{
				d->send("No tip added.\r\n");
				CeditTipMenu(d);
				return;
			}
			Tips->push_back(arg);
			CeditTipMenu(d);
			return;
		case CEDIT_TIP_DELETE:
			if(!GetTip(atoi(arg)).size())
			{
				d->send("That tip does not exist. Try again(Q to quit): ");
				return;
			}
			if(toupper(*arg) == 'Q')
			{
				CeditTipMenu(d);
				return;
			}
			Tips->remove(GetTip(atoi(arg)));
			CeditTipMenu(d);
			return;
			/*************************************************/
		case CEDIT_TUNNEL_SIZE:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the maximum number of people allowed in a tunnel : ");
			}
			else
			{
				OLC_CONFIG(d)->play.tunnel_size = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_EXP_GAIN:
			if (*arg)
				OLC_CONFIG(d)->play.max_exp_gain = atoi(arg);

			CeditGameOptionsMenu(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_EXP_MULTIPLIER:
			if(!*arg || !MiscUtil::isNumber(arg))
			{
				d->send("Must be numerical. Try again : ");
				return;
			}
			OLC_CONFIG(d)->play.ExpMultiplier = atof(arg);
			CeditGameOptionsMenu(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_WPGAIN_MULTIPLIER:
			if(!*arg || !MiscUtil::isNumber(arg))
			{
				d->send("Must be numerical. Try again : ");
				return;
			}
			OLC_CONFIG(d)->play.WpGainMultiplier = atof(arg);
			CeditGameOptionsMenu(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_WPLOSS_MULTIPLIER:
			if(!*arg || !MiscUtil::isNumber(arg))
			{
				d->send("Must be numerical. Try again : ");
				return;
			}
			OLC_CONFIG(d)->play.WpLossMultiplier = atof(arg);
			CeditGameOptionsMenu(d);
			break;

			/*------------------------------------------------------------------*/

		case CEDIT_RESTAT:
			if(toupper(*arg) == 'Q')
			{
				CeditGameOptionsMenu(d);
				break;
			}
			if(Time::DateToTime(arg) <= 0)
			{
				d->send("The date must in the following format: MONTH-DAY-YEAR (Q to quit)\r\nTry again");
				return;
			}
			d->olc->config->play.restat_time.setTime(Time::DateToTime(arg));
			CeditGameOptionsMenu(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_RESET:
			if(toupper(*arg) == 'Q')
			{
				CeditGameOptionsMenu(d);
				break;
			}
			if(Time::DateToTime(arg) <= 0)
			{
				d->send("The date must in the following format: MONTH-DAY-YEAR (Q to quit)\r\nTry again");
				return;
			}
			d->olc->config->play.reset_time = Time::DateToTime(arg);
			CeditGameOptionsMenu(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_NPC_CORPSE_TIME:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the number of tics before NPC corpses decompose : ");
			}
			else
			{
				OLC_CONFIG(d)->play.max_npc_corpse_time = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_PC_CORPSE_TIME:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the number of tics before PC corpses decompose : ");
			}
			else
			{
				OLC_CONFIG(d)->play.max_pc_corpse_time = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_IDLE_VOID:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the number of tics before PC's are sent to the void (idle) : ");
			}
			else
			{
				OLC_CONFIG(d)->play.idle_void = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_HIT_EXP:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the new max hit exp(level exp divided by your input): ");
			}
			else if(atoi(arg) <= 0)
			{
				d->send("Input MUST be above zero.\r\nTry again : ");
			}
			else
			{
				OLC_CONFIG(d)->play.max_hit_exp = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_HIT_EXP_MULT:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the new hit exp multiplier: ");
			}
			else if(atoi(arg) <= 0)
			{
				d->send("Input MUST be above zero.\r\nTry again : ");
			}
			else
			{
				OLC_CONFIG(d)->play.hit_exp_mult = atof(arg);
				CeditGameOptionsMenu(d);
			}
			break;

		case CEDIT_IDLE_RENT_TIME:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the number of tics before PC's are automatically rented and forced to quit : ");
			}
			else
			{
				OLC_CONFIG(d)->play.idle_rent_time = atoi(arg);
				CeditGameOptionsMenu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_OK:
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_NOPERSON:
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_NOEFFECT:
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_OBJ_SAVE:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the maximum objects a player can save : ");
			}
			else
			{
				OLC_CONFIG(d)->csd.max_obj_save = atoi(arg);
				cedit_disp_crash_save_options(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_AUTOSAVE_TIME:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the interval for player's being autosaved : ");
			}
			else
			{
				OLC_CONFIG(d)->csd.autosave_time = atoi(arg);
				cedit_disp_crash_save_options(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_IMMORT_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where immortals should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where immortals should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.immortal_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_AIEL_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where aiel should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where aiel should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.aiel_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_SEANCHAN_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where seanchan should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where seanchan should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.seanchan_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_TROLLOC_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where trollocs should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where trollocs should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.trolloc_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_HUMAN_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where humans should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where humans should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.human_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_FROZEN_START_ROOM:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the room's vnum where frozen people should load into : ");
			}
			else if (real_room(atoi(arg)) == NOWHERE)
			{
				d->send("That room doesn't exist!\r\n"
				        "Enter the room's vnum where frozen people should load into : ");
			}
			else
			{
				OLC_CONFIG(d)->room_nums.frozen_start.vnum = atoi(arg);
				cedit_disp_room_numbers(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_LOGNAME:
			if (!*arg)
			{
				d->send("That is an invalid choice!\r\n"
				        "Enter the name of the logfile : ");
			}
			else
			{
				OLC_CONFIG(d)->operation.LOGNAME = str_dup(arg);
				cedit_disp_operation_options(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_PLAYING:
			OLC_CONFIG(d)->operation.max_playing = atoi(arg);
			cedit_disp_operation_options(d);
			break;

			/*-------------------------------------------------------------------*/

		case CEDIT_MAX_BAD_PWS:
			OLC_CONFIG(d)->operation.max_bad_pws = atoi(arg);
			cedit_disp_operation_options(d);
			break;

			/*-------------------------------------------------------------------*/

		default:
			/*
			 * We should never get here, but just in case...
			 */
			cleanup_olc(d, CLEANUP_CONFIG);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: cedit_parse(): Reached default case!");
			d->send("Oops...\r\n");
			break;
	}
}

/*
 * End of parse_cedit()
 */
void reassign_rooms(void)
{
	void assign_rooms(void);
	unsigned int i;

	/* remove old funcs */
	for (i = 0; i < World.size(); ++i)
		World[i]->func = NULL;

	/* reassign spec_procs */
	assign_rooms();
}

void cedit_string_cleanup(Descriptor *d)
{
	switch (OLC_MODE(d))
	{
		case CEDIT_MENU:
		case CEDIT_WELC_MESSG:
		case CEDIT_START_MESSG:
			cedit_disp_operation_options(d);
			break;
	}
}
