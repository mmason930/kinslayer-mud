/*
	Joshua Roys - Serai of 'Kinslayer MUD' and 'Wheel of Time MUD' 2004, 2005
	http://www.epicsol.org/kinslayer/
	http://www.wotmud.org/
	http://mud.frostmud.com/~serai/
 
04/09/04 22:14 - Started kedit.cpp
04/10/04 17:30 - Can create new kits
04/13/04 22:24 - New faster computer :)
                 Kits save to disk
04/14/04 01:26 - Kits load from disk
04/14/04 03:07 - Appears to be done.  Bugtesting.
07/10/04 02:35 - Added multiple kits and percent loads.  Hopefully.
04/20/09 12:13 - Converted to database load/save. (Galnor)
04/21/09 01:55 - Converted C-style list to C++, and added KitManager class. (Galnor)
 
*/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "olc.h"
#include "constants.h"
#include "interpreter.h"

#include "MiscUtil.h"
#include "Descriptor.h"

ACMD(do_klist);

#if 0
const int KEDIT_DEBUG = 1; 
#endif

// externs
KitManager *KitManager::Self = NULL;

// local functions
void kedit_disp_menu(Descriptor *);
void kedit_parse(Descriptor *, char *);
void kedit_save_internally(Descriptor *);
void kedit_setup_existing(Descriptor *, Kit *);
void kedit_setup_new(Descriptor *);

KitManager::KitManager()
{
}
KitManager::~KitManager()
{
	while( !MyKits.empty() )
	{
		delete (MyKits.front());
		MyKits.pop_front();
	}
}
KitManager &KitManager::GetManager()
{
	if( Self == NULL )
		Self = new KitManager();
	return (*Self);
}
void KitManager::Free()
{
	if( Self )
		delete (Self);
}

Kit *KitManager::GetKitByVnum( const int vnum )
{
	//too lazy to binary search this sucker. Hopefully we won't have a proble with too many kits in the list.
	for( std::list<Kit *>::iterator kIter = MyKits.begin();kIter != MyKits.end();++kIter )
	{
		if( (*kIter)->vnum == vnum )
			return (*kIter);
	}
	return (NULL);
}
void KitManager::AddKit( Kit *k )
{
	for( std::list<Kit *>::iterator kIter = MyKits.begin();kIter != MyKits.end();++kIter )
	{
		if( (*kIter)->vnum > k->vnum )
		{
			MyKits.insert(kIter, k);
			return;
		}
	}
	MyKits.push_back(k);
}
std::list<Kit *>::iterator KitManager::RemoveKit( Kit *k )
{
	std::list<Kit *>::iterator kIter = std::find( MyKits.begin(),MyKits.end(), k );

	if( kIter == MyKits.end() )
	{//Not found
		return kIter;//Return the end of the list.
	}
	return MyKits.erase(kIter);//Remove & return new valid iterator.
}
std::list<Kit *>::iterator KitManager::RemoveKit( const int vnum )
{
	return (RemoveKit( GetKitByVnum( vnum ) ));
}
std::list<Kit *> KitManager::GetAllKits()
{
	return (this->MyKits);
}
std::list<Kit *> KitManager::GetRangeOfKits( int lowVnum, int highVnum )
{
	if( lowVnum > highVnum )
	{//swap with a XOR mask.
		lowVnum ^= highVnum;
		highVnum ^= lowVnum;
		lowVnum ^= highVnum;
	}
	std::list<Kit *> RangeKits;
	std::list<Kit *>::iterator kIter;

	//Loop until we hit the bottom of the vnum range.
	for( kIter = MyKits.begin();kIter != MyKits.end() && (*kIter)->vnum < lowVnum;++kIter );

	//Once there, we need to circulate until we hit the end of the range, adding each kit within...
	for( ;kIter != MyKits.end() && (*kIter)->vnum <= highVnum;++kIter )
		RangeKits.push_back( (*kIter) );
	//And that's it!
	return RangeKits;
}

