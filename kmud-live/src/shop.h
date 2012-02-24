/* ************************************************************************
*   File: shop.h                                        Part of CircleMUD *
*  Usage: shop file definitions, structures, constants                    *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


class ShopBuyData
{
public:
	int type;
	std::string keywords;

	ShopBuyData()
	{
		type = -1;
	}
};

#define BUY_TYPE(i)		((i).type)
#define BUY_WORD(i)		((i).keywords)


class Shop
{
	public:
		int	 vnum;						/* Virtual number of this shop			*/
		std::vector<int> producing;		/* Which item to produce (virtual)		*/
		float profit_buy;				/* Factor to multiply cost with			*/
		float profit_sell;				/* Factor to multiply cost with			*/
		std::map<int, ShopBuyData> Type;/* Which items to trade					*/
		std::string	no_such_item1;		/* Message if keeper hasn't got an item	*/
		std::string	no_such_item2;		/* Message if player hasn't got an item	*/
		std::string	missing_cash1;		/* Message if keeper hasn't got cash	*/
		std::string	missing_cash2;		/* Message if player hasn't got cash	*/
		std::string	do_not_buy;			/* If keeper dosn't buy such things		*/
		std::string	message_buy;		/* Message when player buys item		*/
		std::string	message_sell;		/* Message when player sells item		*/
		int	 temper1;					/* How does keeper react if no money	*/
		int	 bitvector;					/* Can attack? Use bank? Cast here?		*/
		int	 keeper;					/* The mobil who owns the shop (virtual)*/
		int	 with_who;					/* Who does the shop trade with?		*/
		std::vector<int> in_room;		/* Where is the shop?					*/
		int	 open1, open2;				/* When does the shop open?				*/
		int	 close1, close2;			/* When does the shop close?			*/
		int	 bankAccount;				/* Store all gold over 15000 (disabled)	*/
		int	 lastsort;					/* How many items are sorted in inven?	*/
		SPECIAL (*func);				/* Secondary spec_proc for shopkeeper	*/

		Shop();
		void BootTypes( FILE* file );
		void BootProducing( FILE* file );
		~Shop();
};


const int MAX_TRADE = 5; // List maximums for compatibility 
const int MAX_PROD = 5; // with shops before v3.0 
#define VERSION3_TAG	"v3.0"	/* The file has v3.0 shops in it!	*/
const int MAX_SHOP_OBJ = 100; // "Soft" maximum for list maximums 


/* Pretty general macros that could be used elsewhere */
#define IS_GOD(ch)		(!IS_NPC(ch) && (GET_LEVEL(ch) >= LVL_GOD))
#define GET_OBJ_NUM(obj)	(obj->item_number)
#define END_OF(buffer)		((buffer) + strlen((buffer)))


/* Possible states for objects trying to be sold */
const int OBJECT_DEAD = 0; 
const int OBJECT_NOTOK = 1; 
const int OBJECT_OK = 2; 
const int OBJECT_NOVAL = 3; 


/* Types of lists to read */
const int LIST_PRODUCE = 0; 
const int LIST_TRADE = 1; 
const int LIST_ROOM = 2; 


/* Whom will we not trade with (bitvectorfor SHOP_TRADE_WITH()) */
const int TRADE_NOGOOD = 1; 
const int TRADE_NOEVIL = 2; 
const int TRADE_NONEUTRAL = 4; 
const int TRADE_NOMAGIC_USER = 8; 
const int TRADE_NOCLERIC = 16; 
const int TRADE_NOTHIEF = 32; 
const int TRADE_NOWARRIOR = 64; 


struct stack_data
{
	int data[100];
	int len;
} ;

#define S_DATA(stack, index)	((stack)->data[(index)])
#define S_LEN(stack)		((stack)->len)


/* Which expression type we are now parsing */
const int OPER_OPEN_PAREN = 0; 
const int OPER_CLOSE_PAREN = 1; 
const int OPER_OR = 2; 
const int OPER_AND = 3; 
const int OPER_NOT = 4; 
const int MAX_OPER = 4; 


#define SHOP_NUM(i)		(shop_index[(i)].vnum)
#define SHOP_KEEPER(i)		(shop_index[(i)].keeper)
#define SHOP_OPEN1(i)		(shop_index[(i)].open1)
#define SHOP_CLOSE1(i)		(shop_index[(i)].close1)
#define SHOP_OPEN2(i)		(shop_index[(i)].open2)
#define SHOP_CLOSE2(i)		(shop_index[(i)].close2)
#define SHOP_ROOM(i, num)	(shop_index[(i)].in_room[(num)])
#define SHOP_BUYTYPE(i, num)	(BUY_TYPE(shop_index[(i)].type[(num)]))
#define SHOP_BUYWORD(i, num)	(BUY_WORD(shop_index[(i)].type[(num)]))
#define SHOP_PRODUCT(i, num)	(shop_index[(i)].producing[(num)])
#define SHOP_BANK(i)		(shop_index[(i)].bankAccount)
#define SHOP_BROKE_TEMPER(i)	(shop_index[(i)].temper1)
#define SHOP_BITVECTOR(i)	(shop_index[(i)].bitvector)
#define SHOP_TRADE_WITH(i)	(shop_index[(i)].with_who)
#define SHOP_SORT(i)		(shop_index[(i)].lastsort)
#define SHOP_BUYPROFIT(i)	(shop_index[(i)].profit_buy)
#define SHOP_SELLPROFIT(i)	(shop_index[(i)].profit_sell)
#define SHOP_FUNC(i)		(shop_index[(i)].func)

#define NOTRADE_GOOD(i)		(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOGOOD))
#define NOTRADE_EVIL(i)		(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOEVIL))
#define NOTRADE_NEUTRAL(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NONEUTRAL))
#define NOTRADE_MAGIC_USER(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOMAGIC_USER))
#define NOTRADE_CLERIC(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOCLERIC))
#define NOTRADE_THIEF(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOTHIEF))
#define NOTRADE_WARRIOR(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOWARRIOR))



const int WILL_START_FIGHT = 1; 
const int WILL_BANK_MONEY = 2; 

#define SHOP_KILL_CHARS(i)	(IS_SET(SHOP_BITVECTOR(i), WILL_START_FIGHT))
#define SHOP_USES_BANK(i)	(IS_SET(SHOP_BITVECTOR(i), WILL_BANK_MONEY))

#define MSG_NOT_OPEN_YET	"Come back later!"
#define MSG_NOT_REOPEN_YET	"Sorry, we have closed, but come back later."
#define MSG_CLOSED_FOR_DAY	"Sorry, come back tomorrow."
#define MSG_NO_STEAL_HERE	"$n is a bloody thief!"
#define MSG_NO_SEE_CHAR		"I don't trade with someone I can't see!"
#define MSG_NO_SELL_ALIGN	"Get out of here before I call the guards!"
#define MSG_NO_SELL_CLASS	"We don't serve your kind here!"
#define MSG_NO_USED_WANDSTAFF	"I don't buy used up wands or staves!"
#define MSG_CANT_KILL_KEEPER	"Get out of here before I call the guards!"



















