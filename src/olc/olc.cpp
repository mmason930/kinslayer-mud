/***************************************************************************
*  OasisOLC - olc.c 		                                           *
*    				                                           *
*  Copyright 1996 Harvey Gilpin.                                          *
***************************************************************************/

#define _OASIS_OLC_

#include "../conf.h"
#include "../comm.h"
#include "../spells.h"
#include "../screen.h"
#include "../shop.h"
#include "../stats.h"
#include "../mobs.h"
#include "../js/js.h"
#include "../auction.h"

#include "../StringUtil.h"
#include "../Descriptor.h"
#include "../UserEmailAddress.h"
#include "../rooms/Room.h"

#include "../commands/infrastructure/Social.h"

#include <boost/regex.hpp>

//External data structures.
extern struct Index *obj_index;

extern const int rev_dir[];
extern int top_of_objt;
extern const char *dirs[];
extern Descriptor *descriptor_list;

//External functions.
extern void kedit_setup_existing( Descriptor *, Kit * );
extern void kedit_setup_new( Descriptor * );
void zedit_setup( Descriptor *d, int room_num );
void zedit_new_zone( Character *ch, int new_zone );
void medit_setup_new( Descriptor *d );
void medit_setup_existing( Descriptor *d, int rmob_num );
void redit_setup_new( Descriptor *d );
void redit_setup_existing( Descriptor *d, int rroom_num );
void redit_save_internally( Descriptor *d );
void oedit_setup_new( Descriptor *d );
void oedit_setup_existing( Descriptor *d, int robj_num );
void sedit_setup_new( Descriptor *d );
void sedit_setup_existing( Descriptor *d, int robj_num );
void sedit_save_to_disk();
extern int real_shop( int vnum );
FILE *file;

struct olc_save_info *olc_save_list = NULL;
char *nrm, *grn, *cyn, *yel, *bld, *mag, *red, *blu;

//Internal function prototypes.
int can_edit_zone( Character *ch, int number );
void olc_saveinfo( Character *ch );

//Global string constants.
const char *save_info_msg[ 9 ] =
    {"Rooms", "Objects", "Zone info",
     "Mobiles", "Shops", "Triggers", "Help", "Actions", "Kits"
    };

//Internal data structures.
struct olc_scmd_data
{
	const char *text;
	int con_type;
};

struct olc_scmd_data olc_scmd_info[] =
    {
	    {"room", CON_REDIT},
	    {"object", CON_OEDIT},
	    {"room", CON_ZEDIT},
	    {"mobile", CON_MEDIT},
	    {"shop", CON_SEDIT},
	    {"action", CON_AEDIT},
	    {"kit", CON_KEDIT},
	    {"cedit", CON_CEDIT},
    };

const char *atTypeStr[4] =
{
	"WEAPONS",
	"ARMOR",
	"MISC",
	"\n"
};

AuctionData::AuctionData()
{
	this->page = 1;
	this->sellDuration = 0;
	this->itemToSell = NULL;
	buyoutPrice = 0;
	startingPrice = 0;
	selectedItem = -1;
	bidRequest=0;
}
AuctionData::~AuctionData()
{
}

void AuctionData::SetItemToSell( Object *o )
{
	itemToSell = o;
}
Object *AuctionData::GetItemToSell()
{
	return itemToSell;
}

int AuctionData::GetSellDuration()
{
	return sellDuration;
}
void AuctionData::SetSellDuration( const int duration )
{
	sellDuration = duration;
}
int AuctionData::GetBuyoutPrice( const char t )
{
	if( toupper(t) == 'G' )
		return (CalcGold(buyoutPrice));
	if( toupper(t) == 'S' )
		return (CalcSilver(buyoutPrice));
	if( toupper(t) == 'C' )
		return (CalcCopper(buyoutPrice));
	return buyoutPrice;
}
void AuctionData::SetBuyoutPrice( const unsigned int bp, const char t )
{
	if( toupper(t) == 'G' )
	{
		buyoutPrice -= CalcGold(buyoutPrice)*10000;
		buyoutPrice += bp*10000;
	}
	else if( toupper(t) == 'S' )
	{
		buyoutPrice -= CalcSilver(buyoutPrice)*100;
		buyoutPrice += bp*100;
	}
	else if( toupper(t) == 'C' )
	{
		buyoutPrice -= CalcCopper(buyoutPrice);
		buyoutPrice += bp;
	}
	else
		buyoutPrice = bp;
}
int AuctionData::GetStartingPrice( const char t )
{
	if( toupper(t) == 'G' )
		return (CalcGold(startingPrice));
	if( toupper(t) == 'S' )
		return (CalcSilver(startingPrice));
	if( toupper(t) == 'C' )
		return (CalcCopper(startingPrice));
	return startingPrice;
}
void AuctionData::SetStartingPrice( const unsigned int sp, const char t )
{
	if( toupper(t) == 'G' )
	{
		startingPrice -= CalcGold(startingPrice)*10000;
		startingPrice += sp*10000;
	}
	else if( toupper(t) == 'S' )
	{
		startingPrice -= CalcSilver(startingPrice)*100;
		startingPrice += sp*100;
	}
	else if( toupper(t) == 'C' )
	{
		startingPrice -= CalcCopper(startingPrice);
		startingPrice += sp;
	}
	else
		startingPrice =sp;
}

void AuctionData::SetSearchTerm( const std::string &st )
{
	this->SearchTerm = st;
}
const std::string &AuctionData::GetSearchTerm()
{
	return (this->SearchTerm);
}
int AuctionData::GetPage()
{
	return page;
}
int AuctionData::NumPages()
{
	return ((this->aiCache.size() / AuctionManager::ItemsPerPage)+1);
}
void AuctionData::SetPage( const int _page )
{
	page = _page;
}
void AuctionData::ClearCache()
{
	while( !aiCache.empty() )
	{
		delete aiCache.back();
		aiCache.pop_back();
	}
}
void AuctionData::ClearSellData()
{
	this->itemToSell	= (0);
	this->sellDuration	= (0);
	this->buyoutPrice	= (0);
	this->startingPrice	= (0);
}
void AuctionData::SetSelectedItem( const int _nr )
{
	selectedItem = _nr;
}
AuctionItem *AuctionData::GetSelectedItem()
{
	return this->aiCache[selectedItem];
}

