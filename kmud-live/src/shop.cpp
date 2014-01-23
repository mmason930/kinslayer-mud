/* ************************************************************************
*   File: shop.c                                        Part of CircleMUD *
*  Usage: shopkeepers: loading config files, spec procs.                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/***
 * The entire shop rewrite for Circle 3.0 was done by Jeff Fink.  Thanks Jeff!
 ***/

#define __SHOP_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "spells.h"
#include "utils.h"
#include "shop.h"
#include "mobs.h"

#include "js_functions.h"
#include "StringUtil.h"
#include "Descriptor.h"
#include "rooms/Room.h"


// External variables
extern struct Index *obj_index;

extern struct GameTime time_info;
extern const char *drinks[];
extern const char *item_types[];
extern const char *extra_bits[];

// Forward/External function declarations
ACMD(do_tell);
ACMD(do_action);
ACMD(do_echo);
ACMD(do_say);
ACMD(do_slist);
void sort_keeper_objs(Character * keeper, int shop_nr);

// Local variables
Shop *shop_index;
int top_shop = 0;
int cmd_say, cmd_tell, cmd_emote, cmd_slap, cmd_puke;

// local functions
void shopping_list(char *arg, Character * ch, Character * keeper, int shop_nr);
void shopping_value(char *arg, Character * ch, Character * keeper, int shop_nr);
void shopping_sell(char *arg, Character * ch, Character * keeper, int shop_nr);
Object *get_selling_obj(Character * ch, char *name, Character * keeper, int shop_nr, int msg);
Object *slide_obj(Object * obj, Character * keeper, int shop_nr);
void shopping_buy(char *arg, Character * ch, Character * keeper, int shop_nr);
Object *get_purchase_obj(Character * ch, char *arg, Character * keeper, int shop_nr, int msg);
Object *get_hash_obj_vis(Character * ch, char *name, Object * listy);
Object *get_slide_obj_vis(Character * ch, char *name, Object * listy);
void boot_the_shops();
void assign_the_shopkeepers(void);
char *customer_string(int shop_nr, int detailed);
void list_all_shops(Character * ch);
void handle_detailed_list(char *buf, char *buf1, Character * ch);
void list_detailed_shop(Character * ch, int shop_nr);
void show_shops(Character * ch, char *arg);
int is_ok_char(Character * keeper, Character * ch, int shop_nr);
int is_open(Character * keeper, int shop_nr, int msg);
int is_ok(Character * keeper, Character * ch, int shop_nr);
void push(struct stack_data * stack, int pushval);
int top(struct stack_data * stack);
int pop(struct stack_data * stack);
void evaluate_operation(struct stack_data * ops, struct stack_data * vals);
int find_oper_num(char token);
int evaluate_expression(Object * obj, char *expr);
int trade_with(Object * item, int shop_nr);
int same_obj(Object * obj1, Object * obj2);
int shop_producing(Object * item, int shop_nr);
int transaction_amt(char *arg);
char *times_message(Object * obj, char *name, int num);
int buy_price(Object * obj, int shop_nr);
int sell_price(Character * ch, Object * obj, int shop_nr);
char *list_object(Character *ch, Object * obj, int cnt, int index, int shop_nr);
int ok_shop_room(int shop_nr, int room);
SPECIAL(shop_keeper);
int ok_damage_shopkeeper(Character * ch, Character * victim);
void read_line(FILE * shop_f, const char *string, void *data);


// config arrays
const char *operator_str[] =
    {
        "[({",
        "])}",
        "|+",
        "&*",
        "^'"
    } ;

// Constant list for printing out who we sell to
const char *trade_letters[] =
    {
        "Good",                 /* First, the alignment based ones */
        "Evil",
        "Neutral",
        "Channeler",           /* Then the class based ones */
        "Ranger",
        "Thief",
        "Warrior",
        "\n"
    };


const char *shop_bits[] =
    {
        "WILL_FIGHT",
        "USES_BANK",
        "\n"
    };

ACMD(do_slist)
{

	int i = 0;

	strcpy(buf, "");

	for(i = 0;i < top_shop;i++)
	{
		sprintf(buf + strlen(buf), "%d.	[ %d ]\r\n", (i + 1), shop_index[i].vnum);
	}

	if(ch->desc)
		page_string(ch->desc, buf, 1);

}

int is_ok_char(Character * keeper, Character * ch, int shop_nr)
{
	char buf[200];

	if (!(CAN_SEE(keeper, ch)))
	{
		char sSay[MAX_STRING_LENGTH];
		strcpy(sSay, MSG_NO_SEE_CHAR);
		do_say(keeper, sSay, cmd_say, 0);
		return (FALSE);
	}

	if (IS_GOD(ch))
		return (TRUE);

	if (IS_NPC(ch))
		return (TRUE);

	if (GET_RACE(ch) != GET_RACE(keeper))
	{
		sprintf(buf, "%s %s", GET_NAME(ch), MSG_NO_SELL_CLASS);
		do_tell(keeper, buf, cmd_tell, 0);
		return (FALSE);
	}

	return (TRUE);
}