ACMD(do_klist)
{
	int lo = 0, hi = 99, found = 0;

	TwoArguments(argument, buf, buf1);
	strcpy(buf2, "");

	if (*buf)
	{
		if (IsNumber(buf))
		{
			lo = atoi(buf);

			if (!*buf1)
			{
				hi = lo + 99;
			}
			else
			{
				hi = atoi(buf1);
			}
		}
		else if (!strn_cmp(buf, "all", strlen(buf)))
		{
			hi = 99999;
		}
		else
		{
			ch->Send("Usage: klist [{\"all\" | <start kit #>} [<end kit #>]]\r\n");
			return;
		}
	}

	if (lo < 0 || hi < 0 || lo > 99999 || hi > 99999 || lo > hi)
	{
		ch->Send("Usage: klist [{\"all\" | <start kit #>} [<end kit #>]]\r\n");
		return;
	}

	std::list< Kit * > RangeOfKits = KitManager::GetManager().GetRangeOfKits( lo, hi );
	for(std::list<Kit *>::iterator kIter = RangeOfKits.begin();kIter != RangeOfKits.end();++kIter)
	{
		sprintf(buf2 + strlen(buf2), "%5d. [%5d] %s\r\n", ++found, (*kIter)->vnum, (*kIter)->Name.c_str());
	}

	if (!found)
		ch->Send("No kits were found in those parameters.\r\n");

	else if (ch->desc)
		page_string(ch->desc, buf2, 1);
}

