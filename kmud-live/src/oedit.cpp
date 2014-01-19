/************************************************************************
 * OasisOLC - oedit.c						v1.5	*
 * Copyright 1996 Harvey Gilpin.					*
 * Original author: Levork						*
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
#include "weaves.h"
#include "zones.h"
#include "handler.h"

#include "StringUtil.h"
#include "Descriptor.h"

//External variable declarations.
extern struct Index *obj_index;
extern Object *object_list;
extern Descriptor *descriptor_list;
extern struct attack_hit_type attack_hit_text[];
extern Shop *shop_index;
extern int top_of_objt;
extern int top_shop;
extern const char *item_types[];
extern const char *wear_bits[];
extern const char *extra_bits[];
extern const char *drinks[];
extern const char *apply_types[];
extern const char *container_bits[];
extern const char *weapon_types[];

//Handy macros.
#define S_PRODUCT(s, i) ((s)->producing[(i)])

void oedit_disp_container_flags_menu(Descriptor *d);
void oedit_disp_extradesc_menu(Descriptor *d);
void oedit_disp_decays_menu(Descriptor *d);
void oedit_disp_weapon_menu(Descriptor *d);
void oedit_disp_weapon_type_menu(Descriptor *d);
void oedit_disp_val1_menu(Descriptor *d);
void oedit_disp_val2_menu(Descriptor *d);
void oedit_disp_val3_menu(Descriptor *d);
void oedit_disp_val4_menu(Descriptor *d);
void oedit_disp_val5_menu(Descriptor *d);
void oedit_disp_val6_menu(Descriptor *d);
void oedit_disp_val7_menu(Descriptor *d);
void oedit_disp_val8_menu(Descriptor *d);
void oedit_disp_val9_menu(Descriptor *d);
void oedit_disp_val10_menu(Descriptor *d);
void oedit_disp_val11_menu(Descriptor *d);
void oedit_disp_val12_menu(Descriptor *d);
void oedit_disp_clan1_menu(Descriptor *d);
void oedit_disp_clan2_menu(Descriptor *d);
void oedit_disp_clan3_menu(Descriptor *d);
void oedit_disp_clan4_menu(Descriptor *d);
void oedit_disp_clan5_menu(Descriptor *d);
void oedit_disp_clan6_menu(Descriptor *d);
void oedit_disp_clan7_menu(Descriptor *d);
void oedit_disp_clan8_menu(Descriptor *d);
void oedit_disp_clan9_menu(Descriptor *d);
void oedit_disp_clan10_menu(Descriptor *d);
void oedit_disp_type_menu(Descriptor *d);
void oedit_disp_extra_menu(Descriptor *d);
void oedit_disp_wear_menu(Descriptor *d);
void oedit_disp_menu(Descriptor *d);

void oedit_parse(Descriptor *d, char *arg);
void oedit_disp_spells_menu(Descriptor *d);
void oedit_liquid_type(Descriptor *d);
void oedit_setup_new(Descriptor *d);
void oedit_setup_existing(Descriptor *d, int real_num);
void oedit_save_internally(Descriptor *d);

/*------------------------------------------------------------------------*\
  Utility and exported functions
\*------------------------------------------------------------------------*/

void oedit_setup_new(Descriptor *d)
{
	OLC_OBJ(d) = new Object();

	OLC_OBJ(d)->proto				= true;
	OLC_OBJ(d)->name				= str_dup("unfinished object");
	OLC_OBJ(d)->description			= str_dup("An unfinished object is lying here.");
	OLC_OBJ(d)->short_description	= str_dup("an unfinished object");

	SET_BITK(GET_OBJ_WEAR(OLC_OBJ(d)), ITEM_WEAR_TAKE);
	OLC_VAL(d) = 0;
	oedit_disp_menu(d);
}

/*------------------------------------------------------------------------*/

void oedit_setup_existing(Descriptor *d, int real_num)
{
	struct ExtraDescription *thist, *temp, *temp2;
	Object *obj;

	obj = new Object();

	*obj = *obj_proto[real_num];

	/*
	 * Copy all strings over.
	 */

	obj->name =				 str_dup(obj_proto[real_num]->name
	                        ? obj_proto[real_num]->name : "undefined");

	obj->short_description	= str_dup(obj_proto[real_num]->short_description ?
	                                 obj_proto[real_num]->short_description : "undefined");

	obj->description		= str_dup(obj_proto[real_num]->description ?
	                            obj_proto[real_num]->description : "undefined");

	obj->action_description = (obj_proto[real_num]->action_description ?
	                           str_dup(obj_proto[real_num]->action_description) : NULL);

	//Extra descriptions if necessary.
	if (obj_proto[real_num]->ex_description)
	{
		temp = new ExtraDescription;

		obj->ex_description = temp;

		for (thist = obj_proto[real_num]->ex_description; thist; thist = thist->next)
		{
			temp->keyword = (thist->keyword && *thist->keyword) ? str_dup(thist->keyword) : NULL;
			temp->description = (thist->description && *thist->description) ?
			                    str_dup(thist->description) : NULL;

			if (thist->next)
			{
				temp2 = new ExtraDescription;
				temp2->keyword = 0;
				temp2->description = 0;

				temp->next = temp2;
				temp = temp2;
			}

			else
				temp->next = NULL;
		}
	}

	/*
	 * Attach new object to player's descriptor.
	 */

	OLC_OBJ(d) = obj;
	OLC_VAL(d) = 0;
	oedit_disp_menu(d);
}

/*------------------------------------------------------------------------*/

#define ZCMD zone_table[zone].cmd[cmd_no]

