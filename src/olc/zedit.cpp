/************************************************************************
 *  OasisOLC - zedit.c						v1.5	*
 *									*
 *  Copyright 1996 Harvey Gilpin.					*
 ************************************************************************/

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../comm.h"
#include "../utils.h"
#include "../db.h"
#include "olc.h"
#include "../shop.h"
#include "../interpreter.h"
#include "../screen.h"
#include "../weather.h"
#include "../mobs.h"
#include "../zones.h"
#include "../Descriptor.h"
#include "../rooms/Room.h"

/*
 * Turn on zedit debugging.  Please mail log results to greerga@van.ml.org
 * This will attempt to find the problem with replastd::cing other zedit commands
 * when you add unrelated ones.
 */
#if 0
const int DEBUG = 1;
#endif

/*-------------------------------------------------------------------*/

//External data structures.
extern struct Index *obj_index;
extern const char *equipment_types[];
extern const char *dirs[];
const char *door_state(int state);

extern Character *character_list;

/*-------------------------------------------------------------------*/

/*
 * Function prototypes.
 */
void zedit_disp_menu(Descriptor *d);
void zedit_setup(Descriptor *d, int room_num);
int start_change_command(Descriptor *d, unsigned int pos);
void zedit_disp_comtype(Descriptor *d);
void zedit_disp_arg1(Descriptor *d);
void zedit_disp_arg2(Descriptor *d);
void zedit_disp_arg3(Descriptor *d);
void zedit_disp_arg4(Descriptor *d);
void zedit_disp_arg5(Descriptor *d);
void zedit_disp_arg6(Descriptor *d);
void zedit_disp_arg7(Descriptor *d);
void sedit_save_to_disk();
void zedit_save_internally(Descriptor *d);
void zedit_new_zone(Character *ch, int vzone_num);

/*-------------------------------------------------------------------*/

/*
 * Nasty internal macros to clean up the code.
 */
#define MYCMD		(OLC_ZONE(d)->cmd[subcmd])
#define OLC_CMD(d)	(OLC_ZONE(d)->cmd[OLC_VAL(d)])

/*-------------------------------------------------------------------*/

/*
 * Utility functions.
 */

/*-------------------------------------------------------------------*/
void zedit_setup(Descriptor *d, int room_num)
{
	Zone *zone;
	int count = 0, cmd_room = -1;
	unsigned int subcmd = 0;

	// Allocate one scratch zone structure.
	OLC_ZONE(d) = new Zone();
	OLC_ZONE(d)->CopyFrom( (zone = ZoneManager::GetManager().GetZoneByRnum( OLC_ZNUM(d) )) );
	OLC_ZONE(d)->SetAge(0);

	// Add all entries in zone_table that relate to this room.
	while(subcmd < zone->cmd.size())
	{
		switch (zone->cmd[subcmd]->command)
		{
			case 'M':
			case 'O':
				cmd_room = zone->cmd[subcmd]->arg3;
				break;
			case 'D':
			case 'R':
				cmd_room = zone->cmd[subcmd]->arg1;
				break;
			default:
				break;
		}

		if (cmd_room == room_num)
			V_InsertBefore(OLC_ZONE(d)->cmd, new ResetCommand(zone->cmd[subcmd]), count++);
		++subcmd;
	}
	zedit_disp_menu(d);// Display main menu.
}

/*-------------------------------------------------------------------*/

/*
 * Create a new zone.
 */

void zedit_new_zone(Character *ch, int vzone_num)
{
	int rnum = 0;

	if(vzone_num < 0)
	{
		ch->send("You can't make negative zones.\r\n");
		return;
	}
	else if(vzone_num > 999)
	{
		ch->send("999 is the highest zone allowed.\r\n");
		return;
	}
	else if(ZoneManager::GetManager().GetZoneByVnum(vzone_num) != NULL)
	{
		ch->send("That zone already exists.\r\n");
		return;
	}
	else if(ZoneManager::GetManager().GetZoneByRoomVnum(vzone_num*100) != NULL )
	{
		ch->send("A zone exists that contains the minumum room vnum(%d).\r\n", (vzone_num*100));
		return;
	}

	Zone *zone = ZoneManager::GetManager().AddNewZone( vzone_num );

	zone->setName("New Zone");
	zone->SetBuilders("<None>");

	redit_save_to_disk(rnum);
	MobManager::GetManager().SavePrototypes(zone->getVnum());
	oedit_save_to_disk(rnum);
	sedit_save_to_disk();
	zone->save();

	/*
	 * Previously, creating a new zone while invisible gave you away.
	 * That quirk has been fixed with the MAX() statement.
	 */
	MudLog(BRF, MAX(LVL_BUILDER, GET_INVIS_LEV(ch)), TRUE, "OLC: %s creates new zone #%d", GET_NAME(ch), vzone_num);
	ch->send("Zone created successfully.\r\n");

	return;
}

/*
 * Save all the information in the player's temporary buffer back into
 * the current zone table.
 */
void zedit_save_internally(Descriptor *d)
{
	int cmd_room = -2, room_num = real_room(OLC_NUM(d));
	unsigned int subcmd = 0;
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum( OLC_ZNUM(d) );

	if( zone == NULL )
	{
		MudLog(LVL_APPR, BRF, TRUE, "zedit_save_internally : Could not find zone #%d", OLC_ZNUM(d));
		return;
	}

	/*
	 * Delete all entries in zone_table that relate to this room so we
	 * can add all the ones we have in their place.
	 */
	while (subcmd < zone->cmd.size())
	{
		switch (zone->cmd[subcmd]->command)
		{
			case 'M':
			case 'O':
				cmd_room = zone->cmd[subcmd]->arg3;
				break;
			case 'D':
			case 'R':
				cmd_room = zone->cmd[subcmd]->arg1;
				break;
			default:
				break;
		}
		if (cmd_room == room_num)
		{
			zone->RemoveCommand(zone->cmd[subcmd], true);
		}
		else
			++subcmd;
	}

	/*
	 * Now add all the entries in the player's descriptor list
	 */

	for(unsigned int i = 0;i < OLC_ZONE(d)->cmd.size();++i)
	{
		zone->cmd.push_back(OLC_ZONE(d)->cmd[i]);
	}
	OLC_ZONE(d)->cmd.clear();
	/*
	 * Finally, if zone headers have been changed, copy over
	 */

	int age = OLC_ZONE(d)->GetAge();
	Weather *OldWeather = zone->GetWeather();
	zone->CopyFrom( OLC_ZONE(d) );
	zone->SetAge(age);
	zone->SetWeather(OldWeather);

	olc_add_to_save_list(zone->getVnum(), OLC_SAVE_ZONE);
}