OLC::OLC()
{
	this->mode				= 0;
	this->zone_num			= 0;
	this->number			= 0;
	this->value				= 0;
	this->value2			= 0;
	this->pos				= 0;
	this->kit_num			= 0;
	this->script_mode		= 0;
	this->trigger_position	= 0;
	this->item_type			= 0;
	this->mob				= NULL;
	this->room				= NULL;
	this->obj				= NULL;
	this->warrant			= NULL;
	this->clan				= NULL;
	this->zone				= NULL;
	this->shop				= NULL;
	this->config			= NULL;
	this->desc				= NULL;
	this->kit				= NULL;
	this->action			= NULL;
	this->storage			= NULL;
	this->sg_index_ptr		= NULL;
	this->statgroup			= NULL;
	this->sg_race			= -1;
	this->sg_class			= -1;
	this->sg_sex			= -1;
	this->buf				= (NULL);
	this->auction			= (NULL);
	this->auction_data		= (NULL);
	this->userEmailAddress	= NULL;
	this->jsTrig			= (NULL);
//	this->jsScripts			= (NULL);
}

/*------------------------------------------------------------*/

CommandHandler  do_myzones  = DEFINE_COMMAND
{
	int i = 0;
	Zone *zone;
	buf[0] = '\0';

	if ( GET_LEVEL( ch ) > LVL_GRGOD )
	{
		ch->send( "You have access to all zones.\r\n" );
		return ;
	}

	for ( i = 0;(zone=ZoneManager::GetManager().GetZoneByRnum(i))!=NULL;++i )
	{//For each zone in the manager
		if ( zone->CanEdit(ch) )
		{//If ch can edit the zone
			sprintf( buf + strlen( buf ), "%d. %s%s%s\r\n", zone->getVnum(), COLOR_GREEN( ch, CL_COMPLETE ),
				zone->getName().c_str(), COLOR_NORMAL( ch, CL_COMPLETE ) );
		}
	}

	if ( *buf == '\0')//Buffer is empty, meaning no zones are to be displayed.
		ch->send( "You have no zones assigned to you.\r\n" );
	else
		ch->send( "You are assigned to the following zones:\r\n%s", buf );
};

/*
 * Exported command do_olc function.
 *
 * This function is the OLC interface.  It deals with all the
 * generic OLC stuff, then passes control to the sub-olc sections.
 */