void kedit_disp_inventory_menu( Descriptor *d )
{
	d->Send("----- Inventory -----\r\n\r\n");
	get_char_cols(d->character);
	OLC_VAL(d) = 1;

	unsigned int i = 0;
	for( std::vector<KitItem>::iterator kIter = d->olc->kit->KitInventory.begin();kIter != d->olc->kit->KitInventory.end();++kIter )
	{
		d->Send("%s%2d%s) [%s%3d%s%%] [%s%6d%s] %s%s%s\r\n", grn, ++i, nrm, cyn, (*kIter).GetPercent(), nrm,
			grn, (*kIter).GetItemVnum(), nrm, yel, obj_proto[ (*kIter).GetItemRnum() ]->short_description, nrm );
	}
	d->SendRaw("\r\n");
	d->Send("%sA%s) Add Item\r\n"
			"%sD%s) Delete Item\r\n"
			"%sQ%s) Quit\r\n", grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE( d ) = KEDIT_INVENTORY;
}

void kedit_disp_menu(Descriptor *d)
{
	Kit *kit;

	kit = OLC_KIT(d);

	get_char_cols(d->character);

	sprintf(buf,
#if defined(CLEAR_SCREEN)
	        "\x1B[2J"
#endif
	        "--- Kit Number:  [%s%d%s]  (%d%s kit)\r\n"
	        " %sN%s) Name : %s%s\r\n",
	        cyn, OLC_KIT(d)->vnum, nrm, OLC_KNUM(d) + 1,
	        (OLC_KNUM(d) + 1 > 3 ? "th" : (OLC_KNUM(d) + 1 == 3 ? "rd" : (OLC_KNUM(d) + 1 == 2 ? "nd" : "st"))),
	        grn, nrm, yel, kit->Name.c_str()
	       );

	for(int i = 0; i < NUM_WEARS; ++i)
	{
		int obj_nr = (kit->KitItems[OLC_KNUM(d)][i].GetItemVnum()>0)
			? (real_object(kit->KitItems[OLC_KNUM(d)][i].GetItemVnum())) : (-1);
		sprintf(buf1,
		        "%s%2d%s) %s : [%s%3d%%%s] [%s%5d%s] %s%s\r\n",
		        grn, i, nrm, where[i],
		        cyn, kit->KitItems[OLC_KNUM(d)][i].GetPercent(), nrm,
		        cyn, OLC_KIT(d)->KitItems[OLC_KNUM(d)][i].GetItemVnum(), nrm,
		        yel, (obj_nr > -1) ? (obj_proto[obj_nr]->short_description) : ("Nothing.")
		       );
		strcat(buf, buf1);
	}

	sprintf(buf2,
	        " %sA%s) Previous kit on mob\r\n"
	        " %sB%s) Next kit on mob\r\n"
			" %sI%s) View inventory\r\n"
	        " %sD%s) Delete : %s%s\r\n"
	        //		" %sV%s) Vnum an object\r\n"
	        " %sQ%s) Quit\r\n"
	        "Enter your choice : ",
	        grn, nrm, grn, nrm, grn, nrm, grn, nrm, cyn, (kit->deleted ? "Yes" : "No"), grn, nrm
	       );

	strcat(buf, buf2);

	d->Send("%s", buf);

	OLC_MODE(d) = KEDIT_MAIN_MENU;

	return;
}

void kedit_parse(Descriptor *d, char *arg)
{
	int pos = -1, rpos = -1;

	// From Act.item.cpp, update KEDIT_WEAR #define's in olc.h if this changes
	int wear_bitvectors[] = {
	                            ITEM_WEAR_TAKE,
	                            ITEM_WEAR_TAKE,
	                            ITEM_WEAR_NECK,
	                            ITEM_WEAR_NECK,
	                            ITEM_WEAR_EAR,
	                            ITEM_WEAR_EAR,
	                            ITEM_WEAR_HEAD,
	                            ITEM_WEAR_ABOUT,
	                            ITEM_WEAR_BACK,
	                            ITEM_WEAR_BODY,
	                            ITEM_WEAR_SHOULDERS,
	                            ITEM_WEAR_ARMS,
	                            ITEM_WEAR_WAIST,
	                            ITEM_WEAR_WRIST,
	                            ITEM_WEAR_WRIST,
	                            ITEM_WEAR_HANDS,
	                            ITEM_WEAR_FINGER,
	                            ITEM_WEAR_FINGER,
	                            ITEM_WEAR_WIELD,
	                            ITEM_WEAR_SHIELD,
	                            ITEM_WEAR_LEGS,
	                            ITEM_WEAR_FEET
	                        };

	switch(OLC_MODE(d))
	{
		case KEDIT_MAIN_MENU:
			switch(*arg)
			{
				case 'Q':
				case 'q':
					if (OLC_VAL(d))
					{
						d->Send("Do you wish to save this kit? : ");
						OLC_MODE(d) = KEDIT_CONFIRM_SAVESTRING;
					}
					else
						cleanup_olc(d, CLEANUP_ALL);

					return;

				case 'N':
				case 'n':
					d->Send("Enter kit name:-\r\n]");
					OLC_MODE(d) = KEDIT_NAME;
					return;
				case 'I':
				case 'i':
					kedit_disp_inventory_menu(d);
					OLC_MODE(d) = KEDIT_INVENTORY;
					return;
				case 'A':
				case 'a':
					if (OLC_KNUM(d) == 0)
					{
						OLC_KNUM(d) = NUM_OF_KITS - 1;
						for(int i = (OLC_KNUM(d)+1) - d->olc->kit->KitItems.size();i > 0;--i)
							d->olc->kit->AppendBlankKit();
					}
					else
						OLC_KNUM(d) -= 1;

					break;

				case 'B':
				case 'b':
					if (OLC_KNUM(d) == NUM_OF_KITS - 1)
						OLC_KNUM(d) = 0;
					else
					{
						OLC_KNUM(d) += 1;
						//Check to see if the kit even exists
						if( (int)d->olc->kit->KitItems.size() < (OLC_KNUM(d)+1) )
							d->olc->kit->AppendBlankKit();//If not, we tack on a new one.
					}

					break;

				case 'D':
				case 'd':
					//Galnor - 3-29-2005 Toggle delete for kit.
					if(OLC_KIT(d)->deleted)
					{
						d->Send("This kit will no longer be deleted when the game reboots.\r\n");
						OLC_KIT(d)->deleted = false;
					}
					else
					{
						d->Send("This kit will be deleted when the game reboots.\r\n");
						OLC_KIT(d)->deleted = true;
					}

					OLC_VAL(d) = 1;

					break;

				default:
				{
					if (IsNumber(arg))
					{
						pos = atoi(arg);

						if (pos < 0 || pos >= NUM_WEARS)
						{
							d->Send("Invalid position!\r\n");
							break;
						}
						int rnum = OLC_KIT(d)->KitItems[OLC_KNUM(d)][pos].GetItemRnum();
						sprintf(buf,
						        "%s : [%s%3d%%%s] [%s%5d%s] %s%s%s\r\n",
						        equipment_types[pos],
						        cyn, OLC_KIT(d)->KitItems[OLC_KNUM(d)][pos].GetPercent(), nrm,
						        cyn, OLC_KIT(d)->KitItems[OLC_KNUM(d)][pos].GetItemVnum(), nrm,
						        yel, (rnum >= 0
								? obj_proto[rnum]->short_description : "Nothing."), nrm
						       );

						d->Send("%s", buf);
						d->Send("Enter object vnum (0 for Nothing) : ");

						// WARNING:  The below is a hack.  It saves (NUM_WEARS * 3) + 3 lines of code at the minimum.
						// Well, not + 3 anymore because of these two comments and the one-line hack.  :p  Serai.
						OLC_MODE(d) = pos + 3;
						OLC_POS(d) = pos;

						return;
					}
					// Will break out to the kedit_disp_menu() at the end of kedit_parse()
					d->Send("Invalid option!\r\n");
					break;
				}
			}
			break;

		case KEDIT_ITEM_VNUM_INVENTORY:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->Send("Your input must be numeric or 'Q' to quit.\r\nTry again : ");
				return;
			}
			if( toupper( arg[0] ) == 'Q' )
			{
				d->olc->value = 0;
				kedit_disp_inventory_menu( d );
				return;
			}
			int iVnum = atoi( arg );
			if( real_object( iVnum ) == -1 )
			{
				d->Send("That item does not exist.\r\nTry again: ");
				return;
			}
			d->olc->value2 = iVnum;
			d->Send("Percent chance to load object : ");
			OLC_MODE( d ) = KEDIT_ITEM_PERCENT;
			return;
		}
		case KEDIT_ADD_ITEM_POS:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->Send("Your input must be numeric or 'Q' to quit.\r\nTry again : ");
				return;
			}
			if( toupper( arg[0] ) == 'Q' )
			{
				kedit_disp_inventory_menu( d );
				return;
			}
			d->olc->pos = -1;
			int vIndex = atoi(arg)-1;
			if( vIndex >= d->olc->kit->KitInventory.size() )
				vIndex = d->olc->kit->KitInventory.size();
			if( vIndex < 0 )
				vIndex = 0;
			d->olc->value = vIndex;
			d->Send("Enter the vnum of the item you'd like to load : ");
			OLC_MODE( d ) = KEDIT_ITEM_VNUM_INVENTORY;
			return;
		}
		case KEDIT_DELETE_ITEM:
		{
			if( !MiscUtil::isNumber( arg ) )
			{
				d->Send("Your input must be numeric or 'Q' to quit.\r\nTry again : ");
				return;
			}
			if( toupper( arg[0] ) == 'Q' )
			{
				kedit_disp_inventory_menu( d );
				return;
			}
			int vIndex = atoi(arg)-1;
			if( vIndex >= d->olc->kit->KitInventory.size() || vIndex < 0 )
			{
				d->Send("That item isn't on the list.\r\nTry again, or choose 'Q' to quit : ");
				return;
			}
			d->olc->kit->KitInventory.erase( d->olc->kit->KitInventory.begin() + vIndex );
			kedit_disp_inventory_menu( d );
			
		}
		case KEDIT_INVENTORY:
		{
			if( toupper(arg[0]) == 'A' ) {
				d->Send("Where on thie list do you want to place this item : ");
				OLC_MODE( d ) = KEDIT_ADD_ITEM_POS;
				return;
			}
			else if( toupper(arg[0]) == 'D' ) {
				d->Send("Which item would you like to delete : ");
				OLC_MODE( d ) = KEDIT_DELETE_ITEM;
				return;
			}
			else if( toupper(arg[0]) == 'Q' ) {
				kedit_disp_menu( d );
				OLC_MODE( d ) = KEDIT_MAIN_MENU;
				return;
			}
			else {
				d->Send("Invalid option.\r\nTry again : ");
				return;
			}
			break;
		}

		case KEDIT_ITEM_PERCENT:
			if (!IsNumber(arg))
			{
				d->Send("Must be a numeric value, try again : ");
				return;
			}

			pos = atoi(arg);

			if (pos < 0 || pos > 100)
			{
				d->Send("Must be from 0 to 100 : ");
				return;
			}

			if( d->olc->pos != -1 ) {
				if (OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].GetPercent() != pos)
				{
					OLC_VAL(d) = 1;
					OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].SetPercent(pos);
				}
			}
			else {
				int index;
				if( OLC_KIT(d)->KitInventory.empty() )
					index = 0;
				else
					index = d->olc->value;
				KitItem ki = KitItem(-1, d->olc->value2, pos, -1 );
				if( index >= OLC_KIT(d)->KitInventory.size() )
					OLC_KIT(d)->KitInventory.push_back( ki );
				else
					V_InsertBefore( OLC_KIT(d)->KitInventory, ki, index );
				kedit_disp_inventory_menu( d );
				return;
			}

			break;

		case KEDIT_CONFIRM_SAVESTRING:
			switch(UPPER(*arg))
			{
				case 'Y':
				case 'y':
					d->olc->kit->Save();
					kedit_save_internally(d);
					/* fall through */

				case 'N':
				case 'n':
					cleanup_olc(d, CLEANUP_ALL);
					return;

				default:
					d->Send("Invalid option!\r\nDo you wish to save this kit? : ");
					return;
			}

			break;

		case KEDIT_NAME:
			OLC_VAL(d) = 1;
			if (strlen(arg) > MAX_KIT_NAME)
				arg[MAX_KIT_NAME - 1] = '\0';

			OLC_KIT(d)->Name = ((arg && *arg) ? arg : "undefined");

			break;

		case KEDIT_WEAR_LIGHT:
		case KEDIT_WEAR_HOLD:
		case KEDIT_WEAR_NECK_1:
		case KEDIT_WEAR_NECK_2:
		case KEDIT_WEAR_EAR_1:
		case KEDIT_WEAR_EAR_2:
		case KEDIT_WEAR_HEAD:
		case KEDIT_WEAR_ABOUT:
		case KEDIT_WEAR_BACK:
		case KEDIT_WEAR_BODY:
		case KEDIT_WEAR_SHOULDERS:
		case KEDIT_WEAR_ARMS:
		case KEDIT_WEAR_WAIST:
		case KEDIT_WEAR_WRIST_R:
		case KEDIT_WEAR_WRIST_L:
		case KEDIT_WEAR_HANDS:
		case KEDIT_WEAR_FINGER_R:
		case KEDIT_WEAR_FINGER_L:
		case KEDIT_WEAR_WIELD:
		case KEDIT_WEAR_SHIELD:
		case KEDIT_WEAR_LEGS:
		case KEDIT_WEAR_FEET:
			if (!IsNumber(arg))
			{
				d->Send("Must be a numeric value, try again : ");
				return;
			}

			// WARNING:  The below is the same hack, saving lots more work.  Serai.
			// Serai - moved it up to the main menu handler, still a hack though
			//		OLC_POS(d) = OLC_MODE(d) - 3;

			if ( (pos = atoi(arg)) == 0)
			{
				OLC_VAL(d) = 1;
				OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].SetItemVnum(NOTHING);
				OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].SetPercent(0);
				break;
			}

			rpos = pos;

			if ((pos = real_object(pos)) >= 0)
			{
				if (!CAN_WEAR(obj_proto[pos], wear_bitvectors[OLC_POS(d)]))
				{
					d->Send("That object can't be worn in this position, try again : ");
					return;
				}

				if (OLC_MODE(d) == KEDIT_WEAR_LIGHT)
				{
					if (GET_OBJ_TYPE(obj_proto[pos]) != ITEM_LIGHT)
					{
						d->Send("That object can't be used as a light, try again : ");
						return;
					}
				}

				if (OLC_MODE(d) == KEDIT_WEAR_HOLD)
				{
					// From Act.item.cpp
					if (!CAN_WEAR(obj_proto[pos], ITEM_WEAR_HOLD) && GET_OBJ_TYPE(obj_proto[pos]) != ITEM_POTION)
					{
						d->Send("That object can't be held, try again : ");
						return;
					}
				}

				// If it changes, mark the kit as changed for save-on-exit prompt.
				if (OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].GetItemVnum() != rpos)
				{
					OLC_VAL(d) = 1;
					OLC_KIT(d)->KitItems[OLC_KNUM(d)][OLC_POS(d)].SetItemVnum(rpos);
				}

				// Lets load items on percent
				OLC_MODE(d) = KEDIT_ITEM_PERCENT;
				d->Send("Percent chance to load object : ");

				return;
			}
			else
			{
				d->Send("That object does not exist, try again : ");
				return;
			}

			break;

		default:
			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE,
			       "SYSERR: OLC: Reached default case in kedit_parse() ... Add a new eq position maybe?");
			return;
	}

	kedit_disp_menu(d);

	return;
}

