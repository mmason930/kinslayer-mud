/************************************************************************
 * OasisOLC - sedit.c						v1.5	*
 * Copyright 1996 Harvey Gilpin.					*
 ************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "shop.h"
#include "olc.h"
#include "mobs.h"
#include "zones.h"
#include "Descriptor.h"

/*-------------------------------------------------------------------*/

/*
 * External variable declarations.
 */
extern class Shop *shop_index;
extern int top_shop;

extern struct Index *obj_index;
extern const char *trade_letters[];
extern const char *shop_bits[];
extern const char *item_types[];

/*-------------------------------------------------------------------*/

/*
 * Handy macros.
 */
#define S_NUM(i)		((i)->vnum)
#define S_KEEPER(i)		((i)->keeper)
#define S_OPEN1(i)		((i)->open1)
#define S_CLOSE1(i)		((i)->close1)
#define S_OPEN2(i)		((i)->open2)
#define S_CLOSE2(i)		((i)->close2)
#define S_BANK(i)		((i)->bankAccount)
#define S_BROKE_TEMPER(i)	((i)->temper1)
#define S_BITVECTOR(i)		((i)->bitvector)
#define S_NOTRADE(i)		((i)->with_who)
#define S_SORT(i)		((i)->lastsort)
#define S_BUYPROFIT(i)		((i)->profit_buy)
#define S_SELLPROFIT(i)		((i)->profit_sell)
#define S_FUNC(i)		((i)->func)

#define S_ROOMS(i)		((i)->in_room)
#define S_PRODUCTS(i)		((i)->producing)
#define S_NAMELISTS(i)		((i)->type)
#define S_ROOM(i, num)		((i)->in_room[(num)])
#define S_PRODUCT(i, num)	((i)->producing[(num)])
#define S_BUYTYPE(i, num)	(BUY_TYPE((i)->type[(num)]))

#define S_NOITEM1(i)		((i)->no_such_item1)
#define S_NOITEM2(i)		((i)->no_such_item2)
#define S_NOCASH1(i)		((i)->missing_cash1)
#define S_NOCASH2(i)		((i)->missing_cash2)
#define S_NOBUY(i)		((i)->do_not_buy)
#define S_BUY(i)		((i)->message_buy)
#define S_SELL(i)		((i)->message_sell)

/*-------------------------------------------------------------------*/

/*
 * Function prototypes.
 */
int real_shop(int vshop_num);
void sedit_setup_new(Descriptor *d);
void sedit_setup_existing(Descriptor *d, int rmob_num);
void sedit_parse(Descriptor *d, char *arg);
void sedit_disp_menu(Descriptor *d);
void sedit_namelist_menu(Descriptor *d);
void sedit_types_menu(Descriptor *d);
void sedit_products_menu(Descriptor *d);
void sedit_rooms_menu(Descriptor *d);
void sedit_compact_rooms_menu(Descriptor *d);
void sedit_shop_flags_menu(Descriptor *d);
void sedit_no_trade_menu(Descriptor *d);
void sedit_save_internally(Descriptor *d);
void sedit_save_to_disk();
void copy_shop(Shop *tshop, Shop *fshop);
void copy_list(int **tlist, int *flist);
void free_type_list(struct shop_buy_data **listy);
void sedit_modify_string(char **str, char *newt);

/*
 * External functions.
 */
SPECIAL(shop_keeper);

/*-------------------------------------------------------------------*\
  utility functions 
\*-------------------------------------------------------------------*/

void sedit_setup_new(Descriptor *d)
{
	Shop *shop;

	/*
	 * Allocate a scratch shop structure.
	 */

	shop = new Shop();

	/*
	 * Fill in some default values.
	 */

	S_KEEPER(shop) = -1;
	S_CLOSE1(shop) = 28;
	S_BUYPROFIT(shop) = 1.0;
	S_SELLPROFIT(shop) = 1.0;

	//Add a spice of default strings.

	S_NOITEM1(shop)	= "%s Sorry, I don't stock that item.";
	S_NOITEM2(shop)	= "%s You don't seem to have that.";
	S_NOCASH1(shop)	= "%s I can't afford that!";
	S_NOCASH2(shop)	= "%s You are too poor!";
	S_NOBUY(shop)	= "%s I don't trade in such items.";
	S_BUY(shop)		= "%s That'll be %d coins, thanks.";
	S_SELL(shop)	= "%s I'll give you %d coins for that.";

	// Presto! A shop.

	OLC_SHOP(d) = shop;
	sedit_disp_menu(d);
}

/*-------------------------------------------------------------------*/

void sedit_setup_existing(Descriptor *d, int rshop_num)
{
	/*
	 * Create a scratch shop structure.
	 */

	OLC_SHOP(d) = new Shop();

	copy_shop(OLC_SHOP(d), shop_index + rshop_num);
	sedit_disp_menu(d);
}