/*-------------------------------------------------------------------*/

/*
 * Save all the zone_table for this zone to disk.  This function now
 * writes simple comments in the form of (<name>) to each record.  A
 * header for each field is also there.
 */

/*-------------------------------------------------------------------*/

/*
 * Some common code to count the number of comands in the list.
 */
int count_commands(ResetCommand *listy)
{
	int count = 0;

	while (listy[count].command != 'S')
		++count;

	return count;
}

/*-------------------------------------------------------------------*/
/*
 * Error check user input and then setup change
 */

int start_change_command(Descriptor *d, unsigned int pos)
{
	if ((pos >= OLC_ZONE(d)->cmd.size()) || (pos < 0))
		return 0;

	/*
	 * Ok, let's get editing
	 */
	OLC_VAL(d) = pos;
	return 1;
}

/**************************************************************************
 Menu functions
 **************************************************************************/
CommandHandler do_zlist = DEFINE_COMMAND
{
	Zone *z = 0;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH], arg4[MAX_INPUT_LENGTH],
		buf2[1024*50], buf3[1024*100];
	int count = 0;
	unsigned int s;
	Room *cmd_room = 0;
	bool zone = false, unique = false, has_seen = false, global = false;
	std::list<int> cmds_seen;
	std::list<int>::iterator iter;

	if( !ch->desc )
		return;

	get_char_cols(ch);
	TwoArguments(TwoArguments(argument, arg1, arg2), arg3, arg4);

	if(!str_cmp(arg2, "unique"))
	{
		strcpy(arg3, arg2);
		strcpy(arg2, "");
	}
	if(!str_cmp(arg3, "unique"))
	{
		unique = true;
		strcpy(arg3, "-1");
	}
	if(!str_cmp(arg1, "zone"))
		zone = true;
	else if (!str_cmp(arg1, "global"))
		global = true;
	else
	{
		strcpy(arg3, arg2);
		strcpy(arg2, arg1);
	}

	memset((char *)buf3, 0, sizeof(buf3));
	memset((char *)buf2, 0, sizeof(buf2));
	memset((char *)buf , 0, sizeof(buf) );
	//Zone commands...
	for( int znr = 0;(z=ZoneManager::GetManager().GetZoneByRnum(znr)) != NULL;++znr)
	{
		if (!global && znr != ch->in_room->getZoneNumber())
			continue;
		ch->send(COLOR_YELLOW(ch, CL_COMPLETE));
		for(s = 0;s < z->cmd.size();++s)
		{
			switch (z->cmd[s]->command)
			{
				case 'M':
				case 'O':
					cmd_room = World[z->cmd[s]->arg3];
					break;
				case 'D':
				case 'R':
					cmd_room = World[z->cmd[s]->arg1];
					break;
				default:
					break;
			}

			if(cmd_room != ch->in_room && !zone && !global)
				continue;
			if(*arg2 && toupper(*arg2) != z->cmd[s]->command)
				continue;

			if(unique && z->cmd[s]->command == 'M')	//Only want unique check for mob types?
			{
				has_seen = false;
				for(iter = cmds_seen.begin();iter != cmds_seen.end();++iter)
				{
					if( (*iter) == z->cmd[s]->arg1)
					{
						has_seen = true;
						break;
					}
				}
				if(has_seen)
					continue;
				cmds_seen.push_back(z->cmd[s]->arg1);
			}
			*buf = '\0';
			switch (z->cmd[s]->command)
			{
			case 'M':
				if(*arg3 && atoi(arg3) != -1 && MobManager::GetManager().RealMobile(atoi(arg3)) != z->cmd[s]->arg1)
					continue;
				sprintf(buf + strlen(buf), "%soad %s [%s%d%s], Chance %d%%, Max: Per Room: %d, Zone: %d, Stacking: %s, Follow: %s",
					z->cmd[s]->if_flag ? " Then l" : "L",
					MobManager::GetManager().GetPrototype(z->cmd[s]->arg1)->player.short_descr,
					cyn, MobManager::GetManager().GetIndex(z->cmd[s]->arg1)->vnum, yel,
					z->cmd[s]->arg2, z->cmd[s]->arg4, z->cmd[s]->arg5,
					z->cmd[s]->arg6 ? "Yes" : "No",
					z->cmd[s]->arg7 == 1 ? "Yes" : "No"
				);
				break;
			case 'G':
				sprintf(buf + strlen(buf), "%sive it %s [%s%d%s], Chance %d%%, Max: In Inv: %d",
					z->cmd[s]->if_flag ? " Then g" : "G",
					obj_proto[z->cmd[s]->arg1]->short_description,
					cyn, obj_index[z->cmd[s]->arg1].vnum, yel,
					z->cmd[s]->arg2, z->cmd[s]->arg3
				);
				break;
			case 'O':
				if(*arg3 && atoi(arg3) != GET_OBJ_VNUM(obj_proto[z->cmd[s]->arg1]) && atoi(arg3) != -1)
					continue;
				sprintf(buf + strlen(buf), "%sLoad %s [%s%d%s], Chance %d%%, Max: Per Room: %d, Zone: %d, Stack: %s",
					z->cmd[s]->if_flag ? " then " : "",
					obj_proto[z->cmd[s]->arg1]->short_description,
					cyn, obj_index[z->cmd[s]->arg1].vnum, yel,
					z->cmd[s]->arg2, z->cmd[s]->arg4, z->cmd[s]->arg5, z->cmd[s]->arg6 ? "Yes" : "No"
			   );
				break;
			case 'E':
				sprintf(buf + strlen(buf), "%sEquip with %s [%s%d%s], %s, Chance %d%%",
					z->cmd[s]->if_flag ? " then " : "",
					obj_proto[z->cmd[s]->arg1]->short_description,
					cyn, obj_index[z->cmd[s]->arg1].vnum, yel,
					equipment_types[z->cmd[s]->arg3], z->cmd[s]->arg2);
				break;
			case 'P':
				sprintf(buf + strlen(buf), "%sPut %s [%s%d%s] in %s [%s%d%s], Chance %d%%",
					z->cmd[s]->if_flag ? " then " : "",
					obj_proto[z->cmd[s]->arg1]->short_description,
					cyn, obj_index[z->cmd[s]->arg1].vnum, yel,
					obj_proto[z->cmd[s]->arg3]->short_description,
					cyn, obj_index[z->cmd[s]->arg3].vnum, yel,
					z->cmd[s]->arg2
				);
				break;
			case 'R':
				sprintf(buf + strlen(buf), "%sRemove %s [%s%d%s] from room.",
					z->cmd[s]->if_flag ? " then " : "",
					obj_proto[z->cmd[s]->arg2]->short_description,
					cyn, obj_index[z->cmd[s]->arg2].vnum, yel
			   );
				break;
			case 'D':
				sprintf(buf + strlen(buf), "%sSet door %s as %s.",
					z->cmd[s]->if_flag ? " then " : "",
					dirs[z->cmd[s]->arg2],
					door_state(z->cmd[s]->arg3)
				);
				break;
			default:
				sprintf(buf + strlen(buf), "<Unknown Command>");
				break;
			}
			sprintf(buf + strlen(buf), " (ID: %d)", z->cmd[s]->dbID);

			if(zone || global)
				sprintf(buf + strlen(buf), ", Room [%s%s%d%s%s]", COLOR_BOLD(ch, CL_COMPLETE), grn, cmd_room->getVnum(), nrm, yel);
			sprintf(buf2, "%d. %s\r\n", ++count, buf);//This is the entire line.

			if( strlen(buf2) + strlen(buf3) > (sizeof(buf3)-1) )
				break;//We're out of room in the buffer!
			strcat(buf3, buf2);
			memset((char *)buf2, 0, sizeof(buf2));
			memset((char *)buf, 0, sizeof(buf));
		}
	}

	//Look through the kits.
	if( toupper(*arg1) == 'K' )
	{
		for( int znr = 0;(z=ZoneManager::GetManager().GetZoneByRnum(znr)) != NULL;++znr)
		{
			for(s = 0;s < z->cmd.size();++s)
			{
				Kit *k = NULL;
				if( z->cmd[ s ]->command != 'M' )
					continue;
				Character *mob = MobManager::GetManager().GetPrototype( z->cmd[s]->arg1 );
				if( mob->IsPurged() || !IS_NPC(mob) || ((k = mob->MobData->primary_kit) == NULL) )
					continue;
				//Look through inventory.
				for(unsigned int i = 0;i < k->KitInventory.size();++i)
				{
					if( k->KitInventory[i].GetItemVnum() == atoi(arg3) )
					{
						sprintf(buf, "%3d. Loading on %-45s [%s%d%s], Chance: %3d%% in Room[%s%s%6d%s%s].\r\n", ++count,
							mob->player.short_descr, cyn, mob->getVnum(), yel,
							k->KitInventory[i].GetPercent(), grn, COLOR_BOLD(ch,CL_COMPLETE),
							z->cmd[s]->GetRealArg3(), nrm, yel);
						if( strlen(buf) + strlen(buf3) < (sizeof(buf3)-1) )
							strcat(buf3, buf);
					}
				}
				//Look through equipment.
				for(unsigned int i = 0;i < k->KitItems.size();++i)
				{
					for(unsigned int pos = 0;pos < k->KitItems[i].size();++pos)
					{
						if( k->KitItems[i].at(pos).GetItemVnum() == atoi(arg3) )
						{
							sprintf(buf, "%3d. Loading on %-45s [%s%d%s], Chance: %3d%% in Room[%s%s%6d%s%s].\r\n", ++count,
								mob->player.short_descr, cyn, mob->getVnum(), yel,
								k->KitItems[i].at(pos).GetPercent(), grn, COLOR_BOLD(ch,CL_COMPLETE),
								z->cmd[s]->GetRealArg3(), nrm, yel);
							if( strlen(buf) + strlen(buf3) < (sizeof(buf3)-1) )
								strcat(buf3, buf);
						}
					}
				}
			}
		}
	}

	page_string(ch->desc, buf3, TRUE);


	if(!count)
		ch->send("%sThere were no commands that met your criteria.\r\n", nrm);
	ch->send(COLOR_NORMAL(ch, CL_COMPLETE));
};