//Originally written by Serai( 2004 )
//Rewritten by Galnor( 04/21/2009 )
void kedit_save_internally(Descriptor *d)
{
	Kit *OldKit;

	if( (OldKit = KitManager::GetManager().GetKitByVnum( d->olc->kit->vnum )) == NULL )
	{//Not in the list.
		OldKit = new Kit();
		OldKit->CopyFrom( d->olc->kit );
		KitManager::GetManager().AddKit( OldKit );
	}
	else
	{//Already in the list.
		OldKit->CopyFrom( d->olc->kit );
	}
}

void Kit::Save()
{
	std::stringstream Query;

	if( this->in_db )
		Query << "UPDATE kits SET name='" << sql::escapeString(this->Name) << "' WHERE vnum='" << this->vnum << "';";
	else
		Query << "INSERT INTO kits (vnum,name) VALUES('" << this->vnum << "','" << sql::escapeString(this->Name) << "');";

	try {
		gameDatabase->sendRawQuery(Query.str());
	} catch (sql::QueryException e) {
		MudLog(BRF, LVL_APPR, TRUE, "An error occurred when saving kit #%d : %s", this->vnum, e.message.c_str());
		return;
	}

//	Query.str("");
//	Query << "DELETE FROM kitItem WHERE kit_vnum='" << this->vnum << "';";
	//Delete all pre-existing kit items...
//	try {
//		gameDatabase->sendRawQuery(Query.str());
//	} catch( sql::QueryException e ) {
//		MudLog(BRF, LVL_APPR, TRUE, "An error occurred when saving kit #%d : %s", this->vnum, e.Message.c_str());
//		return;
//	}

	//Save all the kit items now...
	for(unsigned int i = 0;i < this->KitItems.size();++i)
	{
		for(int p = 0;p < NUM_WEARS;++p)
		{
			if( this->KitItems[i][p].GetItemVnum() == -1 ) continue;
			Query.str("");

			//Self explanitory, really. Different queries depending on whether or not they are in the DB already.
			if( this->KitItems[i][p].IsInDB() )
			{
				Query << "UPDATE kitItem SET kit_vnum='" << this->vnum << "',body_position='"
					<< p << "',obj_vnum='" << this->KitItems[i][p].GetItemVnum()
					<< "',probability='" << this->KitItems[i][p].GetPercent() << "',hierarchy='" << i
					<< "' WHERE id='" << this->KitItems[i][p].GetDBID() << "';";
			}
			else
			{
				Query << "INSERT INTO kitItem (kit_vnum,body_position,obj_vnum,probability,hierarchy) VALUES('"
					<< this->vnum << "','" << p << "','" << this->KitItems[i][p].GetItemVnum() << "','"
					<< this->KitItems[i][p].GetPercent() << "','" << i << "');";
			}

			try {
				gameDatabase->sendRawQuery(Query.str());
			} catch( sql::QueryException e ) {
				MudLog(BRF, LVL_APPR, TRUE, "An error occurred when saving kit #%d : %s", this->vnum, e.message.c_str());
				return;
			}
			//IMPORTANT. We must update the kit item's database ID. Otherwise it will keep trying to insert!
			this->KitItems[i][p].SetDBID( gameDatabase->lastInsertID() );
		}
	}
	for(unsigned int i = 0;i < this->KitInventory.size();++i)
	{
		KitItem& ki = this->KitInventory[i];
		if( ki.GetItemVnum() == -1 ) continue;
		Query.str("");

		//Self explanitory, really. Different queries depending on whether or not they are in the DB already.
		if( ki.IsInDB() )
		{
			Query << "UPDATE kitItem SET kit_vnum='" << this->vnum << "',body_position='-1',"
				<< "obj_vnum='" << ki.GetItemVnum()
				<< "',probability='" << ki.GetPercent() << "',hierarchy='" << i
				<< "' WHERE id='" << ki.GetDBID() << "';";
		}
		else
		{
			Query << "INSERT INTO kitItem (kit_vnum,body_position,obj_vnum,probability,hierarchy) VALUES('"
				<< this->vnum << "','-1','" << ki.GetItemVnum() << "','"
				<< ki.GetPercent() << "','" << i << "');";
		}

		try {
			gameDatabase->sendRawQuery(Query.str());
		} catch( sql::QueryException e ) {
			MudLog(BRF, LVL_APPR, TRUE, "An error occurred when saving kit #%d : %s", this->vnum, e.message.c_str());
			return;
		}
		ki.SetDBID( gameDatabase->lastInsertID() );
	}
	in_db = true;
}