/*-------------------------------------------------------------------*/

void copy_shop(Shop *tshop, Shop *fshop)
{
	/*
	 * Copy basic information over.
	 */

	S_NUM(tshop)			= S_NUM(fshop);
	S_KEEPER(tshop)			= S_KEEPER(fshop);
	S_OPEN1(tshop)			= S_OPEN1(fshop);
	S_CLOSE1(tshop)			= S_CLOSE1(fshop);
	S_OPEN2(tshop)			= S_OPEN2(fshop);
	S_CLOSE2(tshop)			= S_CLOSE2(fshop);
	S_BANK(tshop)			= S_BANK(fshop);
	S_BROKE_TEMPER(tshop)	= S_BROKE_TEMPER(fshop);
	S_BITVECTOR(tshop)		= S_BITVECTOR(fshop);
	S_NOTRADE(tshop)		= S_NOTRADE(fshop);
	S_SORT(tshop)			= S_SORT(fshop);
	S_BUYPROFIT(tshop)		= S_BUYPROFIT(fshop);
	S_SELLPROFIT(tshop)		= S_SELLPROFIT(fshop);
	S_FUNC(tshop)			= S_FUNC(fshop);

	/*
	 * Copy lists over.
	 */

	tshop->in_room = fshop->in_room;
	tshop->producing = fshop->producing;

	tshop->Type = fshop->Type;

	/*
	 * Copy notification strings over.
	 */

	tshop->no_such_item1	= fshop->no_such_item1;
	tshop->no_such_item2	= fshop->no_such_item2;
	tshop->missing_cash1	= fshop->missing_cash1;
	tshop->missing_cash2	= fshop->missing_cash2;
	tshop->do_not_buy		= fshop->do_not_buy;
	tshop->message_buy		= fshop->message_buy;
	tshop->message_sell		= fshop->message_sell;
}

/*-------------------------------------------------------------------*/

/*
 * Copy a -1 terminated integer array list.
 */
void copy_list(int **tlist, int *flist)
{
	int num_items, i;

	if (*tlist)
		delete[] (*tlist);

	/*
	 * Count number of entries.
	 */

	for (i = 0; flist[i] != -1; ++i);
	num_items = i + 1;

	/*
	 * Make space for entries.
	 */

	*tlist = new int[num_items];

	/*
	 * Copy entries over.
	 */

	i = 0;

	do
	{
		(*tlist)[i] = flist[i];

	}
	while (++i < num_items);
}

/*-------------------------------------------------------------------*/

void sedit_add_to_int_list(int **listy, int newt)
{
	int i, num_items, *nlist;

	/*
	 * Count number of entries.
	 */

	for (i = 0; (*listy)[i] != -1; ++i)
		;
	num_items = i;

	/*
	 * Make a new list and slot in the new entry.
	 */

	nlist = new int[num_items + 2];

	for (i = 0; i < num_items; ++i)
		nlist[i] = (*listy)[i];

	nlist[num_items] = newt;
	nlist[num_items + 1] = -1;

	/*
	 * Out with the old, in with the new.
	 */

	delete[] (*listy);
	*listy = nlist;
}

/*-------------------------------------------------------------------*/

void sedit_remove_from_int_list(int **listy, int num)
{
	int i, num_items, *nlist;

	/*
	 * Count number of entries.
	 */

	for (i = 0; (*listy)[i] != -1; ++i)
		;

	if (num >= i || num < 0)
		return;

	num_items = i;

	nlist = new int[num_items];

	for (i = 0; i < num_items; ++i)
		nlist[i] = (i < num) ? (*listy)[i] : (*listy)[i + 1];

	delete[] (*listy);
	*listy = nlist;
}


/*-------------------------------------------------------------------*/

int real_shop(int vshop_num)
{
	int rshop_num;

	for (rshop_num = 0; rshop_num < top_shop; rshop_num++)
		if (SHOP_NUM(rshop_num) == vshop_num)
			return rshop_num;

	return -1;
}

/*-------------------------------------------------------------------*/

/*
 * Generic string modifyer for shop keeper messages.
 */
void sedit_modify_string(std::string &str, char *newt)
{
	char *pointer;

	/*
	 * Check the '%s' is present, if not, add it.
	 */

	if (*newt != '%')
	{
		strcpy(buf, "%s ");
		strcat(buf, newt);
		pointer = buf;
	}

	else
		pointer = newt;

	str = pointer;
}

/*-------------------------------------------------------------------*/

