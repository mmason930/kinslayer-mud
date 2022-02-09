/*****************************************************
 *		Kinslayer's Warrant Edit System.			 *
 *		   By Galnor on 4-17-2005					 *
 *****************************************************/
#include "../conf.h"
#include "../comm.h"
#include "../interpreter.h"
#include "../utils.h"
#include "../db.h"
#include "olc.h"
#include "../clans.h"
#include "../Descriptor.h"

extern Descriptor *descriptor_list;

CommandHandler do_wedit = DEFINE_COMMAND
{
	Descriptor *d;
	Warrant *w;
	std::list<Warrant *>::iterator iter;
	char buf1[MAX_STRING_LENGTH];

	OneArgument(argument, buf1);

	for(d = descriptor_list;d;d = d->next)
	{
		if(STATE(d) == CON_WEDIT)
		{
			ch->send("%s is currently editing the warrants.\r\n", GET_NAME(d->character));
			return;
		}
	}

	d = ch->desc;

	d->olc = new OLC();
	OLC_ZONE(d) = 0;

	STATE(d) = CON_WEDIT;
	Act("$n begins using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
	SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_WRITING));
	MudLog(BRF, LVL_IMMORT, TRUE,  "OLC: %s starts editing the warrants.", GET_NAME(ch));

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		w = new Warrant();
		*w = *(*iter);
		d->olc->WarrantList.push_back(w);
	}
	WeditDispMenu(d);
};

void Warrant::Insert(std::list<Warrant *> &List)
{
	std::list<Warrant *>::iterator iter;

	for(iter = List.begin();iter != List.end();++iter)
	{
		if( (*iter)->vnum > this->vnum)
		{
			List.insert(iter, this);
			return;
		}
	}
	List.insert(iter, this);
}

void SaveWarrants()
{

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS `tempWarrant`");

	gameDatabase->sendRawQuery("CREATE TABLE `tempWarrant` LIKE `warrant`");

	sql::BatchInsertStatement batchInsertStatement(gameDatabase, "tempWarrant", 1000);
	batchInsertStatement.addField("id");
	batchInsertStatement.addField("warrant_name");
	
	batchInsertStatement.start();

	for(std::list<Warrant *>::iterator iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		batchInsertStatement.beginEntry();

		batchInsertStatement.putInt( (*iter)->vnum );
		batchInsertStatement.putString( (*iter)->Name );

		batchInsertStatement.endEntry();
	}

	batchInsertStatement.flush();

	batchInsertStatement.finish();

	try {
		gameDatabase->sendRawQuery("RENAME TABLE `warrant` TO `oldWarrant`, `tempWarrant` TO `warrant`, `oldWarrant` TO `tempWarrant`");
		gameDatabase->sendRawQuery("DROP TABLE `tempWarrant`");
	}
	catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to save warrants: %s", e.getMessage().c_str());
	}
}

void WeditDispMenu(Descriptor *d)
{
	std::list<Warrant *>::iterator iter;
	get_char_cols(d->character);

	d->send("--Warrant Edit Main Menu--\r\n");
	d->send("\r\nCurrent warrants:\r\n");

	for(iter = d->olc->WarrantList.begin();iter != d->olc->WarrantList.end();++iter)
	{
		d->send("%s%d. %s%s\r\n", grn, (*iter)->vnum, cyn, (*iter)->Name.c_str());
	}

	d->send(
	    "------------------ Options ------------------\r\n"
	    "%sN%s) New Warrant     %sD%s) Delete Warrant\r\n"
	    "%sE%s) Edit Warrant    %sQ%s) Exit Editor\r\n",
	    grn, nrm, grn, nrm,
	    grn, nrm, grn, nrm);

	OLC_MODE(d) = WEDIT_MENU;
}

void WeditParse(Descriptor *d, const std::string &arg)
{
	int num = atoi(arg.c_str());
	switch(OLC_MODE(d))
	{
		case WEDIT_MENU:
			switch(toupper(arg[0]))
			{
				case 'Q':
					d->send("Would you like to save your changes? 'Y' for yes, 'N' for no : ");
					OLC_MODE(d) = WEDIT_CONFIRM_SAVE;
					return;
				case 'N':
					d->send("Enter the number for this warrant : ");
					OLC_MODE(d) = WEDIT_NEW_NUMBER;
					return;
				case 'D':
					if(!(d->olc->WarrantList.size()))
					{
						d->send("There are no warrants to delete.\r\n");
						return;
					}
					d->send("Enter the number for the warrant you wish to delete : ");
					OLC_MODE(d) = WEDIT_DELETE_NUMBER;
					return;
				case 'E':
					d->send("Enter the number for the warrant you wish to edit : ");
					OLC_MODE(d) = WEDIT_EDIT_NUMBER;
					return;
				default:
					d->send("Invalid option. Try again : ");
					return;
			}
		case WEDIT_CONFIRM_SAVE:
			switch(toupper(arg[0]))
			{
				case 'Y':
					d->send("Saving warrants...\r\n");
					d->olc->WarrantList.swap(Warrants);
					SaveWarrants();
					cleanup_olc(d, CLEANUP_ALL);
					return;
				case 'N':
					d->send("Exited without saving.\r\n");
					cleanup_olc(d, CLEANUP_ALL);
					return;
				default:
					d->send("You must specify 'Y' or 'N' for saving.\r\n");
					return;
			}
		case WEDIT_NEW_NUMBER:
			if(WarrantByVnum(d->olc->WarrantList, num))
			{
				d->send("A warrant with that virtual number already exists. Try again : ");
				return;
			}
			if(num <= 0)
			{
				d->send("Must specify virtual number above zero. Try again : ");
				return;
			}
			OLC_WARRANT(d) = new Warrant;
			d->olc->warrant->vnum = num;
			d->olc->warrant->Insert(d->olc->WarrantList);

			d->send("Enter the name for this new warrant : ");
			OLC_MODE(d) = WEDIT_NEW_NAME;
			return;
		case WEDIT_NEW_NAME:
			d->olc->warrant->Name = arg;
			d->send("New warrant created.\r\n");
			break;
		case WEDIT_DELETE_NUMBER:
			if(!(d->olc->warrant = WarrantByVnum(d->olc->WarrantList, num)))
			{
				d->send("There is no warrant with that number. Try again : ");
				return;
			}

			d->olc->WarrantList.remove(WarrantByVnum(d->olc->WarrantList, num));
			delete d->olc->warrant;
			break;
		case WEDIT_EDIT_NUMBER:
			if(!(d->olc->warrant = WarrantByVnum(d->olc->WarrantList, num)))
			{
				d->send("There is no warrant with that number. Try again : ");
				return;
			}

			d->olc->WarrantList.remove(d->olc->warrant);
			delete d->olc->warrant;
			d->olc->warrant = new Warrant;
			d->olc->warrant->vnum = num;
			d->olc->warrant->Insert(d->olc->WarrantList);

			d->send("Enter the new name for this warrant : ");
			OLC_MODE(d) = WEDIT_NEW_NAME;
			return;
		default:
			d->send("Illegal wedit mode of %d. Please report this.\r\n");
			MudLog(BRF, MAX(GET_INVIS_LEV(d->character), LVL_APPR), TRUE, "%s has illegal wedit state of %d.", GET_NAME(d->character), OLC_MODE(d));
			cleanup_olc(d, CLEANUP_ALL);
			return;
	}

	WeditDispMenu(d);
}