CommandHandler  do_olc  = DEFINE_COMMAND
{
	int num = -1, save = 0, real_num;
	Descriptor *d;
	Kit *kit;

	/*
	 * No screwing around as a mobile.
	 */

	if ( IS_NPC( ch ) )
		return ;

	if ( subcmd == SCMD_OLC_SAVEINFO )
	{
		olc_saveinfo( ch );
		return ;
	}

	/*
	 * Parse any arguments.
	 */

	TwoArguments( argument, buf1, buf2 );

	if ( !*buf1 )
	{		/* No argument given. */
		switch ( subcmd )
		{
		case SCMD_OLC_ZEDIT:
		case SCMD_OLC_REDIT:
			num = ch->in_room->getVnum();
			break;
		case SCMD_OLC_OEDIT:
		case SCMD_OLC_MEDIT:
		case SCMD_OLC_SEDIT:
		case SCMD_OLC_KEDIT:
			ch->send( "Specify a %s VNUM to edit.\r\n", olc_scmd_info[ subcmd ].text );
			return ;
		case SCMD_OLC_AEDIT:
			ch->send( "Specify an action to edit.\r\n" );
			return;
		}
	}

	else if ( !isdigit( *buf1 ) )
	{
		if ( strn_cmp( "save", buf1, 4 ) == 0 )
		{
			if(subcmd == SCMD_OLC_SEDIT) {

				ch->send("Shops are saved immediately upon exiting the editor. No need to save manually.\r\n");
				//return;
			}
			if(( subcmd == SCMD_OLC_AEDIT ) || (subcmd == SCMD_OLC_SGEDIT))
			{
				save = 1;
				num = 0;
			}
			else if ( !*buf2 )
			{
				ch->send( "Save which zone?\r\n" );
				return ;
			}
			else
			{
				save = 1;
				num = atoi( buf2 ) * 100;
			}
		}

		else if ( ( subcmd == SCMD_OLC_AEDIT ) || ( subcmd == SCMD_OLC_SGEDIT ) )
			num = 0;

		else if ( subcmd == SCMD_OLC_ZEDIT && GET_LEVEL( ch ) >= LVL_GRGOD )
		{
			if ( ( strn_cmp( "new", buf1, 3 ) == 0 ) && *buf2 )
				zedit_new_zone( ch, atoi( buf2 ) );
			else
				ch->send( "Specify a new zone number.\r\n" );
			return ;
		}
		else
		{
			ch->send( "Yikes!  Stop that, someone will get hurt!\r\n" );
			return ;
		}
	}

	/*
	 * If a numeric argument was given, get it.
	 */

	if ( ( num == -1 ) && ( ( subcmd != SCMD_OLC_AEDIT ) || ( subcmd != SCMD_OLC_SGEDIT ) ) )
		num = atoi( buf1 );

	/*
	 * Check that whatever it is isn't already being edited.
	 */

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( d->connected == olc_scmd_info[ subcmd ].con_type )
		{
			if ( d->olc && OLC_NUM( d ) == num )
			{
				if ( subcmd == SCMD_OLC_AEDIT )
					sprintf( buf, "actions are already being editted by %s.\r\n",
					         ( CAN_SEE( ch, d->character ) ? GET_NAME( d->character ) : "someone" ) );
				else if ( subcmd == SCMD_OLC_SGEDIT )
					sprintf( buf, "Stat tempates are already being editted by %s.\r\n",
					         ( CAN_SEE( ch, d->character ) ? GET_NAME( d->character ) : "someone" ) );
				else
					sprintf( buf, "That %s is currently being edited by %s.\r\n",
					         olc_scmd_info[ subcmd ].text, GET_NAME( d->character ) );
				ch->send( buf, ch );

				return ;
			}
		}
	}

	d = ch->desc;
	d->olc = new OLC();
	Zone *zone = NULL;

	/*
	 * Find the zone (or help rnum).
	 */
	if ( subcmd != SCMD_OLC_AEDIT )
	{
		if ( ( zone = ZoneManager::GetManager().GetZoneByRoomVnum(num) ) == NULL )
		{
			ch->send( "Sorry, there is no zone for that number!\r\n" );
			cleanup_olc(d, CLEANUP_ALL);
			return ;
		}
	}
	if( zone )
		OLC_ZNUM(d) = zone->GetRnum();

	if ( GET_LEVEL( ch ) < LVL_GOD )
	{
		if(subcmd != SCMD_OLC_SGEDIT)
		{
			if ( ( subcmd != SCMD_OLC_AEDIT ) && (!zone->CanEdit(ch)) )
			{
				ch->send( "You do not have permission to edit this zone.\r\n" );
				MudLog( NRM, MAX( LVL_GRGOD, GET_INVIS_LEV( ch ) ), TRUE,
				        "%s tried editing zone %d without permission.", GET_NAME( ch ), zone->getVnum() );
				cleanup_olc(d, CLEANUP_ALL);
				return ;
			}
		}
	}

	if ( save )
	{
		const char * type = NULL;

		switch ( subcmd )
		{
		case SCMD_OLC_AEDIT:
			type = "action";
			break;
		case SCMD_OLC_REDIT:
			type = "room";
			break;
		case SCMD_OLC_ZEDIT:
			type = "zone";
			break;
		case SCMD_OLC_SEDIT:
			type = "shop";
			break;
		case SCMD_OLC_MEDIT:
			type = "mobile";
			break;
		case SCMD_OLC_OEDIT:
			type = "object";
			break;
		case SCMD_OLC_SGEDIT:
			type = "StatGroup";
			break;
		}

		if ( !type )
		{
			ch->send( "Oops, I forgot what you wanted to save.\r\n" );
			return ;
		}
		if ( subcmd == SCMD_OLC_AEDIT )
		{
			ch->send( "Saving all actions.\r\n" );
			MudLog( NRM, MAX( LVL_BUILDER, GET_INVIS_LEV( ch ) ), TRUE, "OLC: %s saves all actions.", GET_NAME( ch ) );
		}
		else
		{
			ch->send( "Saving all %ss in zone %d.\r\n", type, zone->getVnum() );

			MudLog( CMP, MAX( LVL_BUILDER, GET_INVIS_LEV( ch ) ), TRUE,
			        "OLC: %s saves %s info for zone %d.", GET_NAME( ch ), type, zone->getVnum() );
		}
		switch ( subcmd )
		{

		case SCMD_OLC_REDIT:
			if ( zone->IsDeleted() )
			{
				d->send( "This zone is set to delete after the next reboot. Not saving...\r\n" );
				break;
			}
			redit_save_to_disk( OLC_ZNUM( d ) );
			break;
		case SCMD_OLC_ZEDIT:
			ZoneManager::GetManager().GetZoneByRnum( OLC_ZNUM(d) )->save();
			break;
		case SCMD_OLC_OEDIT:
			oedit_save_to_disk( OLC_ZNUM( d ) );
			break;
		case SCMD_OLC_MEDIT:
			MobManager::GetManager().SavePrototypes(ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d))->getVnum());
			break;
		}
		cleanup_olc( d, CLEANUP_ALL );
		return ;

	}

	if ( subcmd != SCMD_OLC_AEDIT )
		OLC_NUM( d ) = num;
	else
	{
		OLC_NUM( d ) = 0;
		OLC_STORAGE( d ) = str_dup( buf1 );

		Social *social = CommandUtil::get()->getSocialByAbbreviation(OLC_STORAGE(d));

		if(!social)
		{
			if(CommandUtil::get()->getCommandByName(OLC_STORAGE(d)))
			{
				cleanup_olc( d, CLEANUP_ALL );
				ch->send( "That command already exists.\r\n" );
				return;
			}

			sprintf( buf, "Do you wish to add the '%s' action? ", OLC_STORAGE( d ) );
			ch->send( buf, ch );
			OLC_MODE( d ) = AEDIT_CONFIRM_ADD;
		}

		else
		{
			sprintf( buf, "Do you wish to edit the '%s' action? ", social->command );
			ch->send( buf, ch );
			OLC_MODE( d ) = AEDIT_CONFIRM_EDIT;
		}
	}

	/*
	 * Steal player's descriptor start up subcommands.
	 */
	switch ( subcmd )
	{
	case SCMD_OLC_KEDIT:
		if ( ( kit = KitManager::GetManager().GetKitByVnum( num ) ) != NULL )
			kedit_setup_existing( d, kit );
		else
			kedit_setup_new( d );
		STATE( d ) = CON_KEDIT;
		break;
	case SCMD_OLC_REDIT:
		if ( ( real_num = real_room( num ) ) >= 0 )
			redit_setup_existing( d, real_num );
		else
			redit_setup_new( d );

		STATE( d ) = CON_REDIT;
		break;
	case SCMD_OLC_ZEDIT:
		if ( ( real_num = real_room( num ) ) < 0 )
		{
			ch->send( "That room does not exist.\r\n" );
			cleanup_olc(d, CLEANUP_ALL);
			return ;
		}

		zedit_setup( d, real_num );
		STATE( d ) = CON_ZEDIT;
		break;
	case SCMD_OLC_MEDIT:
		if ( ( real_num = MobManager::GetManager().RealMobile( num ) ) < 0 )
			medit_setup_new( d );
		else
			medit_setup_existing( d, real_num );

		STATE( d ) = CON_MEDIT;
		break;
	case SCMD_OLC_OEDIT:
		if ( ( real_num = real_object( num ) ) >= 0 )
			oedit_setup_existing( d, real_num );
		else
			oedit_setup_new( d );

		STATE( d ) = CON_OEDIT;
		break;
	case SCMD_OLC_SEDIT:
		if ( ( real_num = real_shop( num ) ) >= 0 )
			sedit_setup_existing( d, real_num );
		else
			sedit_setup_new( d );
		STATE( d ) = CON_SEDIT;
		break;

	case SCMD_OLC_SGEDIT:
		STATE( d ) = CON_SGEDIT;
		//sgedit_setup_existing( d, argument );
		break;

	case SCMD_OLC_AEDIT:
		STATE( d ) = CON_AEDIT;
		break;
	}

	Act( "$n starts using OLC.", TRUE, d->character, 0, 0, TO_ROOM );
	SET_BITK( PLR_FLAGS( ch ), Q_BIT(PLR_WRITING) );
};

/*------------------------------------------------------------*\
 Internal utilities
\*------------------------------------------------------------*/

void olc_saveinfo( Character *ch )
{
	struct olc_save_info * entry;

	if ( olc_save_list )
		ch->send( "The following OLC components need saving:-\r\n" );
	else
		ch->send( "The database is up to date.\r\n" );

	for ( entry = olc_save_list; entry; entry = entry->next )
	{
		if ( ( int ) entry->type == OLC_SAVE_HELP )
			sprintf( buf, " - Help Entries.\r\n" );
		else if ( ( int ) entry->type == OLC_SAVE_ACTION )
			sprintf( buf, " - Actions.\r\n" );
		else if ( ( int ) entry->type == OLC_SAVE_SG )
			sprintf( buf, " - Stat Group Templates.\r\n" );
		else if ( ( int ) entry->type == OLC_SAVE_WV)
			sprintf( buf, " - Weaves for weave %d.\r\n", entry->zone );
		else
			sprintf( buf, " - %s for zone %d.\r\n",
			         save_info_msg[ ( int ) entry->type ], entry->zone );
		ch->send( buf, ch );
	}
}