void sedit_save_internally(Descriptor *d)
{
	int rshop, found = 0;
	Shop *shop;
	Shop *new_index;
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d));

	rshop = real_shop(OLC_NUM(d));
	shop = OLC_SHOP(d);
	S_NUM(shop) = OLC_NUM(d);


	if (rshop > -1)
	{	/* The shop already exists, just update it. */
		copy_shop((shop_index + rshop), shop);
	}

	else
	{		/* Doesn't exist - have to insert it. */
		new_index = new Shop[top_shop + 1];

		for (rshop = 0; rshop < top_shop; ++rshop)
		{
			/* Is this the place? */
			if (!found)
			{
				/* Yep, stick it in here. */
				if (SHOP_NUM(rshop) > OLC_NUM(d))
				{
					found = 1;
					copy_shop(&(new_index[rshop]), shop);

					/*
					 * Move the entry that used to go here up a place.
					 */

					new_index[rshop + 1] = shop_index[rshop];
				}

				else	/* This isn't the place, copy over info. */
					new_index[rshop] = shop_index[rshop];
			}

			else
			{	/* Shop's already inserted, copy rest over. */
				new_index[rshop + 1] = shop_index[rshop];
			}
		}

		if (!found)
			copy_shop(&(new_index[rshop]), shop);

		//Switch the new index in.

		delete [] (shop_index);
		shop_index = new_index;
		++top_shop;
	}
}

/*-------------------------------------------------------------------*/

void sedit_save_to_disk()
{
	sql::BatchInsertStatement shopBatchInsertStatement(gameDatabase, "tempShop", 10000);

	sql::BatchInsertStatement shopProducingBatchInsertStatement(gameDatabase, "tempShopProducing", 100000);

	sql::BatchInsertStatement shopBuyTypeBatchInsertStatement(gameDatabase, "tempShopBuyType", 100000);

	sql::BatchInsertStatement shopRoomBatchInsertStatement(gameDatabase, "tempShopRoom", 100000);

	shopBatchInsertStatement.addField("id");

	shopBatchInsertStatement.addField("profit_buy");

	shopBatchInsertStatement.addField("profit_sell");

	shopBatchInsertStatement.addField("open_time_1");

	shopBatchInsertStatement.addField("close_time_1");

	shopBatchInsertStatement.addField("open_time_2");

	shopBatchInsertStatement.addField("close_time_2");

	shopBatchInsertStatement.addField("no_such_item_1");

	shopBatchInsertStatement.addField("no_such_item_2");

	shopBatchInsertStatement.addField("do_not_buy");

	shopBatchInsertStatement.addField("missing_cash_1");

	shopBatchInsertStatement.addField("missing_cash_2");

	shopBatchInsertStatement.addField("buy");

	shopBatchInsertStatement.addField("sell");

	shopBatchInsertStatement.addField("bitvector");

	shopBatchInsertStatement.addField("keeper_vnum");

	shopBatchInsertStatement.addField("will_trade_with");

	shopBatchInsertStatement.start();

	shopProducingBatchInsertStatement.addField("shop_id");

	shopProducingBatchInsertStatement.addField("object_vnum");

	shopProducingBatchInsertStatement.start();

	shopBuyTypeBatchInsertStatement.addField("shop_id");

	shopBuyTypeBatchInsertStatement.addField("object_type");

	shopBuyTypeBatchInsertStatement.addField("keywords");

	shopBuyTypeBatchInsertStatement.start();

	shopRoomBatchInsertStatement.addField("shop_id");

	shopRoomBatchInsertStatement.addField("room_id");

	shopRoomBatchInsertStatement.start();

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempShop, tempShopProducing, tempShopBuyType, tempShopRoom");

	gameDatabase->sendRawQuery("CREATE TABLE tempShop LIKE shop");

	gameDatabase->sendRawQuery("CREATE TABLE tempShopRoom LIKE shopRoom");

	gameDatabase->sendRawQuery("CREATE TABLE tempShopBuyType LIKE shopBuyType");

	gameDatabase->sendRawQuery("CREATE TABLE tempShopProducing LIKE shopProducing");

	for(int i = 0;i < top_shop;++i)
	{
		Shop *shop = &shop_index[i];

		shopBatchInsertStatement.beginEntry();

		shopBatchInsertStatement.putInt(shop->vnum);

		shopBatchInsertStatement.putDouble(shop->profit_buy);

		shopBatchInsertStatement.putDouble(shop->profit_sell);

		shopBatchInsertStatement.putInt(shop->open1);

		shopBatchInsertStatement.putInt(shop->close1);

		shopBatchInsertStatement.putInt(shop->open2);

		shopBatchInsertStatement.putInt(shop->close2);

		shopBatchInsertStatement.putString(shop->no_such_item1);

		shopBatchInsertStatement.putString(shop->no_such_item2);

		shopBatchInsertStatement.putString(shop->do_not_buy);

		shopBatchInsertStatement.putString(shop->missing_cash1);

		shopBatchInsertStatement.putString(shop->missing_cash2);

		shopBatchInsertStatement.putString(shop->message_buy);

		shopBatchInsertStatement.putString(shop->message_sell);

		shopBatchInsertStatement.putInt(shop->bitvector);

		shopBatchInsertStatement.putInt((shop->keeper >= 0 && (u_int)shop->keeper < MobManager::GetManager().NumberOfPrototypes())
			? MobManager::GetManager().GetIndex(shop->keeper)->vnum : (-1));

		shopBatchInsertStatement.putInt(shop->with_who);

		shopBatchInsertStatement.endEntry();

		for(int k = 0;k < (int)shop->producing.size();++k) {

			shopProducingBatchInsertStatement.beginEntry();

			shopProducingBatchInsertStatement.putInt(shop->vnum);

			shopProducingBatchInsertStatement.putInt(shop->producing[k]);

			shopProducingBatchInsertStatement.endEntry();
		}
		for(std::map<int, ShopBuyData>::iterator iter = shop->Type.begin();iter != shop->Type.end();++iter) {

			int objectType = (*iter).second.type;

			std::string keywords = (*iter).second.keywords;

			shopBuyTypeBatchInsertStatement.beginEntry();

			shopBuyTypeBatchInsertStatement.putInt(shop->vnum);

			shopBuyTypeBatchInsertStatement.putInt(objectType);

			shopBuyTypeBatchInsertStatement.putString(keywords);

			shopBuyTypeBatchInsertStatement.endEntry();
		}
		for(int k = 0;k < (int)shop->in_room.size();++k) {

			shopRoomBatchInsertStatement.beginEntry();

			shopRoomBatchInsertStatement.putInt(shop->vnum);

			shopRoomBatchInsertStatement.putInt(shop->in_room[k]);

			shopRoomBatchInsertStatement.endEntry();
		}
	}

	shopBatchInsertStatement.finish();

	shopBuyTypeBatchInsertStatement.finish();

	shopRoomBatchInsertStatement.finish();

	shopProducingBatchInsertStatement.finish();

	gameDatabase->sendRawQuery(
		"RENAME TABLE shop TO shopOld, tempShop TO shop, shopOld TO tempShop, "
		"shopProducing TO shopProducingOld, tempShopProducing TO shopProducing, shopProducingOld TO tempShopProducing,"
		"shopRoom TO shopRoomOld, tempShopRoom TO shopRoom, shopRoomOld TO tempShopRoom,"
		"shopBuyType TO shopBuyTypeOld, tempShopBuyType TO shopBuyType, shopBuyTypeOld TO tempShopBuyType"
		);

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempShop,tempShopRoom,tempShopProducing,tempShopBuyType");

}