void oedit_save_internally(Descriptor *d)
{
	int shop, robj_num, found = FALSE;
	unsigned int cmd_no, i;
	struct ExtraDescription *thist, *next_one;
	struct Index *new_obj_index;
	Object *obj, *swap;
	Descriptor *dsc;
	Zone *zone;

	//Write object to internal tables.

	d->olc->obj->needs_save = true;

	if ((robj_num = real_object(OLC_NUM(d))) > 0)
	{
		/*
		 * We need to run through each and every object currently in the
		 * game to see which ones are pointing to this prototype.
		 * if object is pointing to this prototype, then we need to replace it
		 * with the new one.
		 */

		swap = new Object();

		for (obj = object_list; obj; obj = obj->next)
		{
			if( obj->IsPurged() ) continue; //we don't caaaaarrreeeeee about yo fuckin' purged objects
			if (obj->item_number == robj_num)
			{
				*swap = *obj;
				*obj = *OLC_OBJ(d);

				//Copy game-time dependent variables over.
				obj->in_room = swap->in_room;
				obj->item_number = robj_num;
				obj->carried_by = swap->carried_by;
				obj->worn_by = swap->worn_by;
				obj->worn_on = swap->worn_on;
				obj->in_obj = swap->in_obj;
				obj->contains = swap->contains;
				obj->next_content = swap->next_content;
				obj->next = swap->next;
				obj->SatOnBy = swap->SatOnBy;
				obj->objID = swap->objID;
				obj->retool_desc = swap->retool_desc;
				obj->retool_name = swap->retool_name;
				obj->retool_ex_desc = swap->retool_ex_desc;
				obj->retool_sdesc = swap->retool_sdesc;
			}
		}
		swap->retool_desc = NULL;
		swap->retool_name = NULL;
		swap->retool_ex_desc = NULL;
		swap->retool_sdesc = NULL;
		free_obj(swap);

		boost::uuids::uuid id = obj_proto[robj_num]->objID;
		*obj_proto[robj_num] = *OLC_OBJ(d);
		obj_proto[robj_num]->item_number = robj_num;
		obj_proto[robj_num]->needs_save = true;
		obj_proto[robj_num]->objID = id;
	}
	else
	{
		//It's a new object, we must build new tables to contain it.
		new_obj_index = new Index[top_of_objt + 2];
		memset(new_obj_index, 0, sizeof(Index) * (top_of_objt + 2));

		//Start counting through both tables.
		for (i = 0; i <= top_of_objt; ++i)
		{
			if (!found)
			{
				if (obj_index[i].vnum > OLC_NUM(d))
				{//This is the new item.
					found = TRUE;
					robj_num = i;
					V_InsertBefore(ItemCount, 0, i);
					OLC_OBJ(d)->item_number = robj_num;
					new_obj_index[robj_num].vnum = OLC_NUM(d);
					new_obj_index[robj_num].number = 0;
					new_obj_index[robj_num].func = NULL;

					Object *newObjectPrototype = new Object();
					*newObjectPrototype = *(OLC_OBJ(d));
					obj_proto.insert(obj_proto.begin() + robj_num, newObjectPrototype);
					obj_proto[robj_num]->in_room = 0;

					//Copy over the object that should be here.
					new_obj_index[robj_num + 1] = obj_index[robj_num];
				}

				else
				{//New item has not yet been found. Copy to i
					new_obj_index[i] = obj_index[i];
				}
			}

			else
			{//New item has been recorded. Now copy to i+1
				new_obj_index[i + 1] = obj_index[i];
				obj_proto[i]->item_number = i;
			}
		}

		if(!found)
		{
			robj_num = i;
			OLC_OBJ(d)->item_number = robj_num;
			new_obj_index[robj_num].vnum = OLC_NUM(d);
			new_obj_index[robj_num].number = 0;
			new_obj_index[robj_num].func = NULL;
			
			ItemCount.push_back(0);
			
			Object *newObjectPrototype = new Object();
			*newObjectPrototype = *(OLC_OBJ(d));
			obj_proto.push_back(newObjectPrototype);
			newObjectPrototype->in_room = NULL;
		}

		//Free and replace old tables.

		delete[] (obj_index);
		obj_index = new_obj_index;
		top_of_objt++;

		//Renumber live objects.
		for (obj = object_list; obj; obj = obj->next)
		{
			if (GET_OBJ_RNUM(obj) >= robj_num && !obj->IsPurged())
				++GET_OBJ_RNUM(obj);
		}

		//Renumber zone table.
		for (i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
			for (cmd_no = 0;cmd_no < zone->cmd.size(); ++cmd_no)
				switch (zone->cmd[cmd_no]->command)
				{

					case 'P':
						if (zone->cmd[cmd_no]->arg3 >= robj_num)
							zone->cmd[cmd_no]->arg3++;

						/*
						 * No break here - drop into next case.
						 */

					case 'O':
					case 'G':
					case 'E':
						if (zone->cmd[cmd_no]->arg1 >= robj_num)
							++zone->cmd[cmd_no]->arg1;

						break;

					case 'R':
						if (zone->cmd[cmd_no]->arg2 >= robj_num)
							++zone->cmd[cmd_no]->arg2;

						break;
				}

		//Renumber shop produce.
		for (shop = 0; shop < top_shop; shop++)
		{
			for (i = 0; i < shop_index[shop].producing.size(); ++i)
			{
				if (shop_index[shop].producing[i] >= robj_num)
					++shop_index[shop].producing[i];
			}
		}

		//Renumber produce in shops being edited.
		for (dsc = descriptor_list; dsc; dsc = dsc->next)
		{
			if (dsc->connected == CON_SEDIT)
			{
				for (i = 0; S_PRODUCT(OLC_SHOP(dsc), i) != -1; ++i)
				{
					if (S_PRODUCT(OLC_SHOP(dsc), i) >= robj_num)
					{
						++S_PRODUCT(OLC_SHOP(dsc), i);
					}
				}
			}
		}
	}

	zone = ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d));
	olc_add_to_save_list(zone->getVnum(), OLC_SAVE_OBJ);
}

/*------------------------------------------------------------------------*/

void oedit_save_to_disk(int zone_num)
{
	int realcounter;
	unsigned int counter;
	Object *obj;
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum(zone_num);

	if( zone->GetBottom()>zone->GetTop() )
	{
		MudLog(BRF, LVL_APPR, TRUE, "oedit_save_to_disk() : Zone #%d has invalid top(%d) and bottom(%d)",
			zone->GetTop(),zone->GetBottom());
		return;
	}

	for(counter = zone->GetBottom();counter <= zone->GetTop();++counter)
	{
		if ((realcounter = real_object(counter)) >= 0)
		{
			obj = (obj_proto[realcounter]);

			if(obj->deleted)
				obj->ProtoDelete();
			else if( obj->needs_save )
				obj->protoSave();
		}
	}

	olc_remove_from_save_list(zone->getVnum(), OLC_SAVE_OBJ);
}

/**************************************************************************
 Menu functions
 **************************************************************************/

/*
 * For container flags.
 */
void oedit_disp_container_flags_menu(Descriptor *d)
{
	get_char_cols(d->character);

	sprintbit(GET_OBJ_VAL(OLC_OBJ(d), 1), (const char **) container_bits, buf1);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	d->send(
	    "%s1%s) CLOSEABLE\r\n"
	    "%s2%s) CLOSED\r\n"
	    "%s3%s) LOCKED\r\n"
	    "Container flags: %s%s%s\r\n"
	    "Enter flag, 0 to quit : ",
	    grn, nrm, grn, nrm, grn, nrm, grn, buf1, nrm);
}

/*
 * For extra descriptions.
 */