int is_open(Character * keeper, int shop_nr, int msg)
{
	char buf[200];

	*buf = 0;

	if (SHOP_OPEN1(shop_nr) > time_info.hours)
		strcpy(buf, MSG_NOT_OPEN_YET);

	else if (SHOP_CLOSE1(shop_nr) < time_info.hours)
	{
		if (SHOP_OPEN2(shop_nr) > time_info.hours)
			strcpy(buf, MSG_NOT_REOPEN_YET);

		else if (SHOP_CLOSE2(shop_nr) < time_info.hours)
			strcpy(buf, MSG_CLOSED_FOR_DAY);
	}

	if (!(*buf))
		return (TRUE);

	if (msg)
		do_say(keeper, buf, cmd_tell, 0);

	return (FALSE);
}


int is_ok(Character * keeper, Character * ch, int shop_nr)
{
	if (is_open(keeper, shop_nr, TRUE))
		return (is_ok_char(keeper, ch, shop_nr));

	else
		return (FALSE);
}


void push(struct stack_data * stack, int pushval)
{
	S_DATA(stack, S_LEN(stack)++) = pushval;
}


int top(struct stack_data * stack)
{
	if (S_LEN(stack) > 0)
		return (S_DATA(stack, S_LEN(stack) - 1));

	else
		return (NOTHING);
}


int pop(struct stack_data * stack)
{
	if (S_LEN(stack) > 0)
		return (S_DATA(stack, --S_LEN(stack)));

	else
	{
		Log("Illegal expression %d in shop keyword list.", S_LEN(stack));
		return (0);
	}
}


void evaluate_operation(struct stack_data * ops, struct stack_data * vals)
{
	int oper;

	if ((oper = pop(ops)) == OPER_NOT)
		push(vals, !pop(vals));

	else if (oper == OPER_AND)
		push(vals, pop(vals) && pop(vals));

	else if (oper == OPER_OR)
		push(vals, pop(vals) || pop(vals));
}


int find_oper_num(char token)
{
	int index;

	for (index = 0; index <= MAX_OPER; index++)
		if (strchr(operator_str[index], token))
			return (index);

	return (NOTHING);
}

bool ShopTradeObjByName(Object *obj, char *expr)
{
	char expr_cpy[MAX_INPUT_LENGTH], name_cpy[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH];

	strcpy(expr_cpy, expr);
	strcpy(name_cpy, obj->name);

	while( *name_cpy )
	{
		HalfChop(name_cpy, name, name_cpy);

		if(is_name( name, expr_cpy ))
			return true;
	}

	return false;
}

int evaluate_expression(Object * obj, char *expr)
{
	struct stack_data ops, vals;
	char *ptr, *end, name[200];
	int temp, index;

	if (!expr)
		return TRUE;

	if (!isalpha(*expr))
		return TRUE;

	ops.len = vals.len = 0;
	ptr = expr;

	while (*ptr)
	{

		if (isspace(*ptr))
			++ptr;

		else
		{
			if ((temp = find_oper_num(*ptr)) == NOTHING)
			{
				end = ptr;

				while (*ptr && !isspace(*ptr) && (find_oper_num(*ptr) == NOTHING))
					++ptr;

				strncpy(name, end, ptr - end);
				name[ptr - end] = 0;

				for (index = 0; *extra_bits[index] != '\n'; ++index)
					if (!str_cmp(name, extra_bits[index]))
					{
						push(&vals, IS_SET(GET_OBJ_EXTRA(obj), 1 << index));
						break;
					}

				if (*extra_bits[index] == '\n')
					push(&vals, isname(name, obj->name));
			}

			else
			{
				if (temp != OPER_OPEN_PAREN)
					while (top(&ops) > temp)
						evaluate_operation(&ops, &vals);


				if (temp == OPER_CLOSE_PAREN)
				{
					if ((temp = pop(&ops)) != OPER_OPEN_PAREN)
					{
						Log("Illegal parenthesis in shop keyword expression.");
						return (FALSE);
					}
				}

				else
					push(&ops, temp);

				ptr++;
			}
		}
	}

	while (top(&ops) != NOTHING)
		evaluate_operation(&ops, &vals);

	temp = pop(&vals);

	if (top(&vals) != NOTHING)
	{
		Log("Extra operands left on shop keyword expression stack.");
		return (FALSE);
	}

	return (temp);
}


int trade_with(Object * item, int shop_nr)
{
	if (GET_OBJ_COST(item) < 1)
		return (OBJECT_NOVAL);

	//If the object is flagged NO SELL, return that it is not ok.
	if(IS_OBJ_STAT(item, ITEM_NOSELL))
		return (OBJECT_NOTOK);

	if(shop_index[shop_nr].Type.find(item->getType()) != shop_index[shop_nr].Type.end()) //Type exists
	{
		if(ShopTradeObjByName(item, (char*)shop_index[shop_nr].Type[item->getType()].keywords.c_str()))
			return (OBJECT_OK);
	}
	return (OBJECT_NOTOK);
}


int same_obj(Object * obj1, Object * obj2)
{
	int index;

	if (!obj1 || !obj2)
		return (obj1 == obj2);

	if (GET_OBJ_RNUM(obj1) != GET_OBJ_RNUM(obj2))
		return (FALSE);

	if (GET_OBJ_COST(obj1) != GET_OBJ_COST(obj2))
		return (FALSE);

	if (GET_OBJ_EXTRA(obj1) != GET_OBJ_EXTRA(obj2))
		return (FALSE);

	for (index = 0; index < MAX_OBJ_AFFECT; index++)
		if ((obj1->affected[index].location != obj2->affected[index].location) ||
		        (obj1->affected[index].modifier != obj2->affected[index].modifier))
			return (FALSE);

	return (TRUE);
}