/**************************************************************************
 Menu functions 
 **************************************************************************/

void sedit_products_menu(Descriptor *d)
{
	Shop *shop;
	unsigned int i;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	d->Send("##     VNUM     Product\r\n");

	for (i = 0; i < shop->producing.size(); ++i)
	{
		d->Send("%2d - [%s%5d%s] - %s%s%s\r\n", i,
		        cyn, obj_index[shop->producing[i]].vnum, nrm,
		        yel, obj_proto[shop->producing[i]]->short_description, nrm);
	}

	d->Send("\r\n"
	        "%sA%s) Add a new product.\r\n"
	        "%sD%s) Delete a product.\r\n"
	        "%sQ%s) Quit\r\n"
	        "Enter choice : ", grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE(d) = SEDIT_PRODUCTS_MENU;
}

/*-------------------------------------------------------------------*/

void sedit_compact_rooms_menu(Descriptor *d)
{
	Shop *shop;
	int i, count = 0;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif
	
	for (i = 0;i < shop->in_room.size(); ++i)
	{
		d->Send("%2d - [%s%5d%s]  | %s", i, cyn, shop->in_room[i], nrm,
		        !(++count % 5) ? "\r\n" : "");
	}

	d->Send("\r\n"
	        "%sA%s) Add a new room.\r\n"
	        "%sD%s) Delete a room.\r\n"
	        "%sL%s) Long display.\r\n"
	        "%sQ%s) Quit\r\n"
	        "Enter choice : ", grn, nrm, grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE(d) = SEDIT_ROOMS_MENU;
}

/*-------------------------------------------------------------------*/

void sedit_rooms_menu(Descriptor *d)
{
	Shop *shop;
	unsigned int i;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	d->Send("##     VNUM     Room\r\n\r\n");

	for (i = 0; i < shop->in_room.size(); ++i)
	{
		d->Send("%2d - [%s%5d%s] - %s%s%s\r\n", (i+1), cyn, shop->in_room[i], nrm,
		        yel, FindRoomByVnum(shop->in_room[i])->name, nrm);
	}

	d->Send("\r\n"
	        "%sA%s) Add a new room.\r\n"
	        "%sD%s) Delete a room.\r\n"
	        "%sC%s) Compact Display.\r\n"
	        "%sQ%s) Quit\r\n"
	        "Enter choice : ", grn, nrm, grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE(d) = SEDIT_ROOMS_MENU;
}

/*-------------------------------------------------------------------*/

void sedit_namelist_menu(Descriptor *d)
{
	Shop *shop;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	d->Send("##              Type   Namelist\r\n\r\n");

	std::map<int, ShopBuyData>::iterator t = shop->Type.begin();
	for (int i = 0;t != shop->Type.end();++i, ++t)
	{
		d->Send("%2d - %s%15s%s - %s%s%s\r\n", i, cyn,
		        item_types[ (*t).first ], nrm, yel,
		        (*t).second.keywords.size() ? (*t).second.keywords.c_str() : "<None>", nrm);
	}

	d->Send("\r\n"
	        "%sA%s) Add a new entry.\r\n"
	        "%sD%s) Delete an entry.\r\n"
	        "%sQ%s) Quit\r\n"
	        "Enter choice : ", grn, nrm, grn, nrm, grn, nrm);

	OLC_MODE(d) = SEDIT_NAMELIST_MENU;
}

/*-------------------------------------------------------------------*/

void sedit_shop_flags_menu(Descriptor *d)
{
	int i, count = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	for (i = 0; i < NUM_SHOP_FLAGS; ++i)
	{
		d->Send("%s%2d%s) %-20.20s   %s", grn, i + 1, nrm, shop_bits[i],
		        !(++count % 2) ? "\r\n" : "");
	}

	sprintbit(S_BITVECTOR(OLC_SHOP(d)), shop_bits, buf1);
	d->Send("\r\nCurrent Shop Flags : %s%s%s\r\nEnter choice : ",
	        cyn, buf1, nrm);
	OLC_MODE(d) = SEDIT_SHOP_FLAGS;
}

/*-------------------------------------------------------------------*/

void sedit_no_trade_menu(Descriptor *d)
{
	int i, count = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	for (i = 0; i < NUM_TRADERS; ++i)
	{
		d->Send("%s%2d%s) %-20.20s   %s", grn, i + 1, nrm, trade_letters[i],
		        !(++count % 2) ? "\r\n" : "");
	}

	sprintbit(S_NOTRADE(OLC_SHOP(d)), trade_letters, buf1);
	d->Send("\r\nCurrently won't trade with: %s%s%s\r\n"
	        "Enter choice : ", cyn, buf1, nrm);
	OLC_MODE(d) = SEDIT_NOTRADE;
}

/*-------------------------------------------------------------------*/

void sedit_types_menu(Descriptor *d)
{
	Shop *shop;
	int i, count = 0;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	for (i = 0; i < NUM_ITEM_TYPES; ++i)
	{
		sprintf(buf, "%s%2d%s) %s%-20s%s  %s", grn, i, nrm, cyn, item_types[i],
		        nrm, !(++count % 3) ? "\r\n" : "");
		d->Send(buf);
	}

	sprintf(buf, "%sEnter choice : ", nrm);
	d->Send(buf);
	OLC_MODE(d) = SEDIT_TYPE_MENU;
}

/*-------------------------------------------------------------------*/

/*
 * Display main menu.
 */
void sedit_disp_menu(Descriptor *d)
{
	Shop *shop;

	shop = OLC_SHOP(d);
	get_char_cols(d->character);

	sprintbit(S_NOTRADE(shop)	, trade_letters	, buf1);
	sprintbit(S_BITVECTOR(shop)	, shop_bits		, buf2);
	d->Send(
#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif

	    "-- Shop Number : [%s%d%s]\r\n"
	    "%s0%s) Keeper      : [%s%d%s] %s%s\r\n"
	    "%s1%s) Open 1      : %s%4d%s          %s2%s) Close 1     : %s%4d\r\n"
	    "%s3%s) Open 2      : %s%4d%s          %s4%s) Close 2     : %s%4d\r\n"
	    "%s5%s) Sell rate   : %s%1.2f%s          %s6%s) Buy rate    : %s%1.2f\r\n"
	    "%s7%s) Keeper no item : %s%s\r\n"
	    "%s8%s) Player no item : %s%s\r\n"
	    "%s9%s) Keeper no cash : %s%s\r\n"
	    "%sA%s) Player no cash : %s%s\r\n"
	    "%sB%s) Keeper no buy  : %s%s\r\n"
	    "%sC%s) Buy sucess     : %s%s\r\n"
	    "%sD%s) Sell sucess    : %s%s\r\n"
	    "%sE%s) No Trade With  : %s%s\r\n"
	    "%sF%s) Shop flags     : %s%s\r\n"
	    "%sR%s) Rooms Menu\r\n"
	    "%sP%s) Products Menu\r\n"
	    "%sT%s) Accept Types Menu\r\n"
	    "%sQ%s) Quit\r\n"
	    "Enter Choice : ",

	    cyn, OLC_NUM(d), nrm,
	    grn, nrm, cyn, S_KEEPER(shop) == -1 ?
		-1 : MobManager::GetManager().GetIndex(shop->keeper)->vnum, nrm,
	    yel, S_KEEPER(shop) == -1 ?
		"None" : MobManager::GetManager().GetPrototype(S_KEEPER(shop))->player.short_descr,
	    grn, nrm, cyn, S_OPEN1(shop), nrm,
	    grn, nrm, cyn, S_CLOSE1(shop),
	    grn, nrm, cyn, S_OPEN2(shop), nrm,
	    grn, nrm, cyn, S_CLOSE2(shop),
	    grn, nrm, cyn, S_BUYPROFIT(shop), nrm,
	    grn, nrm, cyn, S_SELLPROFIT(shop),
	    grn, nrm, yel, shop->no_such_item1.c_str(),
	    grn, nrm, yel, shop->no_such_item2.c_str(),
	    grn, nrm, yel, shop->missing_cash1.c_str(),
	    grn, nrm, yel, shop->missing_cash2.c_str(),
	    grn, nrm, yel, shop->do_not_buy.c_str(),
	    grn, nrm, yel, shop->message_buy.c_str(),
	    grn, nrm, yel, shop->message_sell.c_str(),
	    grn, nrm, cyn, buf1,
	    grn, nrm, cyn, buf2,
	    grn, nrm, grn, nrm, grn, nrm, grn, nrm
	);

	OLC_MODE(d) = SEDIT_MAIN_MENU;
}

/**************************************************************************
  The GARGANTUAN event handler
 **************************************************************************/

void sedit_parse(Descriptor *d, char *arg)
{
	int i;
	std::vector<int>::iterator iter;
	std::map< int, ShopBuyData >::iterator T;

	if (OLC_MODE(d) > SEDIT_NUMERICAL_RESPONSE)
	{
		if (!isdigit(arg[0]) && ((*arg == '-') && (!isdigit(arg[1]))))
		{
			d->Send("Field must be numerical, try again : ");
			return;
		}
	}

	switch (OLC_MODE(d))
	{
			/*-------------------------------------------------------------------*/
		case SEDIT_CONFIRM_SAVESTRING:
			switch (*arg)
			{

				case 'y':
				case 'Y':
					d->Send("Saving shop to memory.\r\n");
					sedit_save_internally(d);
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE,
					       "OLC: %s edits shop %d", GET_NAME(d->character), OLC_NUM(d));
					cleanup_olc(d, CLEANUP_STRUCTS);
					return;

				case 'n':
				case 'N':
					cleanup_olc(d, CLEANUP_ALL);
					return;

				default:
					d->Send("Invalid choice!\r\nDo you wish to save the shop? : ");
					return;
			}

			break;

			/*-------------------------------------------------------------------*/
		case SEDIT_MAIN_MENU:
			i = 0;

			switch (*arg)
			{
				case 'q':
				case 'Q':
					/***
					if (OLC_VAL(d))
					{
						d->Send("Do you wish to save the changes to the shop? (y/n) : ");
						OLC_MODE(d) = SEDIT_CONFIRM_SAVESTRING;
					}

					else
						cleanup_olc(d, CLEANUP_ALL);
					***/
					sedit_save_internally(d);
					sedit_save_to_disk();
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE,
					       "OLC: %s edits shop %d", GET_NAME(d->character), OLC_NUM(d));
					cleanup_olc(d, CLEANUP_ALL);

					return;

				case '0':
					OLC_MODE(d) = SEDIT_KEEPER;
					d->Send("Enter virtual number of shop keeper : ");
					return;

				case '1':
					OLC_MODE(d) = SEDIT_OPEN1;
					i++;
					break;

				case '2':
					OLC_MODE(d) = SEDIT_CLOSE1;
					i++;
					break;

				case '3':
					OLC_MODE(d) = SEDIT_OPEN2;
					i++;
					break;

				case '4':
					OLC_MODE(d) = SEDIT_CLOSE2;
					i++;
					break;

				case '5':
					OLC_MODE(d) = SEDIT_BUY_PROFIT;
					i++;
					break;

				case '6':
					OLC_MODE(d) = SEDIT_SELL_PROFIT;
					i++;
					break;

				case '7':
					OLC_MODE(d) = SEDIT_NOITEM1;
					i--;
					break;

				case '8':
					OLC_MODE(d) = SEDIT_NOITEM2;
					i--;
					break;

				case '9':
					OLC_MODE(d) = SEDIT_NOCASH1;
					i--;
					break;

				case 'a':
				case 'A':
					OLC_MODE(d) = SEDIT_NOCASH2;
					i--;
					break;

				case 'b':
				case 'B':
					OLC_MODE(d) = SEDIT_NOBUY;
					i--;
					break;

				case 'c':
				case 'C':
					OLC_MODE(d) = SEDIT_BUY;
					i--;
					break;

				case 'd':
				case 'D':
					OLC_MODE(d) = SEDIT_SELL;
					i--;
					break;

				case 'e':
				case 'E':
					sedit_no_trade_menu(d);
					return;

				case 'f':
				case 'F':
					sedit_shop_flags_menu(d);
					return;

				case 'r':
				case 'R':
					sedit_rooms_menu(d);
					return;

				case 'p':
				case 'P':
					sedit_products_menu(d);
					return;

				case 't':
				case 'T':
					sedit_namelist_menu(d);
					return;

				default:
					sedit_disp_menu(d);
					return;
			}

			if (i != 0)
			{
				d->Send(i == 1 ? "\r\nEnter new value : " : (i == -1 ? "\r\nEnter new text :\r\n] " : "Oops...\r\n"));
				return;
			}

			break;
			/*-------------------------------------------------------------------*/

		case SEDIT_NAMELIST_MENU:

			switch (*arg)
			{

				case 'a':
				case 'A':
					sedit_types_menu(d);
					return;

				case 'd':
				case 'D':
					d->Send("\r\nDelete which entry? : ");
					OLC_MODE(d) = SEDIT_DELETE_TYPE;
					return;

				case 'q':
				case 'Q':
					break;
			}

			break;
			/*-------------------------------------------------------------------*/

		case SEDIT_PRODUCTS_MENU:
			switch (*arg)
			{
				case 'a':
				case 'A':
					d->Send("\r\nEnter new product virtual number : ");
					OLC_MODE(d) = SEDIT_NEW_PRODUCT;
					return;

				case 'd':
				case 'D':
					d->Send("\r\nDelete which product? : ");
					OLC_MODE(d) = SEDIT_DELETE_PRODUCT;
					return;

				case 'q':
				case 'Q':
					break;
			}

			break;
			/*-------------------------------------------------------------------*/

		case SEDIT_ROOMS_MENU:
			switch (*arg)
			{
				case 'a':
				case 'A':
					d->Send("\r\nEnter new room virtual number : ");
					OLC_MODE(d) = SEDIT_NEW_ROOM;
					return;

				case 'c':
				case 'C':
					sedit_compact_rooms_menu(d);
					return;

				case 'l':
				case 'L':
					sedit_rooms_menu(d);
					return;

				case 'd':
				case 'D':
					d->Send("\r\nDelete which room? : ");
					OLC_MODE(d) = SEDIT_DELETE_ROOM;
					return;

				case 'q':
				case 'Q':
					break;
			}

			break;
			/*-------------------------------------------------------------------*/
			/*
			 * String edits.
			 */
		case SEDIT_NOITEM1:
			sedit_modify_string(OLC_SHOP(d)->no_such_item1, arg);
			break;

		case SEDIT_NOITEM2:
			sedit_modify_string(OLC_SHOP(d)->no_such_item2, arg);
			break;

		case SEDIT_NOCASH1:
			sedit_modify_string(OLC_SHOP(d)->missing_cash1, arg);
			break;

		case SEDIT_NOCASH2:
			sedit_modify_string(OLC_SHOP(d)->missing_cash2, arg);
			break;

		case SEDIT_NOBUY:
			sedit_modify_string(OLC_SHOP(d)->do_not_buy, arg);
			break;

		case SEDIT_BUY:
			sedit_modify_string(OLC_SHOP(d)->message_buy, arg);
			break;

		case SEDIT_SELL:
			sedit_modify_string(OLC_SHOP(d)->message_sell, arg);
			break;

		case SEDIT_NAMELIST:
			{
				class ShopBuyData new_entry;

				new_entry.type = OLC_VAL(d);
				new_entry.keywords = (arg && *arg) ? arg : "";
				d->olc->shop->Type[ OLC_VAL(d) ] = new_entry;
			}

			sedit_namelist_menu(d);
			return;

			/*-------------------------------------------------------------------*/
			/*
			 * Numerical responses.
			 */

		case SEDIT_KEEPER:
			i = atoi(arg);

			if ((i = atoi(arg)) != -1)
				if ((i = MobManager::GetManager().RealMobile(i)) < 0)
				{
					d->Send("That mobile does not exist, try again : ");
					return;
				}

			S_KEEPER(OLC_SHOP(d)) = i;

			if (i == -1)
				break;

			/*
			 * Fiddle with special procs.
			 */

			S_FUNC(OLC_SHOP(d)) = MobManager::GetManager().GetIndex((u_int)i)->func;
			MobManager::GetManager().GetIndex((u_int)i)->func = shop_keeper;
			break;

		case SEDIT_OPEN1:
			S_OPEN1(OLC_SHOP(d)) = MAX(0, MIN(28, atoi(arg)));
			break;

		case SEDIT_OPEN2:
			S_OPEN2(OLC_SHOP(d)) = MAX(0, MIN(28, atoi(arg)));
			break;

		case SEDIT_CLOSE1:
			S_CLOSE1(OLC_SHOP(d)) = MAX(0, MIN(28, atoi(arg)));
			break;

		case SEDIT_CLOSE2:
			S_CLOSE2(OLC_SHOP(d)) = MAX(0, MIN(28, atoi(arg)));
			break;

		case SEDIT_BUY_PROFIT:
			sscanf(arg, "%f", &S_BUYPROFIT(OLC_SHOP(d)));
			break;

		case SEDIT_SELL_PROFIT:
			sscanf(arg, "%f", &S_SELLPROFIT(OLC_SHOP(d)));
			break;

		case SEDIT_TYPE_MENU:
			OLC_VAL(d) = MAX(0, MIN(NUM_ITEM_TYPES - 1, atoi(arg)));
			d->Send("Enter namelist (return for none) :-\r\n] ");
			OLC_MODE(d) = SEDIT_NAMELIST;
			return;

		case SEDIT_DELETE_TYPE:

			if( toupper(arg[0]) == 'Q' )
			{
				sedit_namelist_menu(d);
				return;
			}
			if( atoi(arg) >= 0 )
			{
				T = d->olc->shop->Type.begin();
				for(i = 0;i < atoi(arg) && T != d->olc->shop->Type.end();++i, ++T);

				if(T != d->olc->shop->Type.end())
				{
					d->olc->shop->Type.erase( T );
					sedit_namelist_menu(d);
				}
				else
					d->Send("That type does not exist. Try again : ");
			}
			else
				d->Send("Value must be above zero. Try again : ");
			return;

		case SEDIT_NEW_PRODUCT:
			if ((i = atoi(arg)) != -1)
				if ((i = real_object(i)) == -1)
				{
					d->Send("That object does not exist, try again : ");
					return;
				}

			if (i > 0)
				d->olc->shop->producing.push_back( i );

			sedit_products_menu(d);
			return;

		case SEDIT_DELETE_PRODUCT:
		{
			if(atoi(arg) < 0) iter = d->olc->shop->producing.end();
			else
			{
				int n = 0;
				for(iter = d->olc->shop->producing.begin();iter != d->olc->shop->producing.end() && n < atoi(arg);
					n++,iter++);
			}
			
			if( iter != d->olc->shop->producing.end() )
				d->olc->shop->producing.erase( iter );
			sedit_products_menu(d);
			return;
		}
		case SEDIT_NEW_ROOM:
			if ((i = atoi(arg)) != -1)
				if ((i = real_room(i)) < 0)
				{
					d->Send("That room does not exist, try again : ");
					return;
				}

			if (i >= 0)
				d->olc->shop->in_room.push_back( atoi(arg) );

			sedit_rooms_menu(d);
			return;

		case SEDIT_DELETE_ROOM:

			if(atoi(arg) > 0 && (unsigned int)atoi(arg) <= d->olc->shop->in_room.size())
			{
				iter = d->olc->shop->in_room.begin();
				for(i = 1;iter != d->olc->shop->in_room.end() && i < atoi(arg);++i, ++iter);

				if(iter != d->olc->shop->in_room.end())
					d->olc->shop->in_room.erase( iter );
			}
			sedit_rooms_menu(d);
			return;

		case SEDIT_SHOP_FLAGS:
			if ((i = MAX(0, MIN(NUM_SHOP_FLAGS, atoi(arg)))) > 0)
			{
				TOGGLE_BIT(S_BITVECTOR(OLC_SHOP(d)), 1 << (i - 1));
				sedit_shop_flags_menu(d);
				return;
			}

			break;

		case SEDIT_NOTRADE:
			if ((i = MAX(0, MIN(NUM_TRADERS, atoi(arg)))) > 0)
			{
				TOGGLE_BIT(S_NOTRADE(OLC_SHOP(d)), 1 << (i - 1));
				sedit_no_trade_menu(d);
				return;
			}

			break;

			/*-------------------------------------------------------------------*/
		default:
			/*
			 * We should never get here.
			 */

			cleanup_olc(d, CLEANUP_ALL);
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: OLC: sedit_parse(): Reached default case!");
			d->Send("Oops...\r\n");
			break;
	}

	/*-------------------------------------------------------------------*/

	/*
	 * END OF CASE 
	 * If we get here, we have probably changed something, and now want to
	 * return to main menu.  Use OLC_VAL as a 'has changed' flag.
	 */
	OLC_VAL(d) = 1;
	sedit_disp_menu(d);
}




