void oedit_disp_extradesc_menu(Descriptor *d)
{
	ExtraDescription *extra_desc = OLC_DESC(d);

	strcpy(buf1, !extra_desc->next ? "<Not set>\r\n" : "Set.");

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	d->send(
	    "Extra desc menu\r\n"
	    "%s1%s) Keyword: %s%s\r\n"
	    "%s2%s) Description:\r\n%s%s\r\n"
	    "%s3%s) Goto next description: %s\r\n"
	    "%s0%s) Quit\r\n"
	    "Enter choice : ",

	    grn, nrm, yel, (extra_desc->keyword && *extra_desc->keyword) ? extra_desc->keyword : "<NONE>",
	    grn, nrm, yel, (extra_desc->description && *extra_desc->description) ? extra_desc->description : "<NONE>",
	    grn, nrm, buf1, grn, nrm);
	OLC_MODE(d) = OEDIT_EXTRADESC_MENU;
}

/*
 * For decays
 */
void oedit_disp_decays_menu(Descriptor *d)
{
	strcpy(buf1, (d->olc->obj->decayType == OBJECT_DECAY_GENERAL)	? "general"	:
				 (d->olc->obj->decayType == OBJECT_DECAY_CLOTH)		? "cloth"   :
				 (d->olc->obj->decayType == OBJECT_DECAY_WOOD)		? "wood"	:
				 (d->olc->obj->decayType == OBJECT_DECAY_METAL)		? "metal"	:
				 (d->olc->obj->decayType == OBJECT_DECAY_PLANT)		? "plant"	:
				 (d->olc->obj->decayType == OBJECT_DECAY_FOOD)		? "food"	:
				 "<NONE>" );
	
	strcpy(buf2, (d->olc->obj->decayTimerType == OBJECT_DECAY_MINUTES)	? "minutes" :
				 (d->olc->obj->decayTimerType == OBJECT_DECAY_HOURS)	? "hours"	:
				 (d->olc->obj->decayTimerType == OBJECT_DECAY_DAYS)		? "days"	:
				 (d->olc->obj->decayTimerType == OBJECT_DECAY_WEEKS)	? "weeks"	:
				 (d->olc->obj->decayTimerType == OBJECT_DECAY_MONTHS)	? "months"	:
				 (d->olc->obj->decayTimerType == OBJECT_DECAY_YEARS)	? "years"	:
				 "<NONE>" );

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	d->send(
	    "Decays Menu\r\n"
	    "%s1%s) Decay Type: %s%s\r\n"
	    "%s2%s) Decay Timer: %s%d\r\n"
	    "%s3%s) Decay Timer Type: %s%s\r\n"
	    "%s0%s) Quit\r\n"
	    "Enter choice : ",

		grn, nrm, yel, buf1,
		grn, nrm, yel, d->olc->obj->decayTimer,
		grn, nrm, yel, buf2,
		grn, nrm);
	OLC_MODE(d) = OEDIT_DECAYS_MENU;
}

/*
 * Ask for *which* apply to edit.
 */
void oedit_disp_prompt_apply_menu(Descriptor *d)
{
	int counter;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < MAX_OBJ_AFFECT; ++counter)
	{
		if (OLC_OBJ(d)->affected[counter].modifier)
		{
			sprinttype(OLC_OBJ(d)->affected[counter].location, (const char **) apply_types, buf2);

			d->send(" %s%d%s) %+d to %s\r\n", grn, counter + 1, nrm,
			        OLC_OBJ(d)->affected[counter].modifier, buf2);
		}
		else
		{
			d->send(" %s%d%s) None.\r\n", grn, counter + 1, nrm);
		}
	}
	d->send("\r\nEnter affection to modify (0 to quit) : ");
	OLC_MODE(d) = OEDIT_PROMPT_APPLY;
}

/*
 * Ask for liquid type.
 */
void oedit_liquid_type(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < NUM_LIQ_TYPES;++counter)
	{
		d->send(" %s%2d%s) %s%-20.20s %s", grn, counter, nrm, yel,
		        drinks[counter], !(++columns % 2) ? "\r\n" : "");
	}
	d->send("\r\n%sEnter drink type : ", nrm);
	OLC_MODE(d) = OEDIT_VALUE_3;
}

/*
 * The Actual apply to set.
 */
void oedit_disp_apply_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; *(apply_types[counter]) != '\n';++counter)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, counter, nrm,
		        apply_types[counter], !(++columns % 2) ? "\r\n" : "");
	}
	d->send("\r\nEnter apply type (0 is no apply) : ");
	OLC_MODE(d) = OEDIT_APPLY;
}

/*
 * Weapon type.
 */
void oedit_disp_weapon_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < NUM_ATTACK_TYPES; counter++)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, counter, nrm,
		        attack_hit_text[counter].singular,
		        !(++columns % 2) ? "\r\n" : "");
	}

	d->send("\r\nEnter weapon type : ");
}

/*
 * Spell type.
 */
void oedit_disp_spells_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < NUM_SPELLS;++counter)
	{
		d->send("%s%2d%s) %s%-20.20s %s", grn, counter, nrm, yel,
		        WeaveManager::GetManager().GetWeaveName(counter).c_str(), !(++columns % 3) ? "\r\n" : "");
	}
	d->send("\r\n%sEnter spell choice (0 for none) : ", nrm);
}

/*
 * Object value #1
 */
void oedit_disp_val1_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_1;
	printf("%d\n", GET_OBJ_TYPE(OLC_OBJ(d)));
	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_LIGHT:
			/*
			 * values 0 and 1 are unused.. jump to 2
			 */
			oedit_disp_val3_menu(d);
			break;
		case ITEM_POTION:
			d->send("Spell level : ");
			break;
		case ITEM_WEAPON:
			oedit_disp_weapon_type_menu(d);
			break;
		case ITEM_ARMOR:
			d->send("Apply to AC : ");
			break;
		case ITEM_CONTAINER:
			d->send("Max weight to contain : ");
			break;
		case ITEM_ANGREAL:
			d->send("Cost of Weaving, % out of 100 : ");
			break;
		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
			d->send("Max drink units : ");
			break;
		case ITEM_FOOD:
			d->send("Hours to fill stomach : ");
			break;
		case ITEM_MONEY:
			d->send("Number of gold coins : ");
			break;
		case ITEM_KEY:
			d->send("Number of uses before key breaks (0 for infinite) : ");
			break;
		case ITEM_NOTE:
			/*
			 * This is supposed to be language, but it's unused.
			 */
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #2
 */