int shop_producing(Object *item, int shop_nr)
{
	unsigned int counter;

	if (GET_OBJ_RNUM(item) < 0)
		return (FALSE);

	for (counter = 0; counter < shop_index[shop_nr].producing.size(); ++counter)
		if (same_obj(item, obj_proto[ shop_index[shop_nr].producing[counter] ]))
			return (TRUE);

	return (FALSE);
}


int transaction_amt(char *arg)
{
	int num;

	OneArgument(arg, buf);

	if (*buf)
		if ((IsNumber(buf)))
		{
			num = atoi(buf);
			strcpy(arg, arg + strlen(buf) + 1);
			return (num);
		}

	return (1);
}


char *times_message(Object *obj, char *name, int num)
{
	static char buf[256];
	char *ptr;

	if (obj)
		strcpy(buf, obj->GetSDesc());

	else
	{
		if ((ptr = strchr(name, '.')) == NULL)
			ptr = name;

		else
			ptr++;

		sprintf(buf, "%s %s", AN(ptr), ptr);
	}

	if (num > 1)
		sprintf(END_OF(buf), " (x %d)", num);

	return (buf);
}

Object *get_slide_obj_vis(Character * ch, char *name, Object * listy)
{
	Object *i = 0, *last_match = 0;
	int j = 0, num = 0;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;

	strcpy(tmpname, name);
	tmp = tmpname;

	if (!(num = GetNumber(&tmp)))
		return (0);

	for (i = listy, j = 1; i && (j <= num); i = i->next_content)
	{
		if (isname(tmp, i->name) && CAN_SEE_OBJ(ch, i) && !same_obj(last_match, i))
		{
			if (j == num)
				return (i);

			last_match = i;
			++j;
		}
	}
	return (0);
}

Object *get_hash_obj_vis(Character * ch, char *name, Object * listy)
{
	Object *loop, *last_obj = 0;
	int index;

	if ((IsNumber(name + 1)))
		index = atoi(name + 1);

	else
		return (0);

	for (loop = listy; loop; loop = loop->next_content)
		if (CAN_SEE_OBJ(ch, loop) && (loop->obj_flags.cost > 0))
			if (!same_obj(last_obj, loop))
			{
				if (--index == 0)
					return (loop);

				last_obj = loop;
			}

	return (0);
}


Object *get_purchase_obj(Character *ch, char *arg, Character *keeper, int shop_nr, int msg)
{
	char buf[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH];
	Object *obj;

	OneArgument(arg, name);
	do
	{
		if(*name == '#')
			obj = get_hash_obj_vis(ch, name, keeper->carrying);
		else
			obj = get_slide_obj_vis(ch, name, keeper->carrying);
		if (!obj)
		{
			if(msg)
			{
				sprintf(buf, shop_index[shop_nr].no_such_item1.c_str(), GET_NAME(ch));
				do_tell(keeper, buf, cmd_tell, 0);
			}
			return (0);
		}
		if (GET_OBJ_COST(obj) <= 0)
		{
			obj->Extract(true);
			obj = 0;
		}
	}
	while (!obj);
	return (obj);
}


int buy_price(Object *obj, int shop_nr)
{
	return ((int) (GET_OBJ_COST(obj) * SHOP_BUYPROFIT(shop_nr)));
}