/*
 * the main menu
 */
void zedit_disp_menu(Descriptor *d)
{
	int room, counter = 0;
	unsigned int subcmd = 0;

	get_char_cols(d->character);
	room = real_room(OLC_NUM(d));

	/*
	 * Menu header
	 */
	sprintf(buf,
#if defined(CLEAR_SCREEN)
	        "[H[J"
#endif
	        "Room number: %s%d%s		Room zone: %s%d\r\n"
	        "%sB%s) Builders    : %s%s\r\n"
	        "%sZ%s) Zone name   : %s%s\r\n"
	        "%sS%s) Closed/Open : %s%s\r\n"
	        "%sL%s) Lifespan    : %s%d minutes\r\n"
	        "%sF%s) Sunrise     : %s%s\r\n"
	        "%sG%s) Sunset      : %s%s\r\n"
	        "%sX%s) X Coordinate: %s%d\r\n"
	        "%sY%s) Y Coordinate: %s%d\r\n"
	        "%sT%s) Top of zone : %s%d\r\n"
	        "%sR%s) Reset Mode  : %s%s\r\n"
	        "%sH%s) Delete      : %s%s%s\r\n"
	        "[Command list]\r\n",

	        cyn, OLC_NUM(d), nrm,
	        cyn, OLC_ZONE(d)->getVnum(),
	        grn, nrm, yel, OLC_ZONE(d)->GetBuilders().size() ? OLC_ZONE(d)->GetBuilders().c_str() : "<NONE!>",
	        grn, nrm, yel, OLC_ZONE(d)->getName().size() ? OLC_ZONE(d)->getName().c_str() : "<NONE!>",
	        grn, nrm, yel, OLC_ZONE(d)->IsClosed() ? "Closed" : "Open",
	        grn, nrm, yel, OLC_ZONE(d)->GetLifespan(),
	        grn, nrm, yel, OLC_ZONE(d)->GetSunrise().empty() ? "Default" : OLC_ZONE(d)->GetSunrise().c_str(),
	        grn, nrm, yel, OLC_ZONE(d)->GetSunset().empty() ? "Default" : OLC_ZONE(d)->GetSunset().c_str(),
	        grn, nrm, yel, OLC_ZONE(d)->GetX(),
	        grn, nrm, yel, OLC_ZONE(d)->GetY(),
	        grn, nrm, yel, OLC_ZONE(d)->GetTop(),
	        grn, nrm, yel, OLC_ZONE(d)->GetResetMode() ?
	        ((OLC_ZONE(d)->GetResetMode() == 1) ?
	         "Reset when no players are in zone." :
	         "Normal reset.") :
			        "Never reset",
			        grn, nrm, yel, OLC_ZONE(d)->IsDeleted() ? "Yes" : "No", nrm
			       );

	/*
	 * Print the commands for this room into display buffer.
	 */
	while (subcmd < OLC_ZONE(d)->cmd.size())
{
		/*
		 * Translate what the command means.
		 */

		*buf2 = 0;
		switch (OLC_ZONE(d)->cmd[subcmd]->command)
		{
		case 'M':
		{
			Character *Mob = MobManager::GetManager().GetPrototype(OLC_ZONE(d)->cmd[subcmd]->arg1);
			Index *index = MobManager::GetManager().GetIndex(OLC_ZONE(d)->cmd[subcmd]->arg1);
			sprintf(buf2 + strlen(buf2), "%soad %s [%s%s%s], Chance %d%%, Max: Per Room: %d, Zone: %d, Stacking: %s, Follow: %s",
		        OLC_ZONE(d)->cmd[subcmd]->if_flag ? " Then l" : "L",
				(Mob ? Mob->player.short_descr : "Invalid"), cyn, (index ? itos(index->vnum).c_str() : "-1"), yel,
		        OLC_ZONE(d)->cmd[subcmd]->arg2, OLC_ZONE(d)->cmd[subcmd]->arg4, OLC_ZONE(d)->cmd[subcmd]->arg5,
		        OLC_ZONE(d)->cmd[subcmd]->arg6 ? "Yes" : "No",
		        OLC_ZONE(d)->cmd[subcmd]->arg7 == 1 ? "Yes" : "No"
			);
			break;
		}
		case 'G':
			sprintf(buf2 + strlen(buf2), "%sive it %s [%s%d%s], Chance %d%%, Max: In Inv: %d",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " Then g" : "G",
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg1]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg1].vnum, yel,
				OLC_ZONE(d)->cmd[subcmd]->arg2, OLC_ZONE(d)->cmd[subcmd]->arg3
				);
			break;
		case 'O':
			sprintf(buf2 + strlen(buf2), "%sLoad %s [%s%d%s], Chance %d%%, Max: Per Room: %d, Zone: %d, Stack: %s",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " then " : "",
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg1]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg1].vnum, yel,
				OLC_ZONE(d)->cmd[subcmd]->arg2, OLC_ZONE(d)->cmd[subcmd]->arg4,
				OLC_ZONE(d)->cmd[subcmd]->arg5, OLC_ZONE(d)->cmd[subcmd]->arg6 ? "Yes" : "No"
			);
			break;
		case 'E':
			sprintf(buf2 + strlen(buf2), "%sEquip with %s [%s%d%s], %s, Chance %d%%",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " then " : "",
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg1]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg1].vnum, yel,
				equipment_types[OLC_ZONE(d)->cmd[subcmd]->arg3], OLC_ZONE(d)->cmd[subcmd]->arg2);
			break;
		case 'P':
			sprintf(buf2 + strlen(buf2), "%sPut %s [%s%d%s] in %s [%s%d%s], Chance %d%%",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " then " : "",
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg1]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg1].vnum, yel,
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg3]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg3].vnum, yel,
				OLC_ZONE(d)->cmd[subcmd]->arg2
			);
			break;
		case 'R':
			sprintf(buf2 + strlen(buf2), "%sRemove %s [%s%d%s] from room.",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " then " : "",
				obj_proto[OLC_ZONE(d)->cmd[subcmd]->arg2]->short_description,
				cyn, obj_index[OLC_ZONE(d)->cmd[subcmd]->arg2].vnum, yel
			);
			break;
		case 'D':
			sprintf(buf2 + strlen(buf2), "%sSet door %s as %s.",
				OLC_ZONE(d)->cmd[subcmd]->if_flag ? " then " : "",
				dirs[OLC_ZONE(d)->cmd[subcmd]->arg2],
				door_state(OLC_ZONE(d)->cmd[subcmd]->arg3)
			);
			break;
		default:
			strcpy(buf2 + strlen(buf2), "<Unknown Command>");
			break;
		}
		/*
		 * Build the display buffer for this command
		 */
		std::stringstream bufferStream;
		bufferStream << nrm << counter++ << yel << buf2 << "\r\n";
		strcat(buf, bufferStream.str().c_str());
		*buf2 = '\0';
		++subcmd;
	}
	/*
	 * Finish off menu
	 */
	sprintf(buf1,
	        "%s%d - <END OF LIST>\r\n"
	        "%sN%s) New command.       %sE%s) Edit a command\r\n"
	        "%sD%s) Delete a command.\r\n"
	        "%sQ%s) Quit\r\nEnter your choice : ",
	        nrm, counter, grn, nrm, grn, nrm, grn, nrm, grn, nrm
	       );

	strcat(buf, buf1);
	d->send(buf);

	OLC_MODE(d) = ZEDIT_MAIN_MENU;
}