/*------------------------------------------------------------*\
 Exported utilities
\*------------------------------------------------------------*/

/*
 * Add an entry to the 'to be saved' list.
 */

void olc_add_to_save_list( int zone, sbyte type )
{
	struct olc_save_info * newt;

	/*
	 * Return if it's already in the list.
	 */

	for ( newt = olc_save_list; newt; newt = newt->next )
		if ( ( newt->zone == zone ) && ( newt->type == type ) )
			return ;

	newt = new olc_save_info;
	memset( newt, 0, sizeof( olc_save_info ) );
	newt->zone = zone;
	newt->type = type;
	newt->next = olc_save_list;
	olc_save_list = newt;
}

/*
 * Remove an entry from the 'to be saved' list.
 */

void olc_remove_from_save_list( int zone, sbyte type )
{
	struct olc_save_info **entry;
	struct olc_save_info *temp;

	for ( entry = &olc_save_list; *entry; entry = &( *entry ) ->next )
	{
		if ( ( ( *entry ) ->zone == zone ) && ( ( *entry ) ->type == type ) )
		{
			temp = *entry;
			*entry = temp->next;
			delete ( temp );
			return ;
		}
	}
}

/*
 * Set the colour std::string pointers for that which this char will
 * see at color level NRM.  Changing the entries here will change
 * the colour scheme throughout the OLC.
 */

void get_char_cols( Character *ch )
{
	nrm = const_cast< char * > COLOR_NORMAL  ( ch, CL_NORMAL );
	grn = const_cast< char * > COLOR_GREEN   ( ch, CL_NORMAL );
	cyn = const_cast< char * > COLOR_CYAN    ( ch, CL_NORMAL );
	yel = const_cast< char * > COLOR_YELLOW  ( ch, CL_NORMAL );
	bld = const_cast< char * > COLOR_BOLD    ( ch, CL_NORMAL );
	mag = const_cast< char * > COLOR_MAGENTA ( ch, CL_NORMAL );
	red = const_cast< char * > COLOR_RED     ( ch, CL_NORMAL );
	blu = const_cast< char * > COLOR_BLUE    ( ch, CL_NORMAL );
}

/*
 * This procedure removes the '\r\n' from a std::string so that it may be
 * saved to a file.  Use it only on buffers, not on the original
 * strings.
 */
void strip_string( char *buffer )
{
	char * ptr, *str;

	ptr = buffer;
	str = ptr;

	while ( ( *str = *ptr ) )
	{
		++str;
		++ptr;

		if ( *ptr == '\r' )
			++ptr;
	}
}

/*
 * This procdure frees up the strings and/or the structures
 * attatched to a descriptor, sets all flags back to how they
 * should be.
 */

void cleanup_olc( Descriptor *d, sbyte cleanup_type )
{
	if ( d->olc )
	{
		//Check for storage.
		if ( OLC_STORAGE( d ) )
			delete []( OLC_STORAGE( d ) );
		if( d->olc->buf != NULL )
			delete []d->olc->buf;

		//Check for a kit
		if ( OLC_KIT( d ) )
			delete ( OLC_KIT( d ) );

		//Check for a room.
		if ( OLC_ROOM( d ) )
		{
			switch ( cleanup_type )
			{
			case CLEANUP_ALL:
				d->olc->room->freeLiveRoom();
				break;
			case CLEANUP_STRUCTS:
				d->olc->room->freeLiveRoom();
				break;
			default:
				/* The caller has screwed up. */
				break;
			}
		}
		if( d->olc->auction_data )
			delete (d->olc->auction_data);
		if( d->olc->userEmailAddress )
			delete d->olc->userEmailAddress;
		while(d->olc->userEmailAddresses.empty() == false)
		{
			UserEmailAddress *userEmailAddress = d->olc->userEmailAddresses.front();
			delete userEmailAddress;
			d->olc->userEmailAddresses.pop_front();
		}
		//Check for an object.
		if ( OLC_OBJ( d ) )
		{
			d->olc->obj->FreeLiveObject();//No longer considered a 'prototype' because it has been replaced.
		}

		//Check for a mob.
		if ( OLC_MOB( d ) )
		{
			/*** All prototype sensitive data MUST be set to null before deleting. ***/
			d->olc->mob->UnsetMobPrototypeSpecifics();
			//medit_free_mobile() makes sure strings are not in the prototype.
			delete ( d->olc->mob );
		}

		//Check for a zone.
		if ( OLC_ZONE( d ) )
		{
			//cleanup_type is irrelevant here, delete[] () everything.
			delete ( d->olc->zone );
		}

		if ( OLC_CLAN( d ) )
		{
			//Delete the clan. As self explanatory as it gets...
			delete ( d->olc->clan );
		}

		if ( OLC_SG( d ) )
		{
			//Delete the SG. As self explanatory as it gets...
			delete ( d->olc->statgroup );
		}

		//Do NOT delete the warrant since we are not making temporaries.
		if ( OLC_WARRANT( d ) )
			OLC_WARRANT( d ) = NULL;

		//Delete the temporary warrant list.
		while ( d->olc->WarrantList.size() )
		{
			Warrant * temp;
			temp = *( d->olc->WarrantList.begin() );
			d->olc->WarrantList.remove( temp );
			delete temp;
		}

		//Check for a shop.
		if ( OLC_SHOP( d ) )
		{
			//free_shop doesn't perform sanity checks, we must be careful here.
			switch ( cleanup_type )
			{
					case CLEANUP_ALL:
					delete OLC_SHOP( d );
					break;
					case CLEANUP_STRUCTS:
					delete OLC_SHOP( d );
					break;
					default:
					/* The caller has screwed up. */
					break;
			}
		}

		/* Check for aedit stuff -- M. Scott */
		if ( OLC_ACTION( d ) )
		{
			switch ( cleanup_type )
			{
					case CLEANUP_ALL:
						delete d->olc->action;
						d->olc->action = 0;
					break;
					case CLEANUP_STRUCTS:
						delete ( d->olc->action );
					break;
					default:
						/* Caller has screwed up */
					break;
			}
		}



		//Restore descriptor playing status.
		if ( d->character )
		{
			REMOVE_BIT( PLR_FLAGS( d->character ), Q_BIT(PLR_WRITING) );

			if( STATE( d ) == CON_AUCTION )
				Act( "$n takes a break from the auctions.", TRUE, d->character, 0, 0, TO_ROOM );
			else
				Act( "$n stops using OLC.", TRUE, d->character, 0, 0, TO_ROOM );
			STATE( d ) = CON_PLAYING;
		}

		delete ( d->olc );
		d->olc = NULL;
	}
}