void oedit_disp_val2_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_2;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{

		case ITEM_POTION:
			oedit_disp_spells_menu(d);
			break;

		case ITEM_WEAPON:
			d->send("Lower end of damage range : ");
			break;

		case ITEM_ANGREAL:
			d->send("Damage %, out of 100 : ");
			break;

		case ITEM_FOOD:
			/*
			 * Values 2 and 3 are unused, jump to 4...Odd.
			 */
			oedit_disp_val4_menu(d);
			break;

		case ITEM_CONTAINER:
			/*
			 * These are flags, needs a bit of special handling.
			 */
			oedit_disp_container_flags_menu(d);
			break;

		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
			d->send("Initial drink units : ");
			break;

		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #3
 */
void oedit_disp_val3_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_3;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{

		case ITEM_LIGHT:
			d->send("Number of hours (0 = burnt, -1 is infinite) : ");
			break;
		case ITEM_POTION:
			oedit_disp_spells_menu(d);
			break;
		case ITEM_WEAPON:
			d->send("Higher end of damage range : ");
			break;
		case ITEM_CONTAINER:
			d->send("Vnum of key to open container (-1 for no key) : ");
			break;
		case ITEM_ANGREAL:
			d->send("Number of Charges. -1 for unlimited : ");
			break;
		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
			oedit_liquid_type(d);
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #4
 */
void oedit_disp_val4_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_4;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_POTION:
			oedit_disp_spells_menu(d);
			break;
		case ITEM_LIGHT:
			d->send("Maxium hours of light if refuelled (0 = infinite) : ");
			break;
		case ITEM_WEAPON:
			oedit_disp_weapon_menu(d);
			break;
		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
		case ITEM_FOOD:
			d->send("Poisoned (0 = not poison) : ");
			break;
		case ITEM_CONTAINER:
			d->send("Enter pick requirement(-1 for unpickable) : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #5
 */
void oedit_disp_val5_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_5;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_LIGHT:
			d->send("Can this be refilled? (1 for yes, 0 for no) : ");
			break;
		case ITEM_WEAPON:
			d->send("Enter bash rating(0 - 100) : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #6
 */
void oedit_disp_val6_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_6;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter low end of backstab damage range : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #7
 */
void oedit_disp_val7_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_7;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter high end of backstab damage range : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #8
 */
void oedit_disp_val8_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_8;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter backstab rating(0 - 100) : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #9
 */
void oedit_disp_val9_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_9;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter low end of charge damage range : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #10
 */
void oedit_disp_val10_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_10;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter high end of charge damage range : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #11
 */
void oedit_disp_val11_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_11;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter charge rating(0 - 100) : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object value #12
 */
void oedit_disp_val12_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_VALUE_12;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			oedit_disp_weapon_menu(d);
			break;

		default:
			oedit_disp_menu(d);
	}
}

/*
 * Object clan value #1
 */
void oedit_disp_clan1_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_1;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_CONTAINER:
			d->send("Enter clan number : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #2
 */
void oedit_disp_clan2_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_2;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter extra offensive bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #3
 */
void oedit_disp_clan3_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_3;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter extra parry bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #4
 */
void oedit_disp_clan4_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_4;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter extra dodge bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #5
 */
void oedit_disp_clan5_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_5;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			break;
		case ITEM_ARMOR:
			d->send("Enter absorption bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #6
 */
void oedit_disp_clan6_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_6;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter weight bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #7
 */
void oedit_disp_clan7_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_7;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter moves bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #8
 */
void oedit_disp_clan8_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_8;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
		case ITEM_ARMOR:
			d->send("Enter hit point bonus : ");
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #9
 */
void oedit_disp_clan9_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_9;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter bonus to low damage range : ");
		case ITEM_ARMOR:
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object clan value #10
 */
void oedit_disp_clan10_menu(Descriptor *d)
{
	OLC_MODE(d) = OEDIT_CLAN_10;

	switch (GET_OBJ_TYPE(OLC_OBJ(d)))
	{
		case ITEM_WEAPON:
			d->send("Enter bonus to high damage range : ");
		case ITEM_ARMOR:
			break;
		default:
			oedit_disp_menu(d);
	}
}
/*
 * Object type.
 */
void oedit_disp_type_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < NUM_ITEM_TYPES; counter++)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, counter, nrm,
		        item_types[counter], !(++columns % 2) ? "\r\n" : "");
	}
	d
	->send("\r\nEnter object type : ");
}

/*
 * Object extra flags.
 */
void oedit_disp_extra_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->send("[H[J");
#endif

	for (counter = 0; counter < NUM_ITEM_FLAGS; counter++)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, counter + 1, nrm,
		        extra_bits[counter], !(++columns % 2) ? "\r\n" : "");
	}

	sprintbit(GET_OBJ_EXTRA(OLC_OBJ(d)),extra_bits, buf1);
	d->send("\r\nObject flags: %s%s%s\r\n"
	        "Enter object extra flag (0 to quit) : ",
	        cyn, buf1, nrm);
}

/*
 * Object wear flags.
 */
void oedit_disp_wear_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send("[H[J");

#endif

	for (counter = 0; counter < NUM_ITEM_WEARS; counter++)
	{
		d->send("%s%2d%s) %-20.20s %s", grn, counter + 1, nrm,
		        wear_bits[counter], !(++columns % 2) ? "\r\n" : "");
	}

	sprintbit(GET_OBJ_WEAR(OLC_OBJ(d)), wear_bits, buf1);
	d->send("\r\nWear flags: %s%s%s\r\n"
	        "Enter wear flag, 0 to quit : ", cyn, buf1, nrm);
}

void oedit_disp_weapon_type_menu(Descriptor *d)
{

	int counter, columns = 0;

	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->send(".[H.[J");

#endif

	for (counter = 0; counter < MAX_WEAPON_TYPE; counter++)
		d->send("%s%2d%s) %-20.20s %s", grn, counter, nrm,
		        weapon_types[counter], !(++columns % 2) ? "\r\n" : "");

	d->send("\r\nChoose a number listed for the weapon type.\r\n");
}

/*
 * Display main menu.
 */