void shopping_buy(char *arg, Character *ch, Character *keeper, int shop_nr)
{
	char tempstr[200], buf[MAX_STRING_LENGTH];
	Object *obj, *last_obj = NULL;
	int goldamt = 0, buynum, bought = 0;

	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);

	if ((buynum = transaction_amt(arg)) < 0)
	{
		sprintf(buf, "%s A negative amount?  Try selling me something.", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	if (!(*arg) || !(buynum))
	{
		sprintf(buf, "%s What do you want to buy??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	if (!(obj = get_purchase_obj(ch, arg, keeper, shop_nr, TRUE)))
		return;

	if ((buy_price(obj, shop_nr) > ch->points.gold) && !IS_GOD(ch))
	{
		sprintf(buf, shop_index[shop_nr].missing_cash2.c_str(), GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);

		switch (SHOP_BROKE_TEMPER(shop_nr))
		{

			case 0:
				do_action(keeper, (char*)GET_NAME(ch), cmd_puke, 0);
				return;
			case 1:
			{
				char sEcho[32];
				strcpy(sEcho, "smokes on his joint.");
				keeper->InterpCommand("emote %s", sEcho);
				return;
			}
			default:
				return;
		}
	}

	if ((IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch)))
	{
		ch->send("%s: You can't carry any more items.\r\n", fname(obj->name));
		return;
	}

	if ((ch->CarriedWeight() + obj->Weight()) > CAN_CARRY_W(ch))
	{
		ch->send("%s: You can't carry that much weight.\r\n", fname(obj->name));
		return;
	}

	while ((obj) && ((ch->points.gold >= buy_price(obj, shop_nr)) || IS_GOD(ch))
	        && (IS_CARRYING_N(ch) < CAN_CARRY_N(ch)) && (bought < buynum)
	        && (ch->CarriedWeight() + obj->Weight() <= CAN_CARRY_W(ch)))
	{
		++bought;
		// Test if producing shop !

		if (shop_producing(obj, shop_nr))
		{
			obj = read_object(GET_OBJ_RNUM(obj), REAL, true);
			sprintf(obj->creator, "Shopkeeper %s", GET_NAME(keeper));
			if( !obj->IsPurged() ) {
				js_load_triggers(obj);
			}
		}
		else
		{
			obj_from_char(obj);
			--SHOP_SORT(shop_nr);
		}

		obj_to_char(obj, ch);
		goldamt += buy_price(obj, shop_nr);

		if (!IS_GOD(ch))
			ch->points.gold -= buy_price(obj, shop_nr);

		last_obj = obj;
		obj = get_purchase_obj(ch, arg, keeper, shop_nr, FALSE);

		if (!same_obj(obj, last_obj))
			break;
	}

	if (bought < buynum)
	{
		if (!obj || !same_obj(last_obj, obj))
			sprintf(buf, "%s I only have %d to sell you.", GET_NAME(ch), bought);
		else if (ch->points.gold < buy_price(obj, shop_nr))
			sprintf(buf, "%s You can only afford %d.", GET_NAME(ch), bought);
		else if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
			sprintf(buf, "%s You can only hold %d.", GET_NAME(ch), bought);
		else if (ch->CarriedWeight() + obj->Weight() > CAN_CARRY_W(ch))
			sprintf(buf, "%s You can only carry %d.", GET_NAME(ch), bought);
		else
			sprintf(buf, "%s Something screwy only gave you %d.", GET_NAME(ch),
			        bought);

		do_tell(keeper, buf, cmd_tell, 0);
	}

	if (!IS_GOD(ch))
		keeper->points.gold += goldamt;

	sprintf(tempstr, "%s", times_message(ch->carrying, 0, bought));
	sprintf(buf, "$n buys %s.", tempstr);
	Act(buf, FALSE, ch, obj, 0, TO_ROOM);

	sprintf(buf, shop_index[shop_nr].message_buy.c_str(), GET_NAME(ch), goldamt);
	do_tell(keeper, buf, cmd_tell, 0);
	ch->send("You now have %s.\r\n", tempstr);
}

Object *get_selling_obj(Character *ch, char *name, Character *keeper, int shop_nr, int msg)
{
	char buf[MAX_STRING_LENGTH];
	Object *obj;
	int result;

	if (!(obj = get_obj_in_list_vis(ch, name, ch->carrying)))
	{
		if (msg)
		{
			sprintf(buf, shop_index[shop_nr].no_such_item2.c_str(), GET_NAME(ch));
			do_tell(keeper, buf, cmd_tell, 0);
		}
		return (0);
	}

	if ((result = trade_with(obj, shop_nr)) == OBJECT_OK)
		return (obj);

	switch (result)
	{
		case OBJECT_NOVAL:
		case OBJECT_NOTOK:
		case OBJECT_DEAD:
			sprintf(buf, shop_index[shop_nr].do_not_buy.c_str(), GET_NAME(ch));
			break;
		default:
			Log("SYSERR: Illegal return value of %d from trade_with() (%s)",
			    result, __FILE__);	/* Someone might rename it... */
			sprintf(buf, "%s An error has occurred.", GET_NAME(ch));
			break;
	}
	if (msg)
		do_tell(keeper, buf, cmd_tell, 0);
	return (0);
}


int sell_price(Character * ch, Object * obj, int shop_nr)
{
	return ((int) (GET_OBJ_COST(obj) * SHOP_SELLPROFIT(shop_nr)));
}


Object *slide_obj(Object * obj, Character * keeper, int shop_nr)
/*
   This function is a slight hack!  To make sure that duplicate items are
   only listed once on the "list", this function groups "identical"
   objects together on the shopkeeper's inventory list.  The hack involves
   knowing how the list is put together, and manipulating the order of
   the objects on the list.  (But since most of DIKU is not encapsulated,
   and information hiding is almost never used, it isn't that big a deal) -JF
*/
{
	Object *loop;
	int temp;

	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);

	// Extract the object if it is identical to one produced
	if (shop_producing(obj, shop_nr))
	{
		temp = GET_OBJ_RNUM(obj);
		obj->Extract(true);
		return (obj_proto[temp]);
	}

	++SHOP_SORT(shop_nr);
	loop = keeper->carrying;
	obj_to_char(obj, keeper);
	keeper->carrying = loop;

	while (loop)
	{
		if (same_obj(obj, loop))
		{
			obj->next_content = loop->next_content;
			loop->next_content = obj;
			return (obj);
		}

		loop = loop->next_content;
	}

	keeper->carrying = obj;
	return (obj);
}


void sort_keeper_objs(Character *keeper, int shop_nr)
{
	Object *list = 0, *temp;

	while (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
	{
		temp = keeper->carrying;
		obj_from_char(temp);
		temp->next_content = list;
		list = temp;
	}

	while (list)
	{
		temp = list;
		list = list->next_content;

		if ((shop_producing(temp, shop_nr)) &&
		        !(get_obj_in_list_num(GET_OBJ_RNUM(temp), keeper->carrying)))
		{

			obj_to_char(temp, keeper);
			SHOP_SORT(shop_nr)++;
		}

		else
			(void) slide_obj(temp, keeper, shop_nr);
	}
}

void shopping_sell(char *arg, Character *ch, Character *keeper, int shop_nr)
{
	char tempstr[200], buf[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH];
	Object *obj, *tag = 0;
	int sellnum, sold = 0, goldamt = 0;

	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	if ((sellnum = transaction_amt(arg)) < 0)
	{
		sprintf(buf, "%s A negative amount?  Try buying something.", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	if (!(*arg) || !(sellnum))
	{
		sprintf(buf, "%s What do you want to sell??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	OneArgument(arg, name);

	if (!(obj = get_selling_obj(ch, name, keeper, shop_nr, TRUE)))
		return;

	if (keeper->points.gold < sell_price(ch, obj, shop_nr))
	{
		sprintf(buf, "tell %s %s", GET_NAME(ch), shop_index[shop_nr].missing_cash1.c_str());
		keeper->InterpCommand(buf, GET_NAME(ch));
		return;
	}

	while ((obj) && (keeper->points.gold >= sell_price(ch, obj, shop_nr)) && (sold < sellnum))
	{
		++sold;

		goldamt += sell_price(ch, obj, shop_nr);
		keeper->points.gold -= sell_price(ch, obj, shop_nr);

		obj_from_char(obj);
		tag = slide_obj(obj, keeper, shop_nr);
		obj = get_selling_obj(ch, name, keeper, shop_nr, FALSE);
	}

	if (sold < sellnum)
	{
		if (!obj)
			sprintf(buf, "%s You only have %d of those.", GET_NAME(ch), sold);
		else if (keeper->points.gold < sell_price(ch, obj, shop_nr))
			sprintf(buf, "%s I can only afford to buy %d of those.", GET_NAME(ch), sold);
		else
			sprintf(buf, "%s Something really screwy made me buy %d.", GET_NAME(ch), sold);

		do_tell(keeper, buf, cmd_tell, 0);
	}

	ch->points.gold += goldamt;
	strcpy(tempstr, times_message(0, name, sold));
	sprintf(buf, "$n sells %s.", tempstr);
	Act(buf, FALSE, ch, obj, 0, TO_ROOM);

	sprintf(buf, shop_index[shop_nr].message_sell.c_str(), GET_NAME(ch), goldamt);
	do_tell(keeper, buf, 0, 0);
	ch->send("You sell %s\r\n", tempstr);
	ch->send("The shopkeeper now has %s.\r\n", tempstr);
	Act("$n tucks some coins into $s pockets.", FALSE, keeper, 0, 0, TO_ROOM);
}


void shopping_value(char *arg, Character *ch, Character *keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH];
	Object *obj;
	char name[MAX_INPUT_LENGTH];

	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	if (!(*arg))
	{
		sprintf(buf, "%s What do you want me to evaluate??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	OneArgument(arg, name);

	if (!(obj = get_selling_obj(ch, name, keeper, shop_nr, TRUE)))
		return;

	sprintf(buf, "%s I'll give you %d gold coins for that!", GET_NAME(ch),
	        sell_price(ch, obj, shop_nr));
	do_tell(keeper, buf, cmd_tell, 0);

	return;
}

char *list_object(Character *ch, Object * obj, int cnt, int index, int shop_nr)
{
	static char buf[256];
	char buf2[300], buf3[200];

	if (shop_producing(obj, shop_nr))
		strcpy(buf2, "Unlimited   ");
	else
		sprintf(buf2, "%5d       ", cnt);

	sprintf(buf, " %2d)  %s", index, buf2);

	/* Compile object name and information */
	strcpy(buf3, obj->GetSDesc());

	if ((obj->getType() == ITEM_DRINKCON) && (GET_OBJ_VAL(obj, 1)))
		sprintf(END_OF(buf3), " of %s", drinks[obj->GetTotalVal0()]);

	/* FUTURE: */
	/* Add glow/hum/etc */

	sprintf(buf2, "%-46s %-2lu %-2lu %-2lu\r\n", buf3, CalcGold(buy_price(obj, shop_nr)),
	        CalcSilver(buy_price(obj, shop_nr)), CalcCopper(buy_price(obj, shop_nr)));
	strcat(buf, StringUtil::cap(buf2));
	return (buf);
}

void shopping_list(char *arg, Character *ch, Character *keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH];
	Object *obj, *last_obj = 0;
	int cnt = 0, index = 0;

	if (!(is_ok(keeper, ch, shop_nr)))
		return;

	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);

	OneArgument(arg, name);
	strcpy(buf, " ##   Available   Item                                           G  S  C\r\n");
	strcat(buf, "-------------------------------------------------------------------------\r\n");

	if (keeper->carrying)
		for (obj = keeper->carrying; obj; obj = obj->next_content)
			if (CAN_SEE_OBJ(ch, obj) && (obj->obj_flags.cost > 0))
			{

				if (!last_obj)
				{
					last_obj = obj;
					cnt = 1;
				}

				else if (same_obj(last_obj, obj))
					++cnt;

				else
				{
					++index;

					if (!(*name) || isname(name, last_obj->name))
						strcat(buf, list_object(ch, last_obj, cnt, index, shop_nr));

					cnt = 1;
					last_obj = obj;
				}
			}

	++index;

	if (!last_obj)
	{

		if (*name)
			strcpy(buf, "Presently, none of those are for sale.\r\n");

		else
			strcpy(buf, "Currently, there is nothing for sale.\r\n");
	}

	else if (!(*name) || isname(name, last_obj->name))
		strcat(buf, list_object(ch, last_obj, cnt, index, shop_nr));

	page_string(ch->desc, buf, 1);
}


int ok_shop_room(int shop_nr, int room)
{
	unsigned int index;

	for (index = 0;index < shop_index[shop_nr].in_room.size(); ++index)
		if ( shop_index[shop_nr].in_room[index] == room)
			return (TRUE);

	return (FALSE);
}


SPECIAL(shop_keeper)
{
	char argm[MAX_INPUT_LENGTH];
	Character *keeper = (Character *) me;
	int shop_nr;

	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
		if (SHOP_KEEPER(shop_nr) == keeper->nr)
			break;

	if (shop_nr >= top_shop)
		return (FALSE);

	if (shop_index[shop_nr].func && shop_index[shop_nr].func != shop_keeper)	/* Check secondary function */
		if (shop_index[shop_nr].func(ch, me, cmd, arg))
			return (TRUE);

	if (keeper == ch)
	{
		if (cmd)
			SHOP_SORT(shop_nr) = 0;	/* Safety in case "drop all" */

		return (FALSE);
	}

	if (!ok_shop_room(shop_nr, ch->in_room->getVnum()))
		return (0);

	if (!AWAKE(keeper))
		return (FALSE);

	if (CMD_IS(ch, "steal"))
	{
		sprintf(argm, "$N shouts '%s'", MSG_NO_STEAL_HERE);
		do_action(keeper, (char*)GET_NAME(ch), cmd_slap, 0);
		Act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		return (TRUE);
	}

	if (CMD_IS(ch, "buy"))
	{
		shopping_buy(argument, ch, keeper, shop_nr);
		return (TRUE);
	}

	else if (CMD_IS(ch, "sell"))
	{
		shopping_sell(argument, ch, keeper, shop_nr);
		return (TRUE);
	}

	else if (CMD_IS(ch, "value"))
	{
		shopping_value(argument, ch, keeper, shop_nr);
		return (TRUE);
	}

	else if (CMD_IS(ch, "list"))
	{
		shopping_list(argument, ch, keeper, shop_nr);
		return (TRUE);
	}

	return (FALSE);
}


int ok_damage_shopkeeper(Character *ch, Character *victim)
{
	char buf[200];
	int index;

	if (IS_MOB(victim) && (MobManager::GetManager().GetIndex((u_int)victim->nr)->func == shop_keeper))
		for (index = 0; index < top_shop; index++)
			if ((GET_MOB_RNUM(victim) == SHOP_KEEPER(index)) && !SHOP_KILL_CHARS(index))
			{
				do_action(victim, (char*)GET_NAME(ch), cmd_slap, 0);
				sprintf(buf, "%s %s", GET_NAME(ch), MSG_CANT_KILL_KEEPER);
				do_tell(victim, buf, cmd_tell, 0);
				return (FALSE);
			}

	return (TRUE);
}

void read_line(FILE * shop_f, const char *str, void *data)
{
	if (!get_line(shop_f, buf) || !sscanf(buf, str, data))
	{
		Log("SYSERR: Error in shop #%d\n", SHOP_NUM(top_shop));
		exit(1);
	}
}

void Shop::BootTypes( FILE* file )
{
	char Buffer[1024];
	char Keywords[1024];
	int nr;
	
	do
	{
		char *returnValue = fgets( Buffer, sizeof(Buffer), file );

		if(returnValue == NULL)
		{
			Log("SYSERR: Shop::BootTypes : Unknown error occurred while reading file.");
			exit(1);
		}

		Keywords[0] = '\0';
		sscanf( Buffer, "%d", &nr );

		strcpy(Keywords, Buffer + ToString(nr).size());

		if( char* ptr = strchr(Keywords, (int)'\n') )
			*ptr = '\0';

		this->Type[ nr ].type = nr;
		this->Type[ nr ].keywords = Keywords;
	}
	while( nr != -1 );
	this->Type.erase( -1 );
}

void Shop::BootProducing( FILE* file )
{
	char Buffer[1024];
	int nr, vnum;

	do
	{
		char *returnValue = fgets( Buffer, sizeof(Buffer), file );

		if(returnValue == NULL)
		{
			Log("SYSERROR: Shop::BootProducing() : Unknown error occurred while reading file.");
			exit(1);
		}

		sscanf( Buffer, "%d", &nr );
		if( nr != -1 && (vnum = real_object( nr )) != -1 )
			this->producing.push_back( vnum ); /*This line has caused us to crash... */
	}
	while( nr != -1 );
}

Shop::Shop()
{
	this->vnum = 0;
	this->profit_buy = 0.00f;
	this->profit_sell = 0.00f;
	this->temper1 = 0;
	this->bitvector= 0;
	this->keeper = 0;
	this->with_who = 0;
	this->open1 = 0;
	this->open2 = 0;
	this->close1 = 0;
	this->close2 = 0;
	this->bankAccount = 0;
	this->lastsort = 0;
	this->func = 0;
}

/*
 * Free up the whole shop structure and it's content.
 */
Shop::~Shop()
{}

void boot_the_shops()
{
	sql::Query shopQuery = gameDatabase->sendQuery("SELECT * FROM shop ORDER BY id");

	sql::Query shopRoomQuery = gameDatabase->sendQuery("SELECT * FROM shopRoom ORDER BY shop_id");

	sql::Query shopBuyTypeQuery = gameDatabase->sendQuery("SELECT * FROM shopBuyType ORDER BY shop_id");

	sql::Query shopProducingQuery = gameDatabase->sendQuery("SELECT * FROM shopProducing ORDER BY shop_id,object_vnum");

	shop_index = new Shop[ shopQuery->numRows() ];

	top_shop = shopQuery->numRows();

	int shopIndex = 0;

	while(shopQuery->hasNextRow()) {

		Shop *shop = &shop_index[shopIndex];

		sql::Row shopRow = shopQuery->getRow();

		shop->vnum = shopRow.getInt("id");
		shop->profit_buy = shopRow.getFloat("profit_buy");
		shop->profit_sell = shopRow.getFloat("profit_sell");
		shop->open1 = shopRow.getInt("open_time_1");
		shop->close1 = shopRow.getInt("close_time_1");
		shop->open2 = shopRow.getInt("open_time_2");
		shop->close2 = shopRow.getInt("close_time_2");
		shop->no_such_item1 = shopRow.getString("no_such_item_1");
		shop->no_such_item2 = shopRow.getString("no_such_item_2");
		shop->do_not_buy = shopRow.getString("do_not_buy");
		shop->missing_cash1 = shopRow.getString("missing_cash_1");
		shop->missing_cash2 = shopRow.getString("missing_cash_2");
		shop->message_buy = shopRow.getString("buy");
		shop->message_sell = shopRow.getString("sell");
		shop->bitvector = shopRow.getInt("bitvector");
		shop->keeper = MobManager::GetManager().RealMobile(shopRow.getInt("keeper_vnum"));
		shop->with_who = shopRow.getInt("will_trade_with");

		//Skip all the related shop table rows until we are at or above this shop id.
		for(;shopRoomQuery->hasNextRow() && shopRoomQuery->peekRow().getInt("shop_id") < shop->vnum;shopRoomQuery->skipRow());
		for(;shopBuyTypeQuery->hasNextRow() && shopBuyTypeQuery->peekRow().getInt("shop_id") < shop->vnum;shopBuyTypeQuery->skipRow());
		for(;shopProducingQuery->hasNextRow() && shopProducingQuery->peekRow().getInt("shop_id") < shop->vnum;shopProducingQuery->skipRow());

		sql::Row shopRoomRow, shopBuyTypeRow, shopProducingRow;

		while( shopRoomQuery->hasNextRow() && (shopRoomRow = shopRoomQuery->peekRow()).getInt("shop_id") == shop->vnum ) {

			shopRoomQuery->skipRow();

			shop->in_room.push_back(shopRoomRow.getInt("room_id"));
		}

		while( shopProducingQuery->hasNextRow() && (shopProducingRow = shopProducingQuery->peekRow()).getInt("shop_id") == shop->vnum ) {

			shopProducingQuery->skipRow();

			shop->producing.push_back(shopProducingRow.getInt("object_vnum"));
		}

		while( shopBuyTypeQuery->hasNextRow() && (shopBuyTypeRow = shopBuyTypeQuery->peekRow()).getInt("shop_id") == shop->vnum ) {

			shopBuyTypeQuery->skipRow();

			int object_type = shopBuyTypeRow.getInt("object_type");

			shop->Type[object_type].type = object_type;

			shop->Type[object_type].keywords = shopBuyTypeRow.getString("keywords");
		}

		++shopIndex;
	}
}

void assign_the_shopkeepers(void)
{
	int index;

	cmd_say = FindCommand("say");
	cmd_tell = FindCommand("tell");
	cmd_emote = FindCommand("emote");
	cmd_slap = FindCommand("slap");
	cmd_puke = FindCommand("puke");

	for (index = 0; index < top_shop;++index)
	{
		Index *MI;
		if(shop_index[ index ].keeper == -1 ||
		(MI= MobManager::GetManager().GetIndex(shop_index[ index ].keeper)) == NULL)
			continue;
		if (MI->func)
			SHOP_FUNC(index) = MI->func;
		MI->func = shop_keeper;
	}
}

char *customer_string(int shop_nr, int detailed)
{
	int index, cnt = 1;
	static char buf[256];

	*buf = 0;

	for (index = 0; *trade_letters[index] != '\n'; index++, cnt *= 2)
		if (!(SHOP_TRADE_WITH(shop_nr) & cnt))
		{
			if (detailed)
			{
				if (*buf)
					strcat(buf, ", ");

				strcat(buf, trade_letters[index]);
			}

			else
				sprintf(END_OF(buf), "%c", *trade_letters[index]);
		}

		else if (!detailed)
			strcat(buf, "_");

	return (buf);
}

void list_all_shops(Character * ch)
{
	int shop_nr;

	strcpy(buf, "\r\n");

	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if (!(shop_nr % 19))
		{
			strcat(buf, " ##   Virtual   Where    Keeper    Buy   Sell   Customers\r\n");
			strcat(buf, "---------------------------------------------------------\r\n");
		}

		sprintf(buf2, "%3d   %6d   %6d    ", shop_nr + 1, SHOP_NUM(shop_nr),
			shop_index[shop_nr].in_room.size() ? shop_index[shop_nr].in_room[0] : -1);

		if (SHOP_KEEPER(shop_nr) < 0)
			strcpy(buf1, "<NONE>");

		else
			sprintf(buf1, "%6d", MobManager::GetManager().GetIndex((u_int)shop_index[shop_nr].keeper)->vnum);

		sprintf(END_OF(buf2), "%s   %3.2f   %3.2f    ", buf1,
		        SHOP_SELLPROFIT(shop_nr), SHOP_BUYPROFIT(shop_nr));
		strcat(buf2, customer_string(shop_nr, FALSE));
		sprintf(END_OF(buf), "%s\r\n", buf2);
	}

	page_string(ch->desc, buf, 1);
}

void handle_detailed_list(char *buf, char *buf1, Character * ch)
{
	if ((strlen(buf1) + strlen(buf) < 78) || (strlen(buf) < 20))
		strcat(buf, buf1);

	else
	{
		ch->send("\r\n");
		sprintf(buf, "            %s", buf1);
	}
}


void list_detailed_shop(Character * ch, int shop_nr)
{
	Object *obj;
	Character *k;
	Room *temp;
	unsigned int index;

	ch->send("Vnum:       [%5d], Rnum: [%5d]\r\n", SHOP_NUM(shop_nr), shop_nr + 1);
	strcpy(buf, "Rooms:      ");

	for (index = 0; index < shop_index[shop_nr].in_room.size(); ++index)
	{
		if (index)
			strcat(buf, ", ");
		if ((temp = FindRoomByVnum(SHOP_ROOM(shop_nr, index))))
			sprintf(buf1, "%s (#%d)", temp->getName(), temp->getVnum());
		else
			sprintf(buf1, "<UNKNOWN> (#%d)", SHOP_ROOM(shop_nr, index));
		handle_detailed_list(buf, buf1, ch);
	}
	if (!index)
		ch->send("Rooms:      None!\r\n");
	else
	{
		strcat(buf, "\r\n");
		ch->send(buf);
	}

	strcpy(buf, "Shopkeeper: ");

	if (SHOP_KEEPER(shop_nr) >= 0)
	{
		sprintf(END_OF(buf), "%s (#%d), Special Function: %s\r\n",
			GET_NAME(MobManager::GetManager().GetPrototype(SHOP_KEEPER(shop_nr))),
			MobManager::GetManager().GetIndex((u_int)shop_index[shop_nr].keeper)->vnum,
			StringUtil::allUpper(StringUtil::yesNo(SHOP_FUNC(shop_nr))).c_str());
		if ((k = get_char_num(SHOP_KEEPER(shop_nr))))
		{
			ch->send(buf);
			sprintf(buf, "Coins:      [%9d], Bank: [%9d] (Total: %d)\r\n",
			        k->points.gold, SHOP_BANK(shop_nr), (int)k->points.gold + SHOP_BANK(shop_nr));
		}
	}
	else
		strcat(buf, "<NONE>\r\n");

	ch->send(buf);

	strcpy(buf1, customer_string(shop_nr, TRUE));
	ch->send("Customers:  %s\r\n", (*buf1) ? buf1 : "None");

	strcpy(buf, "Produces:   ");

	for (index = 0;index < shop_index[shop_nr].producing.size();++index)
	{
		obj = obj_proto[SHOP_PRODUCT(shop_nr, index)];

		if (index)
			strcat(buf, ", ");

		sprintf(buf1, "%s (#%d)", obj->GetSDesc(),
		        obj_index[SHOP_PRODUCT(shop_nr, index)].vnum);
		handle_detailed_list(buf, buf1, ch);
	}

	if (!index)
		ch->send("Produces:   Nothing!\r\n");
	else
	{
		strcat(buf, "\r\n");
		ch->send(buf);
	}
	strcpy(buf, "Buys:       ");

	std::map<int,ShopBuyData>::iterator t = shop_index[shop_nr].Type.begin();
	for (index = 0; t != shop_index[shop_nr].Type.end(); ++index, ++t)
	{
		if (index)
			strcat(buf, ", ");
		sprintf(buf1, "%s (#%d) ", item_types[(*t).first],
		        (*t).first);

		if ((*t).second.keywords.size())
			sprintf(END_OF(buf1), "[%s]", (*t).second.keywords.c_str());
		else
			strcat(buf1, "[all]");
		handle_detailed_list(buf, buf1, ch);
	}

	if (!index)
		ch->send("Buys:       Nothing!\r\n");
	else
	{
		strcat(buf, "\r\n");
		ch->send(buf);
	}
	sprintf(buf, "Buy at:     [%4.2f], Sell at: [%4.2f], Open: [%d-%d, %d-%d]%s",
	        SHOP_SELLPROFIT(shop_nr), SHOP_BUYPROFIT(shop_nr), SHOP_OPEN1(shop_nr),
	        SHOP_CLOSE1(shop_nr), SHOP_OPEN2(shop_nr), SHOP_CLOSE2(shop_nr), "\r\n");

	ch->send(buf);

	sprintbit((long) SHOP_BITVECTOR(shop_nr), shop_bits, buf1);
	sprintf(buf, "Bits:       %s\r\n", buf1);
	ch->send(buf);
}


void show_shops(Character * ch, char *arg)
{
	int shop_nr;

	if (!*arg)
		list_all_shops(ch);

	else
	{

		if (!str_cmp(arg, "."))
		{

			for (shop_nr = 0; shop_nr < top_shop; ++shop_nr)
			if (ok_shop_room(shop_nr, ch->in_room->getVnum()))
					break;


			if (shop_nr == top_shop)
			{
				ch->send("This isn't a shop!\r\n");
				return;
			}
		}

		else if (IsNumber(arg))
			shop_nr = atoi(arg) - 1;
		else
			shop_nr = -1;

		if ((shop_nr < 0) || (shop_nr >= top_shop))
		{
			ch->send("Illegal shop number.\r\n");
			return;
		}

		list_detailed_shop(ch, shop_nr);
	}
}
