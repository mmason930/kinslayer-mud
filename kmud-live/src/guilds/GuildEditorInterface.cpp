#include <functional>
#include <string>
#include <vector>
#include <iostream>

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../Descriptor.h"
#include "../screen.h"
#include "../olc.h"
#include "../utils.h"
#include "../CharacterUtil.h"
#include "../db.h"

#include "GuildEditorInterface.h"
#include "GuildEditorInterfaceData.h"
#include "GuildUtil.h"
#include "GuildApplication.h"
#include "Guild.h"

EditorInterfaceData *GuildEditorInterface::createData() const
{
	return new GuildEditorInterfaceData();
}

GuildEditorInterface::GuildEditorInterface() : EditorInterface()
{
	mainMenu = addNewMenu();

	EditorInterfaceMenu *newGuildMenu = addNewMenu();
	EditorInterfaceMenu *searchGuildsMenu = addNewMenu();
	EditorInterfaceMenu *leaveGuildMenu = addNewMenu();
	EditorInterfaceMenu *reviewGuildApplicationListMenu = addNewMenu();
	EditorInterfaceMenu *reviewGuildApplicationMenu = addNewMenu();
	EditorInterfaceMenu *denyApplicationMenu = addNewMenu();
	EditorInterfaceMenu *newGuildNameMenu = addMenu(createValueInputMenu(
		"Enter the name of the new Guild: ",
		inputLengthValidator(1, 40, "The guild name must be between 1 and 40 characters."),
		[](EditorInterfaceInstance *i) { ((GuildEditorInterfaceData*)i->getData())->setNewGuildName(i->input); }
	));

	EditorInterfaceMenu *newGuildDescriptionMenu = addMenu(createValueInputMenu(
		"Enter the description of the new Guild: ",
		inputLengthValidator(1, 255, "The guild description must be between 1 and 255 characters."),
		[](EditorInterfaceInstance *i) { ((GuildEditorInterfaceData*)i->getData())->setNewGuildDescription(i->input); }
	));

	//Deny Application Menu
//	denyApplicationMenu

	//Review Application Menu
	reviewGuildApplicationMenu->setParseOperator(
		[=](EditorInterfaceInstance *i)
		{
			auto data = ((GuildEditorInterfaceData*)i->getData());
			switch(i->firstLetter)
			{
			case 'A':
			{
				GuildUtil::get()->approveGuildApplication(game->getConnection(), data->getReviewingGuildApplicationId(), i->ch->player.idnum);
				Guild *guild = GuildUtil::get()->createGuild(game->getConnection(), data->getReviewingGuildApplicationId());
				GuildUtil::get()->initNewlyCreatedGuild(game->getConnection(), guild->getId());
			
				i->send("The Guild `%s` has been approved.\r\n", guild->getName().c_str());

				return i->popAndDisplay();
			}
			case 'D':	return i->pushAndDisplay(denyApplicationMenu);
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}
		}
	);

	reviewGuildApplicationMenu->setPrintOperator(
		[](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = ((GuildEditorInterfaceData*)i->getData());
			auto application = GuildUtil::get()->getGuildApplication(data->getReviewingGuildApplicationId());

			if(application == nullptr)
			{
				MudLog(BRF, MAX(GET_INVIS_LEV(i->ch), LVL_APPR), TRUE, "%s attempting to review invalid guild application #%d.", GET_NAME(i->ch), data->getReviewingGuildApplicationId());
				return nullptr;
			}

			auto playerIndex = CharacterUtil::getPlayerIndexByUserId(application->getSubmittedByUserId());

			return i->send(
				" ~~ Guild Review ~~\r\n\r\n"
				"Guild Name: %s%s%s\r\n"
				"Guild Description: %s%s%s\r\n"
				"Submission Time: %s%s%s\r\n"
				"Submitted By: %s%s%s\r\n\r\n"
				"%sA%s) Approve Application\r\n"
				"%sD%s) Deny Application\r\n"
				"%sQ%s) Quit\r\n\r\n"
				"Enter Option: ",
				yel, application->getGuildName().c_str(), nrm,
				yel, application->getGuildDescription().c_str(), nrm,
				yel, MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getSubmittedDatetime()).c_str(), nrm,
				yel, (playerIndex ? playerIndex->name.c_str() : "<Deleted Player>"), nrm,
				yel, nrm,
				yel, nrm,
				yel, nrm
			);
		}
	);

	//Review Applications List Menu
	reviewGuildApplicationListMenu->setParseOperator(
//		integerValidator(),
		[=](EditorInterfaceInstance *i)
		{
			if(i->firstLetter == 'Q')
				return i->popAndDisplay();
			if(!MiscUtil::isInt(i->input))
				return i->send("You must enter a valid integer, or 'Q' to quit: \r\n");

			int guildApplicationId = MiscUtil::Convert<int>(i->input);

			GuildApplication *application = GuildUtil::get()->getGuildApplication(guildApplicationId);

			if(application == nullptr || !application->getStatus()->getRequiresReview())
				return i->send("The Guild you selected does not exist or is not in a status requiring review.\r\nEnter option: ");

			application->setReviewerUserId(i->ch->player.idnum);
			application->setStatus(GuildApplicationStatus::reviewing);
			
			GuildUtil::get()->putGuildApplication(game->getConnection(), application);
			((GuildEditorInterfaceData*)i->getData())->setReviewingGuildApplicationId(guildApplicationId);
			return i->pushAndDisplay(reviewGuildApplicationMenu);
		}
	);

	reviewGuildApplicationListMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i)
		{
			std::list<GuildApplication *> guildApplications = GuildUtil::get()->getGuildApplicationsRequiringReview();
			guildApplications.sort([](GuildApplication *a, GuildApplication *b) -> bool { return a->getId() < b->getId(); });

			for(GuildApplication *guildApplication : guildApplications)
			{
				i->send("%s%5d%s) %s%s%s\r\n", cyn, guildApplication->getId(), nrm, yel, guildApplication->getGuildName().c_str(), nrm);
			}

			i->send("\r\n%sQ%s) Quit", cyn, nrm);
			i->send("\r\nEnter option: ");
			return nullptr;
		}
	);

	reviewGuildApplicationListMenu->setPreReqOperator(
		[=](EditorInterfaceInstance *i)
		{
			return GET_LEVEL(i->ch) >= LVL_GOD ? boost::optional<std::string>() : std::string("You are not authorized to review Guild applications.");
		}
	);
	
	//Main Menu.
	mainMenu->setParseOperator(
		[=](EditorInterfaceInstance *i)
		{
			switch(i->firstLetter)
			{
			case 'N':	return i->pushAndDisplay(newGuildMenu);
			case 'S':	return i->pushAndDisplay(searchGuildsMenu);
			case 'R':	return i->pushAndDisplay(reviewGuildApplicationListMenu);
			case 'L':	return i->pushAndDisplay(leaveGuildMenu);
			case 'Q':	return i->terminate();
			default:	return i->invalidOption();
			}
		}
	);
	
	mainMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i)
		{
			i->ch->send(" ~~ Guild Manager ~~\r\n\r\n");

			i->ch->send(
				"%sN%s) New Guild Application\r\n"
				"%sS%s) Search Guilds\r\n"
				"%sR%s) Review Guild Applications\r\n"
				"%sL%s) Leave Guild\r\n\r\n"
				"%sQ%s) Exit\r\n",
				cyn, nrm,
				cyn, nrm,
				cyn, nrm,
				cyn, nrm,
				cyn, nrm
			);

			return nullptr;
		}
	);
	
	//New Guild Menu.
	newGuildMenu->setPreReqOperator(
		[=](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			if(GET_LEVEL(i->ch) < 30)
				return std::string("You must be at least level 30 to submit a guild application.");
			if(i->ch->Copper() < GuildUtil::get()->getCoppersToCreateNewGuild())
				return std::string("Creating a new Guild costs ") + MiscUtil::toString(GuildUtil::get()->getCoppersToCreateNewGuild()) + std::string("coppers.");
			if(!GuildUtil::get()->getActiveUserGuildsByUserId(i->ch->player.idnum).empty())
				return std::string("You are already in a guild!\r\n");
			if(GuildUtil::get()->findGuildApplication([=](GuildApplication *a) -> bool {
					return (a->getStatus() == GuildApplicationStatus::pending || a->getStatus() == GuildApplicationStatus::reviewing) && a->getSubmittedByUserId() == i->ch->getUserId();
			}) != nullptr)
				return std::string("You have another guild application that is currently pending.");

			//TODO: Check to see if they have an application to join a guild.

			return boost::optional<std::string>();
		}
	);

	newGuildMenu->setCleanupOperator(
		[](EditorInterfaceInstance *i) -> void
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			data->setNewGuildName("");
			data->setNewGuildDescription("");
		}
	);

	newGuildMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();

			i->ch->send(" ~~ Create New Guild ~~\r\n\r\n");

			i->ch->send(
				"%sN%s) Name of Guild: %s\r\n"
				"%sD%s) Description: %s\r\n"
				"%sS%s) Submit Application\r\n"
				"%sQ%s) Exit\r\n",
				cyn, nrm, data->getNewGuildName().c_str(),
				cyn, nrm, data->getNewGuildDescription().c_str(),
				cyn, nrm,
				cyn, nrm
			);

			return nullptr;
		}
	);

	newGuildMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			switch(i->firstLetter)
			{
			case 'N':	return i->pushAndDisplay(newGuildNameMenu);
			case 'D':	return i->pushAndDisplay(newGuildDescriptionMenu);
			case 'S':
				if(data->getNewGuildDescription().empty() || data->getNewGuildName().empty())
					return i->send("Both the guild name and guild description must first be set.\r\n");
				if(i->ch->Copper() < GuildUtil::get()->getCoppersToCreateNewGuild())
					return i->send("Creating a new Guild costs %d coppers.\r\n", GuildUtil::get()->getCoppersToCreateNewGuild());

				//Submit application & charge the player the required gold.
				GuildUtil::get()->submitGuildApplication(game->getConnection(), i->ch->getUserId(), data->getNewGuildName(), data->getNewGuildDescription(), GuildUtil::get()->getCoppersToCreateNewGuild());
				i->ch->points.gold -= GuildUtil::get()->getCoppersToCreateNewGuild();

				i->send("Congratulations! An application for the Guild `%s` has been submitted!\r\n", data->getNewGuildName().c_str());

				return i->popAndDisplay();
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}

			return nullptr;
		}
	);
}