void kedit_setup_existing(Descriptor *d, Kit *kit)
{
	OLC_NUM(d) = kit->vnum;
	OLC_KNUM(d) = 0;
	OLC_VAL(d) = 0;

	OLC_KIT(d) = new Kit();
	//CREATE(OLC_KIT(d), Kit, 1); Use contructor now.

	OLC_KIT(d)->Name	= kit->Name;
	OLC_KIT(d)->vnum	= kit->vnum;
	OLC_KIT(d)->deleted	= kit->deleted;
	OLC_KIT(d)->in_db	= kit->in_db;

	d->olc->kit->KitItems = kit->KitItems;
	d->olc->kit->KitInventory = kit->KitInventory;

	kedit_disp_menu(d);

	return;
}

void kedit_setup_new(Descriptor *d)
{
	OLC_KNUM(d) = 0;
	OLC_VAL(d) = 0;

	OLC_KIT(d) = new Kit();
	d->olc->kit->KitItems.resize(NUM_OF_KITS);
	for(int i = 0;i < NUM_OF_KITS;++i)
		d->olc->kit->KitItems[i].resize(NUM_WEARS);
	//CREATE(OLC_KIT(d), Kit, 1);

	OLC_KIT(d)->Name = "an unfinished kit";
	OLC_KIT(d)->vnum = OLC_NUM(d);

	kedit_disp_menu(d);
	return;
}

#undef OLC_KNUM