/*-------------------------------------------------------------------*/

/*
 * Print the command type menu and setup response catch.
 */
void zedit_disp_comtype(Descriptor *d)
{
	get_char_cols(d->character);
	d->send(
#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif
	    "%sM%s) Load Mobile to room             %sO%s) Load Object to room\r\n"
	    "%sE%s) Equip mobile with object        %sG%s) Give an object to a mobile\r\n"
	    "%sP%s) Put object in another object    %sD%s) Open/Close/Lock a Door\r\n"
	    "%sR%s) Remove an object from the room\r\n"
	    "What sort of command will this be? : ",
	    grn, nrm, grn, nrm, grn, nrm, grn, nrm, grn, nrm,
	    grn, nrm, grn, nrm
	);
	OLC_MODE(d) = ZEDIT_COMMAND_TYPE;
}

/*-------------------------------------------------------------------*/

/*
 * Print the appropriate message for the command type for arg1 and set
 * up the input catch clause
 */
void zedit_disp_arg1(Descriptor *d)
{
	switch (OLC_CMD(d)->command)
	{
		case 'M':
			d->send("Input mob's vnum : ");
			OLC_MODE(d) = ZEDIT_ARG1;
			break;
		case 'O':
		case 'E':
		case 'P':
		case 'G':
			d->send("Input object vnum : ");
			OLC_MODE(d) = ZEDIT_ARG1;
			break;
		case 'D':
		case 'R':
			/*
			 * Arg1 for these is the room number, skip to arg2
			 */
			OLC_CMD(d)->arg1 = real_room(OLC_NUM(d));
			zedit_disp_arg2(d);
			break;
		default:
			/*
			 * We should never get here  .
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_disp_arg1(): Help!");
			d->send("Oops...\r\n");
			return;
	}
}

/*-------------------------------------------------------------------*/

/*
 * Print the appropriate message for the command type for arg2 and set
 * up the input catch clause.
 */
void zedit_disp_arg2(Descriptor *d)
{
	int i = 0;

	switch (OLC_CMD(d)->command)
	{

		case 'M':
		case 'O':
		case 'P':
		case 'G':
			d->send("Give the percentage chance that this event should happen: ");
			break;

		case 'E':
			d->send("What chance out of 100 should this have to load? ");
			break;
		case 'D':
			while (*dirs[i] != '\n')
			{
				d->send("%d) Exit %s.\r\n", i, dirs[i]);
				++i;
			}

			d->send("Enter exit number for door : ");
			break;

		case 'R':
			d->send("Input object's vnum : ");
			break;
		default:

			/*
			 * We should never get here, but just in case...
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_disp_arg2(): Help!");
			d->send("Oops...\r\n");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG2;
}

/*-------------------------------------------------------------------*/

/*
 * Print the appropriate message for the command type for arg3 and set
 * up the input catch clause.
 */
void zedit_disp_arg3(Descriptor *d)
{
	int i = 0;

	switch (OLC_CMD(d)->command)
	{
		case 'E':

			while (*equipment_types[i] != '\n')
			{
				d->send("%2d) %26.26s %2d) %26.26s\r\n", i,
				        equipment_types[i], i + 1, (*equipment_types[i + 1] != '\n') ?
				        equipment_types[i + 1] : "");

				if (*equipment_types[i + 1] != '\n')
					i += 2;

				else
					break;

			}

			d->send("Location to equip : ");
			break;

		case 'P':
			d->send("Vnum of the container : ");
			break;

		case 'D':
			d->send(	"0)  Door open\r\n"
			         "1)  Door closed\r\n"
			         "2)  Door locked\r\n"
			         "Enter state of the door : ");
			break;

		case 'G':
			d->send("Input the maximum number that can exist in a mobs inventory : ");
			break;

		case 'M':
		case 'O':
		case 'R':
		default:
			/*
			 * We should never get here, just in case.
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_disp_arg3(): Help!");
			d->send("Oops...\r\n");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG3;
}

/*-------------------------------------------------------------------*/
/*. Print the appropriate message for the command type for arg4 and set
    up the input catch clause .*/

void zedit_disp_arg4(Descriptor *d)
{
	switch(OLC_CMD(d)->command)
	{

		case 'M':
		case 'O':
			d->send("Input the maximum number that can exist in this room : ");
			break;

		case 'G':
		case 'P':
		case 'E':
		case 'D':
		case 'R':
		default:
			/*. We should never get here .*/
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_disp_arg4(): Help!");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG4;
}

void zedit_disp_arg5(Descriptor *d)
{
	switch(OLC_CMD(d)->command)
	{

		case 'M':
		case 'O':
			d->send("Input the maximum number that can exist in this zone : ");
			break;
		case 'G':
		case 'P':
		case 'E':
		case 'D':
		case 'R':
		default:
			/*. We should never get here .*/
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_disp_arg5(): Help!");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG5;
}

void zedit_disp_arg6(Descriptor *d)
{
	switch(OLC_CMD(d)->command)
	{

		case 'M':
			d->send("Should this mob stack, or only load when it's previous no longer exists? : ");
			break;
		case 'O':
			d->send("Should this object stack, or only load when it's previous no longer exists? : ");
			break;
		case 'G':
		case 'P':
		case 'E':
		case 'D':
		case 'R':
		default:
			/*. We should never get here .*/
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_disp_arg6(): Help!");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG6;
}

void zedit_disp_arg7(Descriptor *d)
{
	switch(OLC_CMD(d)->command)
	{

		case 'M':
			d->send("Should this mob follow the previous mob? (y/n) : ");
			break;

		case 'O':
		case 'G':
		case 'P':
		case 'E':
		case 'D':
		case 'R':
		default:
			/*. We should never get here .*/
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_disp_arg7(): Help!");
			return;
	}

	OLC_MODE(d) = ZEDIT_ARG7;
}



/*-------------------------------------------------------------------*/


/**************************************************************************
  The GARGANTAUN event handler
 **************************************************************************/

void zedit_parse(Descriptor *d, char *arg)
{
	int i = 0;
	int pos;

	switch (OLC_MODE(d))
	{
			/*-------------------------------------------------------------------*/

		case ZEDIT_CONFIRM_SAVESTRING:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					/*
					 * Save the zone in memory, hiding invisible people.
					 */
					d->send("Saving zone info in memory.\r\n");
					zedit_save_internally(d);
					AddOlcLog( d->character, "zone", d->olc->zone->getVnum() );
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE, "OLC: %s edits zone info for room %d.",
					       GET_NAME(d->character), OLC_NUM(d));
					/* FALL THROUGH */

				case 'n':
				case 'N':
					cleanup_olc(d, CLEANUP_ALL);
					break;

				default:
					d->send("Invalid choice!\r\n");
					d->send("Do you wish to save the zone info? : ");
					break;

			}

			break;
			/* End of ZEDIT_CONFIRM_SAVESTRING */

			/*-------------------------------------------------------------------*/
		case ZEDIT_MAIN_MENU:
			switch (*arg)
			{
				case 'q':
				case 'Q':

					if (OLC_ZONE(d)->GetAge() || OLC_ZONE(d)->getVnum() || OLC_NUM(d))
					{
						d->send("Do you wish to save the changes to the zone info? (y/n) : ");
						OLC_MODE(d) = ZEDIT_CONFIRM_SAVESTRING;
					}
					else
					{
						d->send("No changes made.\r\n");
						cleanup_olc(d, CLEANUP_ALL);
					}

					break;
				case 'h':
				case 'H':
					if(GET_LEVEL(d->character) < LVL_IMPL)
					{
						d->send("You're too low level to do this.\r\n");
						break;
					}
					if(OLC_ZONE(d)->IsDeleted())
					{
						d->send("This zone will no longer be deleted when the game reboots.\r\n");
						OLC_ZONE(d)->UnsetDeleted();
					}
					else
					{
						d->send("This zone will be deleted when the game reboots.\r\n");
						OLC_ZONE(d)->SetDeleted();
					}
					zedit_disp_menu(d);
					break;
				case 'n':
				case 'N':
					/*
					* New entry.
					*/

					d->send("What number in the list should the new command be? : ");
					OLC_MODE(d) = ZEDIT_NEW_ENTRY;
					break;

				case 'e':
				case 'E':
					/*
					* Change an entry.
					*/
					d->send("Which command do you wish to change? : ");
					OLC_MODE(d) = ZEDIT_CHANGE_ENTRY;
					break;

				case 'd':
				case 'D':
					/*
					* Delete an entry.
					*/
					d->send("Which command do you wish to delete? : ");
					OLC_MODE(d) = ZEDIT_DELETE_ENTRY;
					break;

				case 'z':
				case 'Z':
					/*
					* Edit zone name.
					*/
					d->send("Enter new zone name : ");
					OLC_MODE(d) = ZEDIT_ZONE_NAME;
					break;
				case 's':
				case 'S':
					/* Close / Open the zone */
					OLC_ZONE(d)->SetClosed(OLC_ZONE(d)->IsClosed() ? false : true);
					d->send("The zone is now %s.", OLC_ZONE(d)->IsClosed() ? "closed" : "open");
					zedit_disp_menu(d);
					return;
				case 't':
				case 'T':
					/*
					* Edit top of zone.
					*/

					if (GET_LEVEL(d->character) < LVL_GOD)
						zedit_disp_menu(d);

					else
					{
						d->send("Enter new top of zone : ");
						OLC_MODE(d) = ZEDIT_ZONE_TOP;
					}

					break;
				case 'x':
				case 'X':
					d->send("Enter the X coordinate for this zone.\r\n");
					OLC_MODE(d) = ZEDIT_X;
					break;
				case 'y':
				case 'Y':
					d->send("Enter the Y coordinate for this zone.\r\n");
					OLC_MODE(d) = ZEDIT_Y;
					break;

				case 'l':
				case 'L':
					/*
					* Edit zone lifespan.
					*/
					d->send("Enter new zone lifespan : ");
					OLC_MODE(d) = ZEDIT_ZONE_LIFE;
					break;

				case 'r':
				case 'R':
					/*
					Edit zone reset mode.
					*/
					d->send(	"\r\n"
					         "0) Never reset\r\n"
					         "1) Reset only when no players in zone\r\n"
					         "2) Normal reset\r\n"
					         "Enter new zone reset type : ");
					OLC_MODE(d) = ZEDIT_ZONE_RESET;
					break;

				case 'b':
				case 'B':
					if (GET_LEVEL(d->character) < LVL_GOD)
					{
						d->send("You do not have permission to edit the builders.\r\n");
						zedit_disp_menu(d);
					}

					else
					{
						/*
						Edit zone builder list.
						*/
						d->send("Enter new zone builders : ");
						OLC_MODE(d) = ZEDIT_ZONE_BUILDERS;
					}
					break;
				case 'f':
				case 'F':
					d->send("Enter new sunrise message : ");
					OLC_MODE(d) = ZEDIT_SUNRISE;
					break;
				case 'g':
				case 'G':
					d->send("Enter new sunset message : ");
					OLC_MODE(d) = ZEDIT_SUNSET;
					break;
				default:
					zedit_disp_menu(d);
					break;
			}
			break;
			/* End of ZEDIT_MAIN_MENU */

			/*-------------------------------------------------------------------*/
		case ZEDIT_NEW_ENTRY:
			/*
			 * Get the line number and insert the new line.
			 */
			pos = atoi(arg);
			if (isdigit(*arg) && OLC_ZONE(d)->AddCommand(pos))
			{
				if (start_change_command(d, pos))
				{
					zedit_disp_comtype(d);
					OLC_ZONE(d)->SetAge(1);
				}
			}
			else
				zedit_disp_menu(d);
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_DELETE_ENTRY:
			/*
			 * Get the line number and delete the line.
			 */
			pos = (unsigned int)atoi(arg);
			if (isdigit(*arg))
			{
				d->olc->zone->RemoveCommand(pos, true);
				OLC_ZONE(d)->SetAge(1);
			}
			zedit_disp_menu(d);
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_CHANGE_ENTRY:
			/*
			 * Parse the input for which line to edit, and goto next quiz.
			 */
			pos = atoi(arg);
			if (isdigit(*arg) && start_change_command(d, pos))
			{
				zedit_disp_comtype(d);
				OLC_ZONE(d)->SetAge(1);
			}
			else
				zedit_disp_menu(d);
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_COMMAND_TYPE:
			/*
			 * Parse the input for which type of command this is, and goto next
			 * quiz.
			 */
			OLC_CMD(d)->command = toupper(*arg);
			if (!OLC_CMD(d)->command || (strchr("MOPEDGR", OLC_CMD(d)->command) == NULL))
			{
				d->send("Invalid choice, try again : ");
			}
			else
			{
				if (OLC_VAL(d))
				{	/* If there was a previous command. */
					d->send("Is this command dependent on the success of the previous one? (y/n)\r\n");
					OLC_MODE(d) = ZEDIT_IF_FLAG;
				}

				else
				{	/* 'if-flag' not appropriate. */
					OLC_CMD(d)->if_flag = 0;
					zedit_disp_arg1(d);
				}
			}
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_IF_FLAG:
			/*
			 * Parse the input for the if flag, and goto next quiz.
			 */
			switch (*arg)
			{
				case 'y':
				case 'Y':
					OLC_CMD(d)->if_flag = 1;
					break;
				case 'n':
				case 'N':
					OLC_CMD(d)->if_flag = 0;
					break;
				default:
					d->send("Try again : ");
					return;
			}
			zedit_disp_arg1(d);
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG1:
			/*
			 * Parse the input for arg1, and goto next quiz.
			 */

			if (!isdigit(*arg))
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}
			switch (OLC_CMD(d)->command)
			{
				case 'M':
					if ((pos = MobManager::GetManager().RealMobile(atoi(arg))) >= 0)
					{
						OLC_CMD(d)->arg1 = pos;
						zedit_disp_arg2(d);
					}
					else
						d->send("That mobile does not exist, try again : ");
					break;
				case 'O':
				case 'P':
				case 'E':
				case 'G':
					if ((pos = real_object(atoi(arg))) >= 0)
					{
						OLC_CMD(d)->arg1 = pos;
						zedit_disp_arg2(d);
					}
					else
						d->send("That object does not exist, try again : ");
					break;
				case 'D':
				case 'R':
				default:
					/*
					 * We should never get here.
					 */
					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_parse(): case ARG1: Ack!");
					d->send("Oops...\r\n");
					break;
			}
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG2:
			/*
			 * Parse the input for arg2, and goto next quiz.
			 */

			if (!isdigit(*arg))
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}

			switch (OLC_CMD(d)->command)
			{
				case 'M':
				case 'O':

					if(atoi(arg) < 0 || atoi(arg) > 100)
					{
						d->send("Must be from 0 to 100. Try again : ");
						return;
					}

					OLC_CMD(d)->arg2 = atoi(arg);
					OLC_CMD(d)->arg3 = real_room(OLC_NUM(d));
					zedit_disp_arg4(d);
					break;

				case 'G':

					if(atoi(arg) < 0 || atoi(arg) > 100)
					{
						d->send("Must be from 0 to 100. Try again : ");
						return;
					}

					OLC_CMD(d)->arg2 = atoi(arg);
					zedit_disp_arg3(d);
					break;

				case 'P':

					if(atoi(arg) < 0 || atoi(arg) > 100)
					{
						d->send("Must be from 0 to 100. Try again : ");
						return;
					}
					OLC_CMD(d)->arg2 = atoi(arg);
					OLC_CMD(d)->arg4 = real_room(OLC_NUM(d));
					zedit_disp_arg3(d);
					break;

				case 'E':
					OLC_CMD(d)->arg2 = atoi(arg);
					zedit_disp_arg3(d);
					break;

				case 'D':
					pos = atoi(arg);
					/*
					 * Count directions.
					 */

					while (*dirs[i] != '\n')
						++i;

					if ((pos < 0) || (pos > (unsigned int)i))
						d->send("Try again : ");

					else
					{
						OLC_CMD(d)->arg2 = pos;
						zedit_disp_arg3(d);
					}

					break;

				case 'R':
					if ((pos = real_object(atoi(arg))) >= 0)
					{
						OLC_CMD(d)->arg2 = pos;
						zedit_disp_menu(d);
					}

					else
						d->send("That object does not exist, try again : ");

					break;

				default:
					/*
					 * We should never get here, but just in case...
					 */

					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_parse(): case ARG2: Ack!");
					d->send("Oops...\r\n");
					break;
			}

			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG3:
			/*
			 * Parse the input for arg3, and go back to main menu.
			 */

			if (!isdigit(*arg))
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}

			switch (OLC_CMD(d)->command)
			{

				case 'E':
					pos = atoi(arg);
					/*
					 * Count number of wear positions.  We could use NUM_WEARS, this is
					 * more reliable.
					 */
					while (*equipment_types[i] != '\n')
						++i;

					if ((pos < 0) || (pos > (unsigned int)i))
						d->send("Try again : ");

					else
					{
						OLC_CMD(d)->arg3 = pos;
						zedit_disp_menu(d);
					}

					break;

				case 'P':
					if ((pos = real_object(atoi(arg))) >= 0)
					{
						OLC_CMD(d)->arg3 = pos;
						zedit_disp_menu(d);
					}

					else
						d->send("That object does not exist, try again : ");

					break;

				case 'D':
					pos = atoi(arg);

					if ((pos < 0) || (pos > 3))
						d->send("Try again : ");

					else
					{
						OLC_CMD(d)->arg3 = pos;
						zedit_disp_menu(d);
					}

					break;

				case 'G':
					pos = atoi(arg);

					if (pos < 0)
						d->send("Try again : ");

					else
					{
						OLC_CMD(d)->arg3 = pos;
						zedit_disp_menu(d);
					}

					break;

				case 'M':
				case 'O':
				case 'R':
				default:

					/*
					 * We should never get here, but just in case...
					 */

					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_parse(): case ARG3: Ack!");
					d->send("Oops...\r\n");
					break;
			}

			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG4:
			/*. Parse the input for arg4, and go back to main menu. */
			if (!isdigit(*arg) && (*arg != '-' && !isdigit(*(arg + 1))))
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}

			switch(OLC_CMD(d)->command)
			{
				case 'M':
				case 'O':
				case 'E':
				case 'G':
				case 'P':
					OLC_CMD(d)->arg4 = atoi(arg);
					zedit_disp_arg5(d);
					break;
				case 'D':
				case 'R':
				default:
					/*. We should never get here .*/
					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_parse(): case ARG4: Ack!");
					break;
			}
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG5:
			/*. Parse the input for arg5, and go back to main menu. */
			if (!isdigit(*arg) && (*arg != '-' || !isdigit(*(arg + 1))))
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}

			switch(OLC_CMD(d)->command)
			{
				case 'M':
				case 'P':
				case 'E':
				case 'G':
				case 'O':
					OLC_CMD(d)->arg5 = atoi(arg);
					zedit_disp_arg6(d);
					break;
				case 'D':
				case 'R':
				default:
					/*. We should never get here .*/
					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_parse(): case ARG5: Ack!");
					break;
			}
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG6:
			/*. Parse the input for arg6, and go back to main menu. */
			if (!isdigit(*arg) && OLC_CMD(d)->command != 'M' && OLC_CMD(d)->command != 'O')
			{
				d->send("Must be a numeric value, try again : ");
				return;
			}

			switch(OLC_CMD(d)->command)
			{
				case 'M':
					if(toupper(*arg) == 'Y')
						OLC_CMD(d)->arg6 = 1;
					else if(toupper(*arg) == 'N')
						OLC_CMD(d)->arg6 = 0;
					else
					{
						d->send("You must type 'Y' or 'N' : ");
						return;
					}
					zedit_disp_arg7(d);
					break;
				case 'O':
					if(toupper(*arg) == 'Y')
						OLC_CMD(d)->arg6 = 1;
					else if(toupper(*arg) == 'N')
						OLC_CMD(d)->arg6 = 0;
					else
					{
						d->send("You must type 'Y' or 'N' : ");
						return;
					}
					zedit_disp_menu(d);
					break;
				case 'P':
				case 'E':
				case 'G':
					OLC_CMD(d)->arg6 = atoi(arg);
					zedit_disp_menu(d);
					break;
				case 'D':
				case 'R':
				default:
					/*. We should never get here .*/
					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_parse(): case ARG5: Ack!");
					break;
			}
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ARG7:
			/*. Parse the input for arg7, and go back to main menu. */
			switch(OLC_CMD(d)->command)
			{
				case 'M':
					switch (*arg)
					{
						case 'y':
						case 'Y':
							OLC_CMD(d)->arg7 = 1;
							break;
						case 'n':
						case 'N':
							OLC_CMD(d)->arg7 = -1;
							break;
						default:
							d->send("Try again : ");
							return;
					}
					zedit_disp_menu(d);
					break;
				case 'O':
				case 'P':
				case 'E':
				case 'G':
				case 'D':
				case 'R':
				default:
					/*. We should never get here .*/
					cleanup_olc(d, CLEANUP_ALL);
					MudLog(BRF, LVL_GOD, TRUE, "SYSERR: OLC: zedit_parse(): case ARG5: Ack!");
					break;
			}
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ZONE_NAME:

			// Add new name and return to main menu.
			OLC_ZONE(d)->setName(arg);
			zedit_disp_menu(d);
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_ZONE_BUILDERS:

			// Add new builders and return to main menu.
			OLC_ZONE(d)->SetBuilders(arg);
			zedit_disp_menu(d);
			break;
		case ZEDIT_SUNRISE:
			OLC_ZONE(d)->SetSunrise(arg);
			zedit_disp_menu(d);
			break;
		case ZEDIT_SUNSET:
			OLC_ZONE(d)->SetSunset(arg);
			zedit_disp_menu(d);
			break;
			/*-------------------------------------------------------------------*/
		case ZEDIT_ZONE_RESET:
			/*
			 * Parse and add new reset_mode and return to main menu.
			 */
			pos = atoi(arg);
			if (!isdigit(*arg) || (pos < 0) || (pos > 2))
				d->send("Try again (0-2) : ");
			else
			{
				OLC_ZONE(d)->SetResetMode(pos);
				zedit_disp_menu(d);
			}
			break;

			/*-------------------------------------------------------------------*/

		case ZEDIT_X:
			pos = atoi(arg);
			if(!isdigit(*arg))
				d->send("You must enter a digit.\r\n");
			else
			{
				OLC_ZONE(d)->SetX(pos);
				zedit_disp_menu(d);
			}
			break;

		case ZEDIT_Y:
			pos = atoi(arg);
			if(!isdigit(*arg))
				d->send("You must enter a digit.\r\n");
			else
			{
				OLC_ZONE(d)->SetY(pos);
				zedit_disp_menu(d);
			}
			break;

		case ZEDIT_ZONE_LIFE:
			/*
			 * Parse and add new lifespan and return to main menu.
			 */
			pos = atoi(arg);
			if (!isdigit(*arg) || (pos < 0) || (pos > 240))
				d->send("Try again (0-240) : ");
			else
			{
				OLC_ZONE(d)->SetLifespan(pos);
				zedit_disp_menu(d);
			}
			break;

			/*-------------------------------------------------------------------*/
		case ZEDIT_ZONE_TOP:
			/*
			 * Parse and add new top room in zone and return to main menu.
			 */
			if (OLC_ZNUM(d) == (int)(ZoneManager::GetManager().NumZones()-1))
			{
				OLC_ZONE(d)->SetTop(MIN(OLC_ZONE(d)->GetBottom()+500, MAX(OLC_ZONE(d)->GetBottom(), (unsigned int)atoi(arg))));
			}
			else
				OLC_ZONE(d)->SetTop(MAX(d->olc->zone->GetBottom(),
				MIN(ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d)+1)->GetBottom(), (unsigned int)atoi(arg))));
			zedit_disp_menu(d);
			break;

			/*-------------------------------------------------------------------*/
		default:
			/*
			 * We should never get here, but just in case...
			 */
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: zedit_parse(): Reached default case!");
			d->send("Oops...\r\n", d);
			break;
	}
}

/*
 * End of parse_zedit()
 */