// These defines were harvested from....  zedit.c ?
#define ZCMD (zone_table[zone].cmd[cmd_no])
#define W_EXIT(room, num) (World[(room)]->dir_option[(num)])

CommandHandler  do_rlist  = DEFINE_COMMAND
{
	char arg1[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ];
	int r1 = 0, r2 = 0;
	bool found = false;

	if ( !argument )
	{
		ch->send( "Syntax: rstd::list <Bottom Room Number> <Top Room Number>\r\n" );
		return ;
	}

	TwoArguments( argument, arg1, arg2 );
	r1 = atoi( arg1 );
	r2 = atoi( arg2 );

	if ( r1 < 0 || r2 < 0 )
	{
		ch->send( "Both numbers must be above zero.\r\n" );
		return ;
	}

	if ( r2 < r1 )
	{
		ch->send( "The second room number must be greater than or equal to the first. %d %d, not %d %d\r\n", r2, r1, r1, r2 );
		return ;
	}

	for ( unsigned int i = 0;i < World.size();++i )
	{
		if (World[i]->getVnum() > r2)
			break;

		if (World[i]->getVnum() >= r1)
		{
			found = true;
			ch->send( "[%s%s%d%s] %s\r\n", COLOR_BOLD( ch, CL_COMPLETE ), COLOR_GREEN( ch, CL_COMPLETE ),
				World[i]->getVnum(), COLOR_NORMAL(ch, CL_COMPLETE), World[i]->getName());
		}
	}

	if ( !found )
		ch->send( "No rooms found within that range.\r\n" );

};

/*********************************************************************************
 *                                                                               *
 * 11/17/2009 - Galnor - Redoing this command to work more like the new and      *
 *               improved olist command, where we will be able to sort mobs      *
 *               by different attributes and get more information from the list  *
 *                                                                               *
 * Updated 02/21/2014 - Galnor - Allow keyword search, optimize sorting.         *
 *                                                                               *
 *********************************************************************************/