void oedit_disp_menu(Descriptor *d)
{
	Object *obj;

	obj = OLC_OBJ(d);
	get_char_cols(d->character);

	/*
	 * Build buffers for first part of menu.
	 */

	sprinttype(GET_OBJ_TYPE(obj), (const char **) item_types, buf1);
	sprintbit(GET_OBJ_EXTRA(obj), extra_bits, buf2);

	/*
	 * Build first half of menu.
	*/

	d->send(
#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif
	    "-- Item number : [%s%d%s]\r\n"
	    "%s1%s) Namelist : %s%s\r\n"
	    "%s2%s) S-Desc   : %s%s\r\n"
	    "%s3%s) L-Desc   :-\r\n%s%s\r\n"
	    "%s4%s) A-Desc   :-\r\n%s%s"
	    "%s5%s) Type        : %s%s\r\n"
	    "%s6%s) Extra flags : %s%s\r\n",

	    cyn, OLC_NUM(d), nrm,
	    grn, nrm, yel, (obj->name && *obj->name) ? obj->name : "undefined",
	    grn, nrm, yel, (obj->short_description && *obj->short_description) ? obj->short_description : "undefined",
	    grn, nrm, yel, (obj->description && *obj->description) ? obj->description : "undefined",
	    grn, nrm, yel, (obj->action_description && *obj->action_description) ? obj->action_description : "<not set>\r\n",
	    grn, nrm, cyn, buf1,
	    grn, nrm, cyn, buf2

	);

	/*
	 * Build second half of menu.
	 */
	sprintbit(GET_OBJ_WEAR(obj), wear_bits, buf1);
	d->send(
	    "%s7%s) Wear flags  : %s%s\r\n"
	    "%s8%s) Weight      : %s%.1f\r\n"
	    "%s9%s) Cost        : %s%d G, %d S, %d C\r\n"
	    "%sA%s) Cost/Day    : %s%d G, %d S, %d C\r\n"
	    "%sB%s) Timer       : %s%d\r\n"
	    "%sC%s) Maximum     : %s%d\r\n"
	    "%sD%s) Offensive   : %s%d\r\n"
	    "%sE%s) Parry       : %s%d\r\n"
	    "%sF%s) Dodge       : %s%d\r\n"
	    "%sG%s) Absorb      : %s%d\r\n"
		"%s-%s) Bash        : %s%f\r\n"
	    "%sH%s) Values      : %s%d %d %d %d %d %d %d %d %d %d %d %d\r\n"
	    "%sI%s) Clan        : %s%d %d %d %d %d %.1f %d %d %d %d\r\n"
		"%sK%s) Delete      : %s%s\r\n"
		"%sJ%s) JavaScript  : %s%s\r\n"
	    "%sT%s) Applies menu\r\n"
	    "%sU%s) Extra descriptions menu\r\n"
		"%sV%s) Decay menu\r\n"
	    "%sQ%s) Quit\r\n"
	    "Enter choice : ",

	    grn, nrm, cyn, buf1,
	    grn, nrm, cyn, obj->obj_flags.weight,
	    grn, nrm, cyn, CalcGold(GET_OBJ_COST(obj)), CalcSilver(GET_OBJ_COST(obj)), CalcCopper(GET_OBJ_COST(obj)),
	    grn, nrm, cyn, CalcGold(GET_OBJ_RENT(obj)), CalcSilver(GET_OBJ_RENT(obj)), CalcCopper(GET_OBJ_RENT(obj)),
	    grn, nrm, cyn, GET_OBJ_TIMER(obj),
	    grn, nrm, cyn, obj->Max,
	    grn, nrm, cyn, GET_OBJ_OB(obj),
	    grn, nrm, cyn, GET_OBJ_PB(obj),
	    grn, nrm, cyn, GET_OBJ_DB(obj),
	    grn, nrm, cyn, GET_OBJ_ABS(obj),
		grn, nrm, cyn, obj->BashRating(),
	    grn, nrm, cyn, GET_OBJ_VAL(obj, 0),
	    GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 2),
	    GET_OBJ_VAL(obj, 3), GET_OBJ_VAL(obj, 4),
	    GET_OBJ_VAL(obj, 5), GET_OBJ_VAL(obj, 6),
	    GET_OBJ_VAL(obj, 7), GET_OBJ_VAL(obj, 8),
	    GET_OBJ_VAL(obj, 9), GET_OBJ_VAL(obj, 10),
	    GET_OBJ_VAL(obj, 11),
		grn, nrm, cyn, GET_OBJ_CLAN(obj),
		GET_OBJ_CL_OB(obj), GET_OBJ_CL_PB(obj),
		GET_OBJ_CL_DB(obj), GET_OBJ_CL_ABS(obj),
		GET_OBJ_CL_WEIGHT(obj), GET_OBJ_CL_MVS(obj),
		GET_OBJ_CL_HPS(obj), GET_OBJ_CL_DMG1(obj),
		GET_OBJ_CL_DMG2(obj),
	    grn, nrm, cyn, obj->deleted ? "Yes" : "No",
		grn, nrm, cyn, (obj->js_scripts->size() ? "Set." : "Not Set."),
	    grn, nrm,
	    grn, nrm,
		grn, nrm,
	    grn, nrm);
	OLC_MODE(d) = OEDIT_MAIN_MENU;
}

/***************************************************************************
 main loop (of sorts).. basically interpreter throws all input to here
 ***************************************************************************/