CommandHandler  do_mlist  = DEFINE_COMMAND
{
	if( !ch->desc )
		return;
	char arg1[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ], arg3[ MAX_INPUT_LENGTH ], sortType[MAX_INPUT_LENGTH], *keyword = NULL;
	int m1 = -1, m2 = -1, r1 = -1, r2 = -1;
	std::stringstream OutBuf;
	Character *Prototype = NULL;
	std::list<std::string> SortTypes;
	std::map<int,int> zoneLoads, openZoneLoads; // Mob Rnum to Mob Zone Load

	std::string strSortTypes = "exp lvl level hp ob pb db vnum kit gold zloads open";
	SortTypes = StringUtil::splitToContainer< std::list< std::string > >(strSortTypes, ' ');

	*arg1 = *arg2 = *arg3 = *sortType = '\0';

	if ( !argument )
	{
		ch->send( "Syntax: mlist [<Keyword> | <Bottom Room Number> <Top Room Number>] <Sort Type>\r\n" );
		return ;
	}

	TwoArguments( OneArgument( argument, arg1 ), arg2, arg3 );
	//Only one numeric boundary provided.
	if( (MiscUtil::isNumber(arg1) && !MiscUtil::isNumber(arg2)) || (MiscUtil::isNumber(arg2) && !MiscUtil::isNumber(arg1)) )
	{
		ch->send( "Syntax: mlist <Bottom Room Number> <Top Room Number> <Sort Type>\r\n" );
		return;
	}

	if( MiscUtil::isNumber(arg1) && MiscUtil::isNumber(arg2) )
	{
		m1 = atoi( arg1 );
		m2 = atoi( arg2 );

		if(m1 < 0)
		{
			ch->send("Bottom number must be zero or above.");
			return;
		}

		if(m2 < 0)
		{
			ch->send("Top number must be zero or above.");
			return;
		}

		if ( m2 < m1 )
		{
			ch->send( "The second mob number must be greater than or equal to the first. %d %d, not %d %d\r\n", m2, m1, m1, m2 );
			return;
		}

		strcpy(sortType, arg3);
	}
	else
	{//No boundary specified. Use keyword.
		m1 = m2 = -1;
		keyword = &arg1[0];
		strcpy(sortType, arg2);
	}

	if( (*sortType) )
	{
		bool found = false;
		for( std::list<std::string>::iterator sIter = SortTypes.begin();sIter != SortTypes.end();++sIter )
		{
			if( !str_cmp(sortType, (*sIter)) )
			{
				found = true;
				break;
			}
		}
		if( !found )
		{
			ch->send("Invalid sort type. Choose from the following: %s\r\n", strSortTypes.c_str());
			return;
		}
	}

	std::vector<Character *> mobList;

	for(unsigned int i = 0;i < MobManager::GetManager().NumberOfPrototypes();++i)
	{
		Character *mob = MobManager::GetManager().GetPrototype( i );

		if(keyword)
		{
			if(isname(keyword, mob->player.name))
				mobList.push_back(mob);
		}
		else
		{
			int vnum = mob->getVnum();
			if(vnum >= m1 && vnum <= m2)
				mobList.push_back(mob);
		}
	}

	if( mobList.empty() )
	{
		ch->send("No mobs were found.\r\n");
		return;
	}

	/* First, we need to go through and figure out how many zone commands exist that have a chance of loading this mob */
	for( unsigned int z = 0;z < ZoneManager::GetManager().NumZones();++z )
	{
		Zone *zone = ZoneManager::GetManager().GetZoneByRnum(z);
		for(unsigned int c = 0;c < zone->cmd.size();++c)
		{
			int zMobRnum = zone->cmd[c]->arg1;
			if( zone->cmd[c]->command == 'M' )
			{
				++zoneLoads[zMobRnum];

				if(!zone->IsClosed())
					++openZoneLoads[zMobRnum];
			}
		}
	}

	//Sort the mobs.
	std::function<bool(Character *mob1, Character *mob2)> sortPredicate =
		[](Character *mob1, Character *mob2) -> bool {
			return mob1->getVnum() < mob2->getVnum();
		};

	if(!str_cmp(sortType, "lvl") || !str_cmp(sortType, "level"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return GET_LEVEL(mob1) > GET_LEVEL(mob2);
		};
	else if(!str_cmp(sortType, "exp"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return GET_EXP(mob1) > GET_EXP(mob2);
		};
	else if(!str_cmp(sortType, "ob"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return GET_OB(mob1) > GET_OB(mob2);
		};
	else if(!str_cmp(sortType, "pb"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return GET_PB(mob1) > GET_PB(mob2);
		};
	else if(!str_cmp(sortType, "db"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return GET_DB(mob1) > GET_DB(mob2);
		};
	else if(!str_cmp(sortType, "hp"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return mob1->points.max_hit > mob2->points.max_hit;
		};
	else if(!str_cmp(sortType, "kit"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return mob1->MobData->primary_kit && mob2->MobData->primary_kit && mob1->MobData->primary_kit->vnum > mob2->MobData->primary_kit->vnum;
		};
	else if(!str_cmp(sortType, "gold"))
		sortPredicate = [](Character *mob1, Character *mob2) -> bool {
			return mob1->points.gold > mob2->points.gold;
		};
	else if(!str_cmp(sortType, "zloads"))
		sortPredicate = [&](Character *mob1, Character *mob2) -> bool {
			return zoneLoads[mob1->nr] > zoneLoads[mob2->nr];
		};
	else if(!str_cmp(sortType, "open"))
		sortPredicate = [&](Character *mob1, Character *mob2) -> bool {
			return openZoneLoads[mob1->nr] > openZoneLoads[mob2->nr];
		};


	std::sort(mobList.begin(), mobList.end(), sortPredicate);

	OutBuf << " Vnum               Name                                    Exp      Lvl      HP       OB        PB        DB       Kit       Copper   Zloads   Open" << std::endl;
	OutBuf << "------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;

	get_char_cols(ch);
	std::string nameStr;
	unsigned int nSizeLim = 45;

	//Loop through the list and get the display.
	for( Character *mob : mobList )
	{
		nameStr = GET_NAME(mob);
		if( nameStr.size() > nSizeLim )
		{
			nameStr.resize(nSizeLim - 3);
			nameStr += std::string("...");
		}
		int zoneCount = zoneLoads.count(mob->nr) ? zoneLoads[mob->nr] : (0);
		int openZoneCount = openZoneLoads.count(mob->nr) ? openZoneLoads[mob->nr] : (0);
		OutBuf << "[" << cyn << std::right << std::setw(5) << mob->getVnum() << nrm << "]";
		OutBuf << "  " << grn << std::left << std::setw(nSizeLim) << nameStr << nrm;
		OutBuf << " [" << cyn << std::right << std::setw(10) << GET_EXP(mob) << nrm << "]";
		OutBuf << " [" << cyn << std::right << std::setw(3) << ((int)GET_LEVEL(mob)) << nrm << "]";
		OutBuf << " [" << cyn << std::right << std::setw(6) << (mob)->points.move << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(6) << GET_OB(mob) << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(6) << GET_PB(mob) << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(6) << GET_DB(mob) << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(6)
				<< ((mob)->MobData->primary_kit ? MiscUtil::convert<std::string>(mob->MobData->primary_kit->vnum) : "<None>") << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(7) << mob->points.gold << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(4) << zoneCount << nrm << "]";
		OutBuf << " [ " << cyn << std::right << std::setw(4) << openZoneCount << nrm << "]";
		OutBuf << std::endl;
	}
	char *cBuf = new char[OutBuf.str().size()+1];
	strcpy(cBuf, OutBuf.str().c_str());
	page_string(ch->desc, cBuf, TRUE);

	delete[] cBuf;
};

int OlistCanDisp( Object *obj, char *arg1, char *arg2, char *arg3, bool RangeCheck )
{
	if(IS_OBJ_STAT(obj, ITEM_TRADES))
		return 0;

	//Sorting by number range...
	if ( RangeCheck )
	{
		//Need two numbers for range check.
		if ( ( !*arg1 || !*arg2 ) )
			return 0;
		else if ( GET_OBJ_VNUM( obj ) < atoi( arg1 ) )
			return 0;
		else if ( GET_OBJ_VNUM( obj ) > atoi( arg2 ) )
			return -1;
		else
			return 1;
	}
	//Sorting by Item Type
	else
	{
		//Need an item type name.
		if ( !*arg1 )
			return 0;
		//Check Weapon Types
		else if ( IS_LONG_BLADE( obj ) && ( !strn_cmp( arg1, "lblades", strlen( arg1 ) ) || !strn_cmp( arg1, "swords", strlen( arg1 ) ) ) )
			return 1;
		else if ( IS_SHORT_BLADE( obj ) && ( !strn_cmp( arg1, "daggers", strlen( arg1 ) ) || !strn_cmp( arg1, "sblades", strlen( arg1 ) ) ) )
			return 1;
		else if ( IS_CLUB( obj ) && !strn_cmp( arg1, "clubs", strlen( arg1 ) ) )
			return 1;
		else if ( IS_SPEAR( obj ) && !strn_cmp( arg1, "spears", strlen( arg1 ) ) )
			return 1;
		else if ( IS_AXE( obj ) && !strn_cmp( arg1, "axes", strlen( arg1 ) ) )
			return 1;
		else if ( IS_LANCE( obj ) && !strn_cmp( arg1, "lances", strlen( arg1 ) ) )
			return 1;
		else if ( IS_STAFF( obj ) && !strn_cmp( arg1, "staves", strlen( arg1 ) ) )
			return 1;
		else if ( IS_BOW( obj ) && !strn_cmp( arg1, "bows", strlen( arg1 ) ) )
			return 1;
		else if ( IS_CHAIN( obj ) && !strn_cmp( arg1, "chains", strlen( arg1 ) ) )
			return 1;
		else if ( obj->getType() == ITEM_WEAPON && !strn_cmp( arg1, "weapons", strlen( arg1 ) ) )
			return 1;

		//Armor Types
		else if ( IS_BODY_GEAR( obj ) && !strn_cmp( arg1, "body", strlen( arg1 ) ) )
			return 1;
		else if ( IS_HEAD_GEAR( obj ) && !strn_cmp( arg1, "head", strlen( arg1 ) ) )
			return 1;
		else if ( IS_SHOULDER_GEAR( obj ) && !strn_cmp( arg1, "shoulders", strlen( arg1 ) ) )
			return 1;
		else if ( IS_ARM_GEAR( obj ) && !strn_cmp( arg1, "arms", strlen( arg1 ) ) )
			return 1;
		else if ( IS_HAND_GEAR( obj ) && !strn_cmp( arg1, "hands", strlen( arg1 ) ) )
			return 1;
		else if ( IS_LEG_GEAR( obj ) && !strn_cmp( arg1, "legs", strlen( arg1 ) ) )
			return 1;
		else if ( IS_FEET_GEAR( obj ) && !strn_cmp( arg1, "feet", strlen( arg1 ) ) )
			return 1;
		else if ( IS_WRIST_GEAR( obj ) && !strn_cmp( arg1, "wrist", strlen( arg1 ) ) )
			return 1;
		else if ( IS_WAIST_GEAR( obj ) && !strn_cmp( arg1, "waist", strlen( arg1 ) ) )
			return 1;
		else if ( IS_EARRING( obj ) && !strn_cmp( arg1, "earrings", strlen( arg1 ) ) )
			return 1;
		else if ( IS_RING( obj ) && !strn_cmp( arg1, "rings", strlen( arg1 ) ) )
			return 1;
		else if ( IS_NECKLACE( obj ) && !strn_cmp( arg1, "neck", strlen( arg1 ) ) )
			return 1;
		else if ( IS_SHIELD( obj ) && !strn_cmp( arg1, "shields", strlen( arg1 ) ) )
			return 1;
		else if ( IS_CLOAK( obj ) && !strn_cmp( arg1, "about", strlen( arg1 ) ) )
			return 1;
		else if ( obj->getType() == ITEM_ARMOR && !strn_cmp( arg1, "armor", strlen( arg1 ) ) )
			return 1;
		else if (!strn_cmp(arg1, "misc", strlen(arg1)))
			return 1;
		else
			return 0;
	}

	MudLog( BRF, LVL_APPR, TRUE, "Error in OlistCanDisp(): Should not be at this point. Need a coder to check this." );
	return 0;
}

std::string parseParameters(const std::string &input, std::map<std::string, std::string> &parameters)
{
	std::string regexStr = "--(\\w+)(?:=(\\w+))?";
	boost::regex expression(regexStr.c_str());
	boost::match_results<std::string::const_iterator> what;
	auto start = input.begin();
	while(boost::regex_search(start, input.end(), what, expression, boost::match_default))
	{
		std::string paramName = what[ 1 ];
		std::string paramValue = what[ 2 ];

		start = what[ 0 ].second;

		parameters[ paramName ] = paramValue;

		//TODO: Remove the text from the string.
	}

	return "";
}

/* 2006 - Galnor - Added to aide with balancing. 'vnum o' does not allow easy lookup. 
                   This variation will allow items to be filtered and sorted in various ways.
*/
CommandHandler  do_olist  = DEFINE_COMMAND
{
	char arg1[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ], arg3[ MAX_INPUT_LENGTH ];
	int i = 0, ret = 0;
	bool RangeCheck = false;
	std::list<Object *> armor, weapons, misc, *l;
	std::list<Object *>::iterator iter;

	std::map<std::string, std::string> params;
	parseParameters(argument, params);

	std::string buffer;
	//olist body
	//olist body abs
	//olist 1 100 abs

	OneArgument( TwoArguments( argument, arg1, arg2 ), arg3 );

	if ( !*arg1 )
	{
		ch->send( "Syntax: olist <Bottom Obj Number> <Top Obj Number> <Sort Type(optional)>  OR\r\n"
		          "        olist <Item Type> <Sort Type(optional)>" );
		return ;
	}
	//Ok, we know they're doing a range check here.
	if ( IsNumber( arg1 ) && IsNumber( arg2 ) )
	{
		RangeCheck = true;

		if ( atoi( arg1 ) < 0 || atoi( arg2 ) < 0 )
		{
			ch->send( "Both numbers must be above zero.\r\n" );
			return ;
		}

		if ( atoi( arg2 ) < atoi( arg1 ) )
		{
			ch->send( "The second obj number must be greater than or equal to the first. %d %d, not %d %d\r\n",
			          atoi( arg2 ), atoi( arg1 ), atoi( arg1 ), atoi( arg2 ) );
			return ;
		}
	}
	//They're doing a type check.
	else
	{
		RangeCheck = false;
		strcpy( arg3, arg2 );
	}

	for ( i = 0;i <= top_of_objt;++i )
	{
		if ( ( ret = OlistCanDisp(obj_proto[ i ], arg1, arg2, arg3, RangeCheck) ) > 0 )
		{
			if ( obj_proto[ i ]->getType() == ITEM_WEAPON )
				l = &weapons;
			else if ( obj_proto[ i ]->getType() == ITEM_ARMOR )
				l = &armor;
			else
				l = &misc;

			if ( !l->size() )
			{
				l->push_back(obj_proto[ i ]);
				continue;
			}

			for ( iter = l->begin();iter != l->end();++iter )
			{
				if ( (l == &armor || l == &misc) && !str_cmp( arg3, "abs" ) )
				{
					if ( GET_OBJ_ABS(obj_proto[ i ]) >= GET_OBJ_ABS( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( (l == &armor || l == &misc) && !str_cmp( arg3, "db" ) )
				{
					if ( GET_OBJ_DB(obj_proto[ i ]) >= GET_OBJ_DB( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( !str_cmp( arg3, "pb" ) )
				{
					if ( GET_OBJ_PB( obj_proto[ i ] ) >= GET_OBJ_PB( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( (l == &armor || l == &misc) && !str_cmp( arg3, "defense" ) )
				{
					if ( GET_OBJ_PB(obj_proto[ i ] ) + GET_OBJ_DB( obj_proto[ i ] ) >= GET_OBJ_PB( *iter ) + GET_OBJ_DB( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( !str_cmp( arg3, "ob" ) )
				{
					if ( GET_OBJ_OB( obj_proto[ i ] ) >= GET_OBJ_OB( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( !str_cmp( arg3, "weight" ) )
				{
					if ( GET_OBJ_WEIGHT( obj_proto[ i ] ) >= GET_OBJ_WEIGHT( *iter ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( l == &weapons && !str_cmp( arg3, "dmg1" ) )
				{
					if ( GET_OBJ_VAL( obj_proto[ i ], 1 ) >= GET_OBJ_VAL( ( *iter ), 1 ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( l == &weapons && !str_cmp( arg3, "dmg2" ) )
				{
					if ( GET_OBJ_VAL( obj_proto[ i ], 2 ) >= GET_OBJ_VAL( ( *iter ), 2 ) )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else if ( l == &weapons && !str_cmp( arg3, "bash" ) )
				{
					if (obj_proto[ i ]->BashRating() >= (*iter)->BashRating() )
					{
						l->insert( iter, obj_proto[ i ] );
						break;
					}
				}
				else
				{
					if ( !*arg3 )
					{
						if ( GET_OBJ_VNUM( obj_proto[ i ] ) < GET_OBJ_VNUM( *iter ) )
						{
							l->insert( iter, obj_proto[ i ] );
							break;
						}
					}
					else
					{
						ch->send( "Item value must be one of the following:\r\n"
						          "WEAPONS: weight, ob, dmg1, dmg2, pb, db%s\r\n"
						          "ARMOR:   weight, on, pb, db, defense\r\n"
						          "MISC:    weight, pb, db, ob, defense\r\n",
								  ", bash");
						return ;
					}
				}
			}
			if ( iter == l->end() )
				l->push_back( obj_proto[ i ] );
		}
		else if ( ret == -1 )
			break;
	}
	if ( !weapons.size() && !armor.size() && !misc.size() )
		ch->send( "No items matched your search request.\r\n" );
	else
	{
		if ( armor.size() )
		{
			strcat(buffer,
			         "               %s%sA R M O R       I T E M S%s\r\n"
			         " Vnum               Item name                          ABS   DB    PB    Def    Wgt    Num\r\n"
			         "--------------------------------------------------------------------------------------------\r\n",
			         COLOR_BOLD( ch, CL_COMPLETE ), COLOR_CYAN( ch, CL_COMPLETE ), COLOR_NORMAL( ch, CL_COMPLETE ) );
			for ( iter = armor.begin();iter != armor.end();++iter )
			{
				strcat(buffer, "[%s%-7d%s]   %s%s%-40.40s%s  [%s%3d%s] [%s%3d%s] [%s%3d%s] [%s%3d%s] [%s%5.1f%s] [%s%4d%s]\r\n",
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_VNUM( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_BOLD( ch, CL_COMPLETE ), COLOR_GREEN( ch, CL_COMPLETE ), ( *iter ) ->GetSDesc(),
				         COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_ABS( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_DB( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_PB( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_DB( *iter ) + GET_OBJ_PB( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_WEIGHT( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
						 COLOR_CYAN( ch, CL_COMPLETE ), ItemCount[(*iter)->item_number], COLOR_NORMAL(ch, CL_COMPLETE) );
			}
		}
		if ( weapons.size() )
		{
			strcat(buffer,
			         "               %s%sW E A P O N         I T E M S%s\r\n"
			         " Vnum               Item name                           OB   PB     Wgt    Dmg1  Dmg2  Bash     Num  Hands\r\n"
			         "------------------------------------------------------------------------------------------------------------\r\n",
					 COLOR_BOLD( ch, CL_COMPLETE ), COLOR_CYAN( ch, CL_COMPLETE ), COLOR_NORMAL( ch, CL_COMPLETE ) );
			for ( iter = weapons.begin();iter != weapons.end();++iter )
			{
				char BashRating[128];
				sprintf(BashRating, "[%s%6.2f%s]",COLOR_CYAN(ch,CL_COMPLETE),
					(*iter)->BashRating(), COLOR_NORMAL(ch,CL_COMPLETE));
				strcat(buffer,
				         "[%s%-7d%s]   %s%s%-40.40s%s  [%s%3d%s] [%s%3d%s] [%s%5.1f%s] [%s%3d%s] [%s%3d%s] %s [%s%4d%s] [%s%1d%s]\r\n",
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_VNUM( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_BOLD( ch, CL_COMPLETE ), COLOR_GREEN( ch, CL_COMPLETE ), ( *iter ) ->GetSDesc(),
				         COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_OB( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_PB( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_WEIGHT( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_VAL( ( *iter ), 1 ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_VAL( ( *iter ), 2 ), COLOR_NORMAL( ch, CL_COMPLETE ),
						 BashRating,
						 COLOR_CYAN( ch, CL_COMPLETE ), ItemCount[(*iter)->item_number], COLOR_NORMAL(ch, CL_COMPLETE),
						 COLOR_CYAN( ch, CL_COMPLETE ), (IS_OBJ_STAT(*iter, ITEM_TWO_HANDED) ? 2 : 1 ), COLOR_NORMAL( ch, CL_COMPLETE ));
			}
		}
		if ( misc.size() )
		{
			strcat(buffer,
			         "               %s%sM I S C       I T E M S%s\r\n"
			         " Vnum               Item name                           OB    PB     DB      Wgt     Num\r\n"
			         "-----------------------------------------------------------------------------------------\r\n",
					 COLOR_BOLD( ch, CL_COMPLETE ), COLOR_CYAN( ch, CL_COMPLETE ), COLOR_NORMAL( ch, CL_COMPLETE ) );
			for ( iter = misc.begin();iter != misc.end();++iter )
			{
				strcat(buffer,
				         "[%s%-7d%s]   %s%s%-40.40s%s  [%s%3d%s]  [%s%3d%s]  [%s%3d%s]  [%s%5.1f%s]  [%s%4d%s]\r\n",
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_VNUM( *iter ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_BOLD( ch, CL_COMPLETE ), COLOR_GREEN( ch, CL_COMPLETE ), ( *iter ) ->GetSDesc(),
				         COLOR_NORMAL( ch, CL_COMPLETE ),
						 COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_OB( ( *iter ) ), COLOR_NORMAL( ch, CL_COMPLETE ),
						 COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_DB( ( *iter ) ), COLOR_NORMAL( ch, CL_COMPLETE ),
						 COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_PB( ( *iter ) ), COLOR_NORMAL( ch, CL_COMPLETE ),
				         COLOR_CYAN( ch, CL_COMPLETE ), GET_OBJ_WEIGHT( ( *iter ) ), COLOR_NORMAL( ch, CL_COMPLETE ),
						 COLOR_CYAN( ch, CL_COMPLETE ), ItemCount[(*iter)->item_number], COLOR_NORMAL(ch, CL_COMPLETE));
			}
		}
		page_string(ch->desc, (char *)buffer.c_str(), TRUE);
	}
};

void AddOlcLog( Character *ch, const std::string &Type, const int targetId )
{
	try {
		std::stringstream QueryBuffer;
		QueryBuffer << " INSERT INTO olcLog("
					<< "   `user_id`,"
					<< "   `target_type`,"
					<< "   `target_id`,"
					<< "   `created_datetime`"
					<< " ) VALUES("
					<< ch->player.idnum << ","
					<< sql::escapeQuoteString(Type) << ","
					<< targetId << ","
					<< sql::encodeQuoteDate(time(0))
					<< ")";

		gameDatabase->sendRawQuery( QueryBuffer.str() );
	} catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "AddOLCLog - %s", e.getMessage().c_str());
		return;
	}
}