void oedit_parse(Descriptor *d, char *arg)
{
	int number, max_val, min_val;

	switch (OLC_MODE(d))
	{

		case OEDIT_CONFIRM_SAVESTRING:

			switch (*arg)
			{

				case 'y':
				case 'Y':
					d->send("Saving object to memory.\r\n");
					oedit_save_internally(d);
					AddOlcLog( d->character, "obj", d->olc->obj->getVnum() );
					cleanup_olc(d, CLEANUP_STRUCTS);
					return;

				case 'n':
				case 'N':
					/*
					 * Cleanup all.
					 */
					cleanup_olc(d, CLEANUP_ALL);
					return;

				default:
					d->send("Invalid choice!\r\n");
					d->send("Do you wish to save this object internally?\r\n");
					return;
			}

		case OEDIT_MAIN_MENU:
			/*
			 * Throw us out to whichever edit mode based on user input.
			 */

			switch (*arg)
			{
				case 'q':
				case 'Q':
					if (OLC_VAL(d))
					{	/* Something has been modified. */
						d->send("Do you wish to save this object internally? : ");
						OLC_MODE(d) = OEDIT_CONFIRM_SAVESTRING;
					}
					else
						cleanup_olc(d, CLEANUP_ALL);
					return;
				case '1':
					d->send("Enter namelist : ");
					OLC_MODE(d) = OEDIT_EDIT_NAMELIST;
					break;
				case '2':
					d->send("Enter short desc : ");
					OLC_MODE(d) = OEDIT_SHORTDESC;
					break;
				case '3':
					d->send("Enter long desc :-\r\n| ");
					OLC_MODE(d) = OEDIT_LONGDESC;
					break;
				case '4':
					OLC_MODE(d) = OEDIT_ACTDESC;
					d->send("Enter action description: (/s saves /h for help)\r\n\r\n");
					d->backstr = NULL;

					if (OLC_OBJ(d)->action_description)
					{
						d->send(OLC_OBJ(d)->action_description);
						d->backstr = str_dup(OLC_OBJ(d)->action_description);
					}
					d->str = &OLC_OBJ(d)->action_description;
					d->max_str = MAX_MESSAGE_LENGTH;
					OLC_VAL(d) = 1;
					break;
				case '5':
					oedit_disp_type_menu(d);
					OLC_MODE(d) = OEDIT_TYPE;
					break;
				case '6':
					oedit_disp_extra_menu(d);
					OLC_MODE(d) = OEDIT_EXTRAS;
					break;
				case '7':
					oedit_disp_wear_menu(d);
					OLC_MODE(d) = OEDIT_WEAR;
					break;
				case '8':
					d->send("Enter weight : ");
					OLC_MODE(d) = OEDIT_WEIGHT;
					break;
				case '9':
					d->send("Enter cost : ");
					OLC_MODE(d) = OEDIT_COST;
					break;
				case 'a':
				case 'A':
					d->send("Enter cost per day : ");
					OLC_MODE(d) = OEDIT_COSTPERDAY;
					break;
				case 'b':
				case 'B':
					d->send("Enter timer : ");
					OLC_MODE(d) = OEDIT_TIMER;
					break;
				case 'c':
				case 'C':
					d->send("Enter the maximum number of this object allowed to exist : ");
					OLC_MODE(d) = OEDIT_MAX;
					break;
				case 'd':
				case 'D':
					d->send("Enter Value :");
					OLC_MODE(d) = OEDIT_OB;
					break;
				case 'e':
				case 'E':
					d->send("Enter Value :");
					OLC_MODE(d) = OEDIT_PB;
					break;
				case 'f':
				case 'F':
					d->send("Enter Value :");
					OLC_MODE(d) = OEDIT_DB;
					break;
				case 'g':
				case 'G':
					d->send("Enter Value :");
					OLC_MODE(d) = OEDIT_ABS;
					break;
				case 'h':
				case 'H':
					/*
					 * Clear any old values
					 */
					OLC_VAL(d) = 1;
					GET_OBJ_VAL(OLC_OBJ(d), 0) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 1) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 2) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 3) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 4) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 5) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 6) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 7) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 8) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 9) = 0;
					GET_OBJ_VAL(OLC_OBJ(d), 10) = 0;
					oedit_disp_val1_menu(d);
					break;
				case 'i':
				case 'I':
					/*
					 * Clear any old values
					 */
					GET_OBJ_CLAN(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_OB(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_PB(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_DB(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_ABS(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_WEIGHT(OLC_OBJ(d)) = 0.0;
					GET_OBJ_CL_MVS(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_HPS(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_DMG1(OLC_OBJ(d)) = 0;
					GET_OBJ_CL_DMG2(OLC_OBJ(d)) = 0;
					oedit_disp_clan1_menu(d);
					break;
				case 'k':
				case 'K':
					if(OLC_OBJ(d)->deleted)
					{
						d->send("This item will no longer be deleted.\r\n");
						OLC_OBJ(d)->deleted = false;
					}
					else
					{
						d->send("This item will be deleted after the next reboot.\r\n");
						OLC_OBJ(d)->deleted = true;
					}
					OLC_VAL(d) = 1;
					oedit_disp_menu(d);
					return;
				case 't':
				case 'T':
					OLC_VAL(d) = 1;
					oedit_disp_prompt_apply_menu(d);
					break;
				case 'u':
				case 'U':
					/*
					 * If extra descriptions don't exist.
					 */
					if (!OLC_OBJ(d)->ex_description)
					{
						OLC_OBJ(d)->ex_description = new ExtraDescription;
						memset(OLC_OBJ(d)->ex_description, 0, sizeof(ExtraDescription));
					}
					OLC_VAL(d) = 1;
					OLC_DESC(d) = OLC_OBJ(d)->ex_description;
					oedit_disp_extradesc_menu(d);
					break;
				case 'v':
				case 'V':
					OLC_VAL(d) = 1;
					oedit_disp_decays_menu(d);
					break;
				case 'j':
				case 'J':
					OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_MAIN_MENU;
					OLC_MODE(d) = OLC_JSCRIPT_EDIT;
					d->olc->jsScripts = d->olc->obj->js_scripts;
					JScriptDispMenu(d);
					return;
				default:
					oedit_disp_menu(d);
					break;
			}
			return;			/*
							 * end of OEDIT_MAIN_MENU
							 */

		case OLC_JSCRIPT_EDIT:
			if( JScriptParse(d, arg) )
				return;
			break;

		case OEDIT_EDIT_NAMELIST:
			if (OLC_OBJ(d)->name)
				delete[] (OLC_OBJ(d)->name);
			OLC_OBJ(d)->name = str_dup((arg && *arg) ? arg : "undefined");
			break;

		case OEDIT_SHORTDESC:
			if (OLC_OBJ(d)->short_description)
				delete[] (OLC_OBJ(d)->short_description);
			OLC_OBJ(d)->short_description = str_dup((arg && *arg) ? arg : "undefined");
			break;

		case OEDIT_LONGDESC:
			if (OLC_OBJ(d)->description)
				delete[] (OLC_OBJ(d)->description);
			OLC_OBJ(d)->description = str_dup((arg && *arg) ? arg : "undefined");
			break;

		case OEDIT_TYPE:
			number = atoi(arg);
			if ((number < 1) || (number >= NUM_ITEM_TYPES))
			{
				d->send("Invalid choice, try again : ");
				return;
			}
			else
				GET_OBJ_TYPE(OLC_OBJ(d)) = number;
			break;

		case OEDIT_EXTRAS:
			number = atoi(arg);
			if ((number < 0) || (number > NUM_ITEM_FLAGS))
			{
				oedit_disp_extra_menu(d);
				return;
			}
			else if (number == 0)
				break;
			else
			{
				TOGGLE_FLAG(GET_OBJ_EXTRA(OLC_OBJ(d)), (number - 1));
				oedit_disp_extra_menu(d);
				return;
			}

		case OEDIT_WEAR:
			number = atoi(arg);
			if ((number < 0) || (number > NUM_ITEM_WEARS))
			{
				d->send("That's not a valid choice!\r\n");
				oedit_disp_wear_menu(d);
				return;
			}
			else if (number == 0)	/* Quit. */
				break;
			else
			{
				TOGGLE_FLAG(GET_OBJ_WEAR(OLC_OBJ(d)), (number - 1));
				oedit_disp_wear_menu(d);
				return;
			}

		case OEDIT_WEIGHT:
			GET_OBJ_WEIGHT(OLC_OBJ(d))	= (double)atof(arg);
			break;
		case OEDIT_COST:
			GET_OBJ_COST(OLC_OBJ(d))	= atoi(arg);
			break;
		case OEDIT_COSTPERDAY:
			GET_OBJ_RENT(OLC_OBJ(d))	= atoi(arg);
			break;
		case OEDIT_TIMER:
			GET_OBJ_TIMER(OLC_OBJ(d))		= atoi(arg);
			break;
		case OEDIT_OB:
			GET_OBJ_OB(OLC_OBJ(d))		= atoi(arg);
			break;
		case OEDIT_PB:
			GET_OBJ_PB(OLC_OBJ(d))		= atoi(arg);
			break;
		case OEDIT_ABS:
			GET_OBJ_ABS(OLC_OBJ(d))		= atoi(arg);
			break;
		case OEDIT_DB:
			GET_OBJ_DB(OLC_OBJ(d))		= atoi(arg);
			break;
		case OEDIT_MAX:
			OLC_OBJ(d)->Max				= atoi(arg);
			break;
		case OEDIT_VALUE_1:
			/*
			 * Lucky, I don't need to check any of these for out of range values.
			 * Hmm, I'm not so sure - Rv
			 */
			GET_OBJ_VAL(OLC_OBJ(d), 0) = atoi(arg);
			/*
			 * proceed to menu 2
			 */
			oedit_disp_val2_menu(d);
			return;
		case OEDIT_VALUE_2:
			/*
			 * Here, I do need to check for out of range values.
			 */
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_POTION:
					if (number < 0 || number >= NUM_SPELLS)
						oedit_disp_val2_menu(d);
					else
					{
						GET_OBJ_VAL(OLC_OBJ(d), 1) = number;
						oedit_disp_val3_menu(d);
					}
					break;
				case ITEM_CONTAINER:
					/*
					 * Needs some special handling since we are dealing with flag values
					 * here.
					 */
					if (number < 0 || number > 3)
						oedit_disp_container_flags_menu(d);
					else if (number != 0)
					{
						TOGGLE_BIT(GET_OBJ_VAL(OLC_OBJ(d), 1), Q_BIT(number-1));
						Log("Closeable: %s", StringUtil::allUpper(StringUtil::yesNo(IS_SET(GET_OBJ_VAL(OLC_OBJ(d),1), CONT_CLOSEABLE))).c_str());
						Log("Locked: %s", StringUtil::allUpper(StringUtil::yesNo(IS_SET(GET_OBJ_VAL(OLC_OBJ(d),1), CONT_LOCKED))).c_str());
						Log("Closed: %s", StringUtil::allUpper(StringUtil::yesNo(IS_SET(GET_OBJ_VAL(OLC_OBJ(d),1), CONT_CLOSED))).c_str());
						OLC_VAL(d) = 1;
						oedit_disp_val2_menu(d);
					}
					else
						oedit_disp_val3_menu(d);
					break;
				default:
					GET_OBJ_VAL(OLC_OBJ(d), 1) = number;
					oedit_disp_val3_menu(d);
			}
			return;

		case OEDIT_VALUE_3:
			number = atoi(arg);
			/*
			 * Quick'n'easy error checking.
			 */
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_LIGHT:
					min_val = 0;
					max_val = 32000;
					break;
				case ITEM_POTION:
					min_val = 0;
					max_val = NUM_SPELLS - 1;
					break;
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 32000;
					break;
				case ITEM_DRINKCON:
				case ITEM_FOUNTAIN:
					min_val = 0;
					max_val = NUM_LIQ_TYPES - 1;
					break;
				default:
					min_val = -32000;
					max_val = 32000;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 2) = MAX(min_val, MIN(number, max_val));
			oedit_disp_val4_menu(d);
			return;

		case OEDIT_VALUE_4:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_LIGHT:
					min_val = 0;
					max_val = 32000;
					break;
				case ITEM_POTION:
					min_val = 0;
					max_val = NUM_SPELLS - 1;
					break;
				case ITEM_WEAPON:
					min_val = 0;
					max_val = NUM_ATTACK_TYPES - 1;
				case ITEM_FOOD:
					min_val = 0;
					max_val = 48;
					break;
				case ITEM_CONTAINER:
					min_val = -1;
					max_val = 100;
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 3) = MAX(min_val, MIN(number, max_val));
			oedit_disp_val5_menu(d);
			return;
		case OEDIT_VALUE_5:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_LIGHT:
					min_val = 0;
					max_val = 1;
					break;
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 100;
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 4) = MAX(min_val, MIN(number, max_val));
			OLC_VAL(d) = 1;
			oedit_disp_val6_menu(d);
			return;
		case OEDIT_VALUE_6:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 5000;
					oedit_disp_val7_menu(d);
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 5) = MAX(min_val, MIN(number, max_val));
			return;
		case OEDIT_VALUE_7:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 5000;
					oedit_disp_val8_menu(d);
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 6) = MAX(min_val, MIN(number, max_val));
			return;
		case OEDIT_VALUE_8:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 100;
					oedit_disp_val9_menu(d);
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 7) = MAX(min_val, MIN(number, max_val));
			return;
		case OEDIT_VALUE_9:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 5000;
					oedit_disp_val10_menu(d);
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 8) = MAX(min_val, MIN(number, max_val));
			return;
		case OEDIT_VALUE_10:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 5000;
					oedit_disp_val11_menu(d);
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 9) = MAX(min_val, MIN(number, max_val));
			return;
		case OEDIT_VALUE_11:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					min_val = 0;
					max_val = 100;
					break;
				default:
					min_val = -32000;
					max_val = 32000;
					break;
			}
			GET_OBJ_VAL(OLC_OBJ(d), 10) = MAX(min_val, MIN(number, max_val));
			break;

		case OEDIT_CLAN_1:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
				case ITEM_ARMOR:
				case ITEM_CONTAINER:
					GET_OBJ_CLAN(OLC_OBJ(d)) = number;
					oedit_disp_clan2_menu(d);
					break;
				default:
					break;
			}
			if(GET_OBJ_TYPE(OLC_OBJ(d)) == ITEM_CONTAINER)
				break;
			return;
		case OEDIT_CLAN_2:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
				case ITEM_ARMOR:
					GET_OBJ_CL_OB(OLC_OBJ(d)) = number;
					oedit_disp_clan3_menu(d);
					break;
				default:
					break;
			}
			return;
		case OEDIT_CLAN_3:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
				case ITEM_ARMOR:
					GET_OBJ_CL_PB(OLC_OBJ(d)) = number;
					oedit_disp_clan4_menu(d);
					break;
				default:
					break;
			}
			return;
		case OEDIT_CLAN_4:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					GET_OBJ_CL_DB(OLC_OBJ(d)) = number;
					oedit_disp_clan6_menu(d);
					break;
				case ITEM_ARMOR:
					GET_OBJ_CL_DB(OLC_OBJ(d)) = number;
					oedit_disp_clan5_menu(d);
					break;
				default:
					break;
			}
			return;
		case OEDIT_CLAN_5:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_ARMOR:
					GET_OBJ_CL_ABS(OLC_OBJ(d)) = number;
				default:
					oedit_disp_clan6_menu(d);
					break;
			}
			return;
		case OEDIT_CLAN_6:
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
				case ITEM_ARMOR:
					GET_OBJ_CL_WEIGHT(OLC_OBJ(d)) = (double)atof(arg);
					oedit_disp_clan7_menu(d);
					break;
				default:
					break;
			}
			return;
		case OEDIT_CLAN_7:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
				case ITEM_ARMOR:
					GET_OBJ_CL_MVS(OLC_OBJ(d)) = number;
					oedit_disp_clan8_menu(d);
					break;
				default:
					break;
			}
			return;
		case OEDIT_CLAN_8:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					GET_OBJ_CL_HPS(OLC_OBJ(d)) = number;
					oedit_disp_clan9_menu(d);
					return;
				case ITEM_ARMOR:
					GET_OBJ_CL_HPS(OLC_OBJ(d)) = number;
				default:
					break;
			}
			break;
		case OEDIT_CLAN_9:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					GET_OBJ_CL_DMG1(OLC_OBJ(d)) = number;
				default:
					oedit_disp_clan10_menu(d);
					break;
			}
			return;
		case OEDIT_CLAN_10:
			number = atoi(arg);
			switch (GET_OBJ_TYPE(OLC_OBJ(d)))
			{
				case ITEM_WEAPON:
					GET_OBJ_CL_DMG2(OLC_OBJ(d)) = number;
				default:
					break;
			}
			break;
		case OEDIT_PROMPT_APPLY:
			if ((number = atoi(arg)) == 0)
				break;
			else if (number < 0 || number > MAX_OBJ_AFFECT)
			{
				oedit_disp_prompt_apply_menu(d);
				return;
			}
			OLC_VAL(d) = number - 1;
			OLC_MODE(d) = OEDIT_APPLY;
			oedit_disp_apply_menu(d);
			return;

		case OEDIT_APPLY:
			if ((number = atoi(arg)) == 0)
			{
				OLC_OBJ(d)->affected[OLC_VAL(d)].location = 0;
				OLC_OBJ(d)->affected[OLC_VAL(d)].modifier = 0;
				oedit_disp_prompt_apply_menu(d);
			}
			else if (number < 0 || number >= NUM_APPLIES)
				oedit_disp_apply_menu(d);
			else
			{
				OLC_OBJ(d)->affected[OLC_VAL(d)].location = number;
				d->send("Modifier : ");
				OLC_MODE(d) = OEDIT_APPLYMOD;
			}
			return;

		case OEDIT_APPLYMOD:
			OLC_OBJ(d)->affected[OLC_VAL(d)].modifier = atoi(arg);
			oedit_disp_prompt_apply_menu(d);
			return;

		case OEDIT_EXTRADESC_KEY:
			if (OLC_DESC(d)->keyword)
				delete[] (OLC_DESC(d)->keyword);
			OLC_DESC(d)->keyword = str_dup((arg && *arg) ? arg : "undefined");
			oedit_disp_extradesc_menu(d);
			return;

		case OEDIT_EXTRADESC_MENU:
			switch ((number = atoi(arg)))
			{
				case 0:
					if (!OLC_DESC(d)->keyword || !OLC_DESC(d)->description)
					{
						ExtraDescription **tmp_desc;

						/*
						 * Clean up pointers
						 */
						for (tmp_desc = &(OLC_OBJ(d)->ex_description); *tmp_desc;tmp_desc = &((*tmp_desc)->next))
						{
							if(*tmp_desc == OLC_DESC(d))
							{
								*tmp_desc = NULL;
								break;
							}
						}
						delete (OLC_DESC(d));
					}
					break;

				case 1:
					OLC_MODE(d) = OEDIT_EXTRADESC_KEY;
					d->send("Enter keywords, separated by spaces :-\r\n| ");
					return;

				case 2:
					OLC_MODE(d) = OEDIT_EXTRADESC_DESCRIPTION;
					d->send("Enter the extra description: (/s saves /h for help)\r\n\r\n");
					d->backstr = NULL;
					if (OLC_DESC(d)->description)
					{
						d->send(OLC_DESC(d)->description);
						d->backstr = str_dup(OLC_DESC(d)->description);
					}
					d->str = &OLC_DESC(d)->description;
					d->max_str = MAX_MESSAGE_LENGTH;
					OLC_VAL(d) = 1;
					return;

				case 3:
					/*
					 * Only go to the next description if this one is finished.
					 */
					if (OLC_DESC(d)->keyword && OLC_DESC(d)->description)
					{
						struct ExtraDescription *new_extra;

						if (OLC_DESC(d)->next)
							OLC_DESC(d) = OLC_DESC(d)->next;
						else
						{	/* Make new extra description and attach at end. */
							new_extra = new ExtraDescription;
							memset(new_extra, 0, sizeof(ExtraDescription));
							OLC_DESC(d)->next = new_extra;
							OLC_DESC(d) = OLC_DESC(d)->next;
						}
					}
					/*
					 * No break - drop into default case.
					 */
				default:
					oedit_disp_extradesc_menu(d);
					return;
			}
			break;
		case OEDIT_DECAYS_MENU:
			switch ((number = atoi(arg)))
			{
				case 0:
					oedit_disp_menu(d);
					return;
				case 1:
					OLC_MODE(d) = OEDIT_DECAY_TYPE;
					d->send("Enter an item type, or -1 to clear. The choices are: \r\n"
							"1) General	\r\n"
							"2) Cloth	\r\n"
							"3) Wood	\r\n"
							"4) Metal	\r\n"
							"5) Plant	\r\n"
							"6) Food	\r\n"
							"0) Quit	\r\n"
							"Your choice: ");
					return;
				case 2:
					OLC_MODE(d) = OEDIT_DECAY_TIMER;
					d->send("Enter the timer length: \r\n");
					return;
				case 3:
					OLC_MODE(d) = OEDIT_DECAY_TIMERTYPE;
					d->send("Enter a timer type, or -1 to clear. The choices are: \r\n"
							"1) Minutes \r\n"
							"2) Hours	\r\n"
							"3) Days	\r\n"
							"4) Weeks	\r\n"
							"5) Months	\r\n"
							"6) Years	\r\n"
							"0) Quit	\r\n"
							"Your choice: ");
					return;
				default:
					oedit_disp_decays_menu(d);
					return;
			}
			break;
		case OEDIT_DECAY_TIMER:
			d->olc->obj->decayTimer = atoi(arg);
			OLC_MODE(d) = OEDIT_DECAYS_MENU;
			OLC_VAL(d) = 1;
			oedit_disp_decays_menu(d);
			return;
		case OEDIT_DECAY_TYPE:
			number = atoi(arg);
			if( number == 0 )
			{
				oedit_disp_decays_menu(d);
				return;
			}
			else if( number < -1 || number > OBJECT_DECAY_FOOD )
			{
				d->send("Choice must be between 1 and 6.\r\n");
				return;
			}
			d->olc->obj->decayType = number;
			OLC_VAL(d) = 1;
			oedit_disp_decays_menu(d);
			return;
		case OEDIT_DECAY_TIMERTYPE:
			number = atoi(arg);
			if( number == 0 )
			{
				oedit_disp_decays_menu(d);
				return;
			}
			else if( number < -1 || number > OBJECT_DECAY_YEARS )
			{
				d->send("Choice must be between 1 and 6.\r\n");
				return;
			}
			d->olc->obj->decayTimerType = number;
			OLC_VAL(d) = 1;
			oedit_disp_decays_menu(d);
			return;
		default:
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: Reached default case in oedit_parse()!");
			d->send("Oops...\r\n");
			break;
	}

	/*
	 * If we get here, we have changed something.
	 */
	OLC_VAL(d) = 1;
	oedit_disp_menu(d);
}




















