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
#include "../StringUtil.h"
#include "../TextTableBuilder.h"

#include "GuildEditorInterface.h"
#include "GuildEditorInterfaceData.h"
#include "GuildUtil.h"
#include "GuildApplication.h"
#include "Guild.h"
#include "GuildApplicationSignature.h"
#include "UserGuildStatus.h"
#include "GuildJoinApplication.h"
#include "GuildJoinApplicationStatus.h"

EditorInterfaceData *GuildEditorInterface::createData() const
{
	return new GuildEditorInterfaceData();
}

GuildEditorInterface::GuildEditorInterface() : EditorInterface()
{
	mainMenu = addNewMenu();

	auto searchGuildsMenu = addNewMenu();
	auto leaveGuildMenu = addNewMenu();
	auto denyApplicationMenu = addNewMenu();
	auto guildCreationApplicationsMenu = addNewMenu();
	auto guildJoinApplicationsMenu = addNewMenu();
	auto guildsMenu = addNewMenu();
	auto guildApplicationViewMenu = addNewMenu();
	auto guildApplicationCreateMenu = addNewMenu();
	auto guildApplicationApproveMenu = addNewMenu();
	auto guildApplicationApproveSignatureMenu = addNewMenu();
	auto guildApplicationDenySignatureMenu = addNewMenu();
	auto guildApplicationApproveDenyMenu = addNewMenu();
	auto guildJoinApplicationViewGuildMenu = addNewMenu();
	auto guildJoinApplicationCreateApplicationMenu = addNewMenu();
	auto guildJoinViewJoinApplicationMenu = addNewMenu();

	guildJoinViewJoinApplicationMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildJoinApplication(data->getGuildJoinApplicationId());
			auto guild = GuildUtil::get()->getGuild(application->getGuildId());
			auto playerIndex = CharacterUtil::getPlayerIndexByUserId(application->getStatusLastModifiedByUserId());

			i->send("ID                : %s%d%s\r\n"
					"Guild             : %s%s%s\r\n"
					"Submitted         : %s%s%s\r\n"
					"Status            : %s%s%s\r\n"
					"Last Modified Date: %s%s%s\r\n"
					"Last Modified By  : %s%s%s\r\n"
					"Message To Guild  : %s%s%s\r\n"
					"Message From Guild: %s%s%s\r\n",
					yel, application->getId(), nrm,
					yel, guild->getName().c_str(), nrm,
					yel, MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getCreatedDatetime()).c_str(), nrm,
					yel, application->getStatus()->getStandardName().c_str(), nrm,
					yel, MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getStatusLastModifiedDatetime()).c_str(), nrm,
					yel, (playerIndex == nullptr ? "<Deleted>" : playerIndex->name.c_str()), nrm,
					yel, application->getMessageToGuild().c_str(), nrm,
					yel, application->getMessageFromGuild() ? application->getMessageFromGuild().get().c_str() : "", nrm
			);

			i->send("\r\n");

			if(application->getStatus()->submitterCanRemove())
				i->send("%sR%s) Remove\r\n", cyn, nrm);

			return i->send("%sQ%s) Quit\r\n", cyn, nrm);
		}
	);

	guildJoinViewJoinApplicationMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildJoinApplication(data->getGuildJoinApplicationId());
			auto guild = GuildUtil::get()->getGuild(application->getGuildId());

			switch(i->firstLetter)
			{
			case 'R':
			{
				if(!application->getStatus()->submitterCanRemove())
					return i->invalidOption();

				GuildUtil::get()->removeGuildJoinApplication(game->getConnection(), i->getUserId(), application->getId());

				i->send("%s%sYou have removed your application to the Guild `%s`.\r\n\r\n", bld, grn, guild->getName().c_str(), nrm);

				return i->print();
			}
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}
		}
	);

	guildJoinApplicationCreateApplicationMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			return i->send("Enter a message that you would like to be sent to the Guild you are applying for: ");
		}
	);

	guildJoinApplicationCreateApplicationMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->submitGuildJoinApplication(game->getConnection(), i->ch->getUserId(), data->getGuildId(), i->input);
			auto guild = GuildUtil::get()->getGuild(application->getGuildId());

			i->send("%s%sYou have successfully applied for the Guild `%s`!%s\r\n\r\n", bld, grn, guild->getName().c_str(), nrm);

			return i->popAndDisplay();
		}
	);

	guildJoinApplicationViewGuildMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto guild = GuildUtil::get()->getGuild(data->getGuildId());
			auto playerIndex = CharacterUtil::getPlayerIndexByUserId(guild->getCreatedByUserId());
			int numberOfMembers = GuildUtil::get()->getNumberOfActiveGuildMembers(guild->getId());

			i->send("ID        : %s%d%s\r\n"
					"Name      : %s%s%s\r\n"
					"Created   : %s%s%s\r\n"
					"Created By: %s%s%s\r\n"
					"Members   : %s%d%s\r\n\r\n"

					"%sA%s) Apply to Join\r\n",
					yel, guild->getId(), nrm,
					yel, guild->getName().c_str(), nrm,
					yel, MiscUtil::formatDateYYYYdmmdddHHcMMcSS(guild->getCreatedDatetime()).c_str(), nrm,
					yel, (playerIndex ? playerIndex->name.c_str() : "<Deleted User>"), nrm,
					yel, numberOfMembers, nrm,
					cyn, nrm
			);

			if(!GuildUtil::get()->getGuildJoinApplications(i->getUserId(), data->getGuildId(), GuildJoinApplicationStatus::getStatusesSubmitterCanRemove()).empty())
				i->send("%sR%s) Remove Application\r\n", cyn, nrm);

			i->send("%sQ%s) Quit\r\n\r\n"

					"Choose an Option: ",
					cyn, nrm
			);
		}
	);

	guildJoinApplicationViewGuildMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto guild = GuildUtil::get()->getGuild(data->getGuildId());

			switch(i->firstLetter)
			{
			case 'A':
			{
				if(guild->getStatus() != GuildStatus::active)
					return i->send("You cannot join this Guild because its status is not active.\r\n");
				if(guild->getRace() != guild->getRace())
					return i->send("You cannot join this Guild because it is for a race other than your own.\r\n");
				if(!GuildUtil::get()->getUserGuildsByUserId(i->ch->getUserId(), {UserGuildStatus::active}).empty())
					return i->send("You are already in another Guild.\r\n");
				if(!GuildUtil::get()->getGuildJoinApplications(i->getUserId(), boost::optional<int>(), {GuildJoinApplicationStatus::reviewing}).empty())
					return i->send("You have another pending application to join a Guild.\r\n");
				if(!GuildUtil::get()->getGuildApplications(i->getUserId(), {GuildApplicationStatus::pending, GuildApplicationStatus::reviewing}).empty())
					return i->send("You have another Guild Creation Application that is pending/reviewing.\r\n");
				if(GET_LEVEL(i->ch) < GuildUtil::get()->getMinimumLevelToJoinGuild())
					return i->send("You must be at least level %d to join a Guild.\r\n");
				for(auto signature : GuildUtil::get()->getGuildApplicationSignaturesByUserId(i->getUserId(), {GuildApplicationSignatureStatus::pending, GuildApplicationSignatureStatus::approved}))
				{
					auto guildApplication = GuildUtil::get()->getGuildApplication(signature->getGuildApplicationId());

					if(!guildApplication->getStatus()->getApplicationClosed())
						return i->send("You have a signature on a Guild Creation Application that is still open.\r\n");
				}

				return i->pushAndDisplay(guildJoinApplicationCreateApplicationMenu);
			}
			case 'R':
			{
				auto pendingApplications = GuildUtil::get()->getGuildJoinApplications(i->getUserId(), data->getGuildId(), GuildJoinApplicationStatus::getStatusesSubmitterCanRemove());

				if(pendingApplications.empty())
					return i->invalidOption();

				for(auto application : pendingApplications)
				{
					GuildUtil::get()->removeGuildJoinApplication(game->getConnection(), i->getUserId(), application->getId());
				}

				i->send("%s%sYou have removed all pending Guild Join Applications.%s\r\n\r\n", bld, red, nrm);
				return i->print();
			}
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}
		}
	);

	guildJoinApplicationsMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{

			i->send(" ~~ Join a Guild ~~\r\n\r\n");

			i->send("The following Guilds are open:\r\n\r\n");
		
			TextTableBuilder builder;
			builder.startRow()
				->put("ID")
				->put("Name")
				->put("Creator")
				->put("Members")
				->put("Created Time");

			for(auto guild : GuildUtil::get()->getGuilds({GuildStatus::active}))
			{
				if(guild->getRace() != GET_RACE(i->ch) && GET_LEVEL(i->ch) < LVL_IMMORT)
					continue;

				auto playerIndex = CharacterUtil::getPlayerIndexByUserId(guild->getCreatedByUserId());

				builder.startRow()
					->put(cyn)->append("G")->append(guild->getId())->append(nrm)
					->put(guild->getName())
					->put(playerIndex->name)
					->put(GuildUtil::get()->getNumberOfActiveGuildMembers(guild->getId()))
					->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(guild->getCreatedDatetime()));
			}

			i->send("%s\r\n\r\n", builder.build().c_str());

			auto applications = GuildUtil::get()->getGuildJoinApplications(i->getUserId(), boost::optional<int>());

			if(applications.empty())
				i->send("You have not submitted any applications to join a Guild.\r\n\r\n");
			else
			{
				TextTableBuilder applicationBuilder;

				applicationBuilder.startRow()
					->put("ID")
					->put("Guild")
					->put("Submitted")
					->put("Status")
					->put("Last Modified By")
					->put("Last Modified Date");

				for(auto application : applications) {
				
					auto playerIndex = CharacterUtil::getPlayerIndexByUserId(application->getStatusLastModifiedByUserId());
					auto guild = GuildUtil::get()->getGuild(application->getGuildId());

					applicationBuilder.startRow()
						->put(cyn)->append("A")->append(application->getId())->append(nrm)
						->put(guild->getName())
						->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getCreatedDatetime()))
						->put(application->getStatus()->getStandardName())
						->put( playerIndex == nullptr ? "<Deleted>" : playerIndex->name.c_str() )
						->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getStatusLastModifiedDatetime()));
				}

				i->send("%s\r\n\r\n", applicationBuilder.build().c_str());
			}

			i->send("%sQ%s) Quit\r\n", cyn, nrm);
		}
	);

	guildJoinApplicationsMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			switch(i->firstLetter)
			{
			case 'Q':		return i->popAndDisplay();
			case 'G':
			{
				std::string guildIdString = i->input.substr(1);

				if(!MiscUtil::isInt(guildIdString))
					return i->invalidOption();

				int guildId = MiscUtil::convert<int>(guildIdString);
				auto guild = GuildUtil::get()->getGuild(guildId);

				if(guild == nullptr || guild->getStatus() != GuildStatus::active || (guild->getRace() != GET_RACE(i->ch) && GET_LEVEL(i->ch) < LVL_IMMORT))
					return i->invalidOption();

				auto data = (GuildEditorInterfaceData*)i->getData();
				data->setGuildId(guildId);

				return i->pushAndDisplay(guildJoinApplicationViewGuildMenu);
			}
			case 'A':
			{
				std::string guildJoinApplicationIdString = i->input.substr(1);

				if(!MiscUtil::isInt(guildJoinApplicationIdString))
					return i->invalidOption();

				int guildJoinApplicationId = MiscUtil::convert<int>(guildJoinApplicationIdString);
				auto application = GuildUtil::get()->getGuildJoinApplication(guildJoinApplicationId);

				if(application == nullptr || application->getUserId() != i->getUserId())
					return i->invalidOption();

				auto data = (GuildEditorInterfaceData*)i->getData();
				data->setGuildJoinApplicationId(application->getId());

				return i->pushAndDisplay(guildJoinViewJoinApplicationMenu);
			}
			default:		return i->invalidOption();
			}
		}
	);



	auto denyApplicationReasonMenu = addMenu(createValueInputMenu(
		"Enter your reason for denying this application: ",
		inputLengthValidator(1, 255, "The reason you enter must be between 1 and 255 characters."),
		[](EditorInterfaceInstance *i) { ((GuildEditorInterfaceData*)i->getData())->setApplicationDenialReason(i->input); }
	));

	auto newGuildNameMenu = addMenu(createValueInputMenu(
		"Enter the name of the new Guild: ",
		inputLengthValidator(1, 40, "The guild name must be between 1 and 40 characters."),
		[](EditorInterfaceInstance *i) { ((GuildEditorInterfaceData*)i->getData())->setNewGuildName(i->input); }
	));

	auto newGuildDescriptionMenu = addMenu(createValueInputMenu(
		"Enter the description of the new Guild: ",
		inputLengthValidator(1, 255, "The guild description must be between 1 and 255 characters."),
		[](EditorInterfaceInstance *i) { ((GuildEditorInterfaceData*)i->getData())->setNewGuildDescription(i->input); }
	));
	
	denyApplicationMenu->setPrintOperator(
		[](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());

			i->send(" ~~ Deny Guild Application: %s ~~\r\n\r\n", application->getGuildName().c_str());

			i->send("%sR%s) Denial Reason: %s%s%s\r\n", cyn, nrm, yel, data->getApplicationDenialReason().c_str(), nrm);
			i->send("%sS%s) Submit Denial\r\n", cyn, nrm);
			i->send("%sQ%s) Quit (Application Will Not Be Denied)\r\n", cyn, nrm);

			i->send("\r\nEnter Choice: ");
		}
	);

	denyApplicationMenu->setPreReqOperator(
		[](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());
			int signatures = GuildUtil::get()->getNumberOfValidGuildApplicationSignatures(application->getId());
			int signaturesRequired = GuildUtil::get()->getNumberOfRequiredSignaturesForNewGuild();
			int levelRequired = LVL_GOD;

			if(GET_LEVEL(i->ch) < levelRequired)
				return std::string("You must be level ") + MiscUtil::toString(levelRequired) + " or greater to perform this operation.";
			if(!application->getStatus()->getRequiresReview())
				return std::string("This application is not in a status that can be approved. Its status is `") + application->getStatus()->getStandardName() + "`";
			if(signatures < signaturesRequired)
				return std::string("This application only has ") + MiscUtil::toString(signatures) + " signature" + StringUtil::plural(signatures) + " out of " + MiscUtil::toString(signaturesRequired) + ".";
			return boost::optional<std::string>();
		}
	);

	denyApplicationMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			switch(i->firstLetter)
			{
			case 'R':	return i->pushAndDisplay(denyApplicationReasonMenu);
			case 'S':
			{
				if(data->getApplicationDenialReason().empty())
					return i->send("You must first enter a denial reason.\r\n");

				GuildUtil::get()->denyGuildApplication(game->getConnection(), data->getGuildApplicationId(), i->getUserId(), data->getApplicationDenialReason());
				
				//TODO: Send some kind of message to player informing them of denial.

				i->send("%s%sThe application has been denied.%s\r\n\r\n", bld, red, nrm);

				return i->popAndDisplay();
			}
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}
		}
	);

	denyApplicationMenu->setCleanupOperator(
		[](EditorInterfaceInstance *i) -> void
		{
			((GuildEditorInterfaceData*)i->getData())->setApplicationDenialReason("");
		}
	);

	//Deny Signature
	guildApplicationDenySignatureMenu->setPrintOperator(
		[](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			return i->send("Enter the ID of the signature you wish to deny, or 'Q' to quit: ");
		}
	);

	guildApplicationDenySignatureMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			if(i->firstLetter == 'Q') return i->popAndDisplay();

			auto data = (GuildEditorInterfaceData*)i->getData();
			int guildApplicationId = data->getGuildApplicationId();
			if(!MiscUtil::isInt(i->input))
				return i->invalidOption();

			int guildApplicationSignatureId = MiscUtil::convert<int>(i->input);
			auto signature = GuildUtil::get()->getGuildApplicationSignature(guildApplicationSignatureId);

			if(signature == nullptr || signature->getGuildApplicationId() != guildApplicationId)
				return i->invalidOption();

			if(!signature->getStatus()->getCanBeDenied())
				return i->send("You cannot deny this signature because its status is `%s`.", signature->getStatus()->getStandardName().c_str());

			GuildUtil::get()->denyGuildApplicationSignature(game->getConnection(), signature->getId(), i->getUserId());
			i->send("You have denied signature #%d.", signature->getId());
			return i->popAndDisplay();
	});

	guildApplicationDenySignatureMenu->setPreReqOperator(
		[=](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());

			if(i->getUserId() != application->getSubmittedByUserId())
				return std::string("You are not authorized to perform that operation.\r\n");
			if(application->getStatus() != GuildApplicationStatus::pending)
				return std::string("You cannot do this because your Guild is in the `") + application->getStatus()->getStandardName() + "` status.\r\n"; 
		}
	);

	//Approve Signature
	guildApplicationApproveSignatureMenu->setPrintOperator(
		[](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			return i->send("Enter the ID of the signature you wish to approve, or 'Q' to quit: ");
		}
	);

	guildApplicationApproveSignatureMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			if(i->firstLetter == 'Q') return i->popAndDisplay();

			auto data = (GuildEditorInterfaceData*)i->getData();
			int guildApplicationId = data->getGuildApplicationId();
			if(!MiscUtil::isInt(i->input))
				return i->invalidOption();

			int guildApplicationSignatureId = MiscUtil::convert<int>(i->input);
			auto signature = GuildUtil::get()->getGuildApplicationSignature(guildApplicationSignatureId);

			if(signature == nullptr || signature->getGuildApplicationId() != guildApplicationId)
				return i->invalidOption();

			if(!signature->getStatus()->getCanBeApproved())
				return i->send("You cannot approve this signature because its status is `%s`.", signature->getStatus()->getStandardName().c_str());

			GuildUtil::get()->approveSignature(game->getConnection(), guildApplicationSignatureId, i->getUserId());
			
			i->send("You have approved signature #%d!\r\n\r\n", signature->getId());

			if(GuildUtil::get()->getNumberOfValidGuildApplicationSignatures(guildApplicationId) >= GuildUtil::get()->getNumberOfRequiredSignaturesForNewGuild())
			{
				auto application = GuildUtil::get()->getGuildApplication(guildApplicationId);

				application->setStatus(GuildApplicationStatus::reviewing);
				//TODO: Set Reviewer User ID.

				GuildUtil::get()->putGuildApplication(game->getConnection(), application);

				i->send("%s%sYour application has reached the required number of signatures! It will now be reviewed by a staff member.%s\r\n\r\n",
					COLOR_BOLD(i->ch, CL_NORMAL), COLOR_GREEN(i->ch, CL_NORMAL), COLOR_NORMAL(i->ch, CL_NORMAL));
			}
			
			return i->popAndDisplay();
		}
	);

	guildApplicationApproveSignatureMenu->setPreReqOperator(
		[=](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());

			if(i->getUserId() != application->getSubmittedByUserId())
				return std::string("You are not authorized to perform that operation.\r\n");
			if(application->getStatus() != GuildApplicationStatus::pending)
				return std::string("You cannot do this because your Guild is in the `") + application->getStatus()->getStandardName() + "` status.\r\n";
			return boost::optional<std::string>();
		}
	);

	//Guild Creation Application Approval
	guildApplicationApproveMenu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			switch(i->firstLetter)
			{
				case 'Y':
				{
					GuildUtil::get()->approveGuildApplication(game->getConnection(), data->getGuildApplicationId(), i->getUserId());
					Guild *guild = GuildUtil::get()->createGuild(game->getConnection(), data->getGuildApplicationId());
					GuildUtil::get()->initNewlyCreatedGuild(game->getConnection(), data->getGuildApplicationId(), guild->getId());
			
					i->send("The Guild `%s` has been approved.\r\n", guild->getName().c_str());

					return i->popAndDisplay();
				}
				case 'N': return i->popAndDisplay();
			}

			return i->invalidOption();
		}
	);

	guildApplicationApproveMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());
			return i->send("Are you sure that you wish to approve the Guild `%s`?", application->getGuildName().c_str());
		}
	);

	guildApplicationApproveMenu ->setPreReqOperator(
		[](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());
			int signatures = GuildUtil::get()->getNumberOfValidGuildApplicationSignatures(application->getId());
			int signaturesRequired = GuildUtil::get()->getNumberOfRequiredSignaturesForNewGuild();
			int levelRequired = LVL_GOD;

			if(GET_LEVEL(i->ch) < levelRequired)
				return std::string("You must be level ") + MiscUtil::toString(levelRequired) + " or greater to perform this operation.";
			if(!application->getStatus()->getRequiresReview())
				return std::string("This application is not in a status that can be approved. Its status is `") + application->getStatus()->getStandardName() + "`";
			if(signatures < signaturesRequired)
				return std::string("This application only has ") + MiscUtil::toString(signatures) + " signature" + StringUtil::plural(signatures) + " out of " + MiscUtil::toString(signaturesRequired) + ".";
			return boost::optional<std::string>();
		}
	);

	guildApplicationViewMenu->setParseOperator(
		[=](EditorInterfaceInstance *i)
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());

			switch(i->firstLetter)
			{
			case 'A':	return i->pushAndDisplay(guildApplicationApproveMenu);
			case 'D':	return i->pushAndDisplay(denyApplicationMenu);
			case 'S':
			{
				if(i->getUserId() == application->getSubmittedByUserId())
					return i->send("You cannot sign an application that you submitted.\r\n\r\n");
				if(!GuildUtil::get()->getUserGuildsByUserId(i->getUserId(), {UserGuildStatus::active}).empty())
					return i->send("You cannot sign a Guild Application because you are already in another Guild.\r\n\r\n");
				if(application->getStatus() != GuildApplicationStatus::pending)
					return i->send("You can only sign an application in the `Pending` status. This application has a status of `%s`.\r\n\r\n", application->getStatus()->getStandardName().c_str());
				if(GET_RACE(i->ch) != application->getSubmittedByUserRace())
					return i->send("You are not of the same race as the creator of this application.\r\n\r\n");
				std::vector<GuildApplicationSignature*> userSignatures = GuildUtil::get()->getGuildApplicationSignaturesByUserId(i->getUserId());
				
				if(std::find_if(userSignatures.begin(), userSignatures.end(), [=](GuildApplicationSignature *signature) -> bool
				{
					return signature->getGuildApplicationId() == application->getId() && signature->getStatus() != GuildApplicationSignatureStatus::removed;
				}) != userSignatures.end())
				{
					return i->send("You have already submitted a signature for this Guild.\r\n\r\n");
				}

				GuildUtil::get()->submitGuildApplicationSignature(game->getConnection(), i->getUserId(), data->getGuildApplicationId());
				i->send("You have signed your support for the creation of the Guild `%s`.\r\n\r\n", application->getGuildName().c_str());

				return i->print();
			}
			case 'Y':	return i->pushAndDisplay(guildApplicationDenySignatureMenu);
			case 'P':	return i->pushAndDisplay(guildApplicationApproveSignatureMenu);
			case 'R':
			{
				bool hadSignature = false;

				if(application->getStatus() != GuildApplicationStatus::pending)
					return i->send("You cannot remove your signature for this application because its status is `%s`.\r\n\r\n", application->getStatus()->getStandardName().c_str());

				for(auto signature : GuildUtil::get()->getGuildApplicationSignaturesByGuildApplicationId(application->getId()))
				{
					if(signature->getUserId() == i->getUserId() && signature->getStatus()->getCanBeRemovedBySubmitter())
					{
						GuildUtil::get()->removeGuildSignatureByUser(game->getConnection(), signature->getId());
						hadSignature = true;
					}
				}

				if(hadSignature)
					i->send("Your signature for the Guild Application `%s` has been removed.\r\n\r\n", application->getGuildName().c_str());
				else
					i->send("You did not have any signatures that could be removed.\r\n\r\n");

				return i->print();
			}
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}
		}
	);

	guildApplicationViewMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
		
			auto data = (GuildEditorInterfaceData*)i->getData();
			auto application = GuildUtil::get()->getGuildApplication(data->getGuildApplicationId());

			if(application == nullptr)
			{
				MudLog(BRF, MAX(GET_INVIS_LEV(i->ch), LVL_APPR), TRUE, "Could not find guild application in View Guild Application menu. Player: %s, ID: %d", GET_NAME(i->ch), data->getGuildApplicationId());
				return nullptr;
			}
			
			auto playerIndex = CharacterUtil::getPlayerIndexByUserId(application->getSubmittedByUserId());
			auto reviewerPlayerIndex = application->getReviewerUserId() ? CharacterUtil::getPlayerIndexByUserId(application->getReviewerUserId().get()) : nullptr;

			i->send(" ~~ View Guild Application ~~\r\n\r\n"

					"ID           : %s%d%s\r\n"
					"Guild ID     : %s%s%s\r\n"
					"Name         : %s%s%s\r\n"
					"Description  : %s%s%s\r\n"
					"Submitted    : %s%s%s\r\n"
					"Creator      : %s%s%s\r\n"
					"Status       : %s%s%s\r\n"
					"Cost         : %s%s%s\r\n"
					"Signatures   : %s%d%s\r\n"
					"Completed    : %s%s%s\r\n"
					"Reviewed By  : %s%s%s\r\n"
					"Denied Reason: %s%s%s\r\n",
					yel, application->getId(), nrm,
					yel, (!application->getGuildId() ? "<Not Created>" : MiscUtil::toString(application->getGuildId().get())).c_str(), nrm,
					yel, application->getGuildName().c_str(), nrm,
					yel, application->getGuildDescription().c_str(), nrm,
					yel, MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getSubmittedDatetime()).c_str(), nrm,
					yel, (playerIndex == nullptr ? "<Deleted>" : playerIndex->name.c_str()), nrm,
					yel, application->getStatus()->getStandardName().c_str(), nrm,
					yel, MiscUtil::toString(application->getCoppersCharged()).c_str(), nrm,
					yel, GuildUtil::get()->getNumberOfValidGuildApplicationSignatures(application->getId()), nrm,
					yel, (application->getCompletedDatetime() ? MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getCompletedDatetime().get()).c_str() : ""), nrm,
					yel, (application->getReviewerUserId() ? (reviewerPlayerIndex ? reviewerPlayerIndex->name.c_str() : "<Deleted>") : ""), nrm,
					yel, application->getDeniedReason() ? application->getDeniedReason().get_ptr()->c_str() : "", nrm
				);

			i->send("\r\n");

			std::vector<GuildApplicationSignature*> signatures = GuildUtil::get()->getGuildApplicationSignaturesByGuildApplicationId(application->getId());
			
			if(!signatures.empty())
			{
				i->send("The following signatures have been placed in support of this application:\r\n\r\n");

				auto builder = TextTableBuilder(false);

				builder.startRow()
					->put("ID")
					->put("Signer Name")
					->put("Status")
					->put("Created")
					->put("Status Last Modified")
					->put("Last Modified By");

				for(auto signature : signatures)
				{
					auto playerIndex = CharacterUtil::getPlayerIndexByUserId(signature->getUserId());
					auto lastModPlayerIndex = CharacterUtil::getPlayerIndexByUserId(signature->getStatusChangedByUserId());

					builder.startRow()
						->put(signature->getId())
						->put(playerIndex == nullptr ? "<Deleted>" : playerIndex->name.c_str())
						->put(signature->getStatus()->getStandardName())
						->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(signature->getCreatedDatetime()))
						->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(signature->getStatusLastModifiedDatetime()))
						->put((lastModPlayerIndex == nullptr ? "<Deleted>" : lastModPlayerIndex->name)
					);
				}

				i->send("%s\r\n\r\n", builder.build().c_str());
			}

			if(denyApplicationMenu->preReqNoPrint(i))
				i->send("%sD%s) Deny Application\r\n", cyn, nrm);
			if(guildApplicationApproveMenu->preReqNoPrint(i))
				i->send("%sA%s) Approve Application\r\n", cyn, nrm);
			if(guildApplicationApproveSignatureMenu->preReqNoPrint(i))
				i->send("%sP%s) Approve Signature\r\n", cyn, nrm);
			if(guildApplicationDenySignatureMenu->preReqNoPrint(i))
				i->send("%sY%s) Deny Signature\r\n", cyn, nrm);
			
			i->send(
					"%sS%s) Sign Application\r\n"
					"%sR%s) Remove Signature\r\n"
					"%sQ%s) Quit\r\n\r\n"

					"Enter Option: ",
					cyn, nrm,
					cyn, nrm,
					cyn, nrm
			);

			return nullptr;
		}
	);
	
	guildCreationApplicationsMenu->setParseOperator(
		[=](EditorInterfaceInstance *i)
		{
			auto data = (GuildEditorInterfaceData*)i->getData();

			switch(i->firstLetter)
			{
				case 'C': return i->pushAndDisplay(guildApplicationCreateMenu);
				case 'Q': return i->popAndDisplay();
			}
			
			if(!MiscUtil::isInt(i->input))
				return i->invalidOption();

			int guildApplicationId = MiscUtil::convert<int>(i->input);
			auto application = GuildUtil::get()->getGuildApplication(guildApplicationId);
			
			if(application == nullptr || (application->getStatus() != GuildApplicationStatus::pending && application->getStatus() != GuildApplicationStatus::reviewing))
				return i->invalidOption();

			data->setGuildApplicationId(guildApplicationId);
			return i->pushAndDisplay(guildApplicationViewMenu);
		}
	);
	guildCreationApplicationsMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			i->send(" ~~ Guild Create Applications ~~\r\n\r\n");

			TextTableBuilder builder;

			builder.startRow()
				->put("ID")
				->put("Guild Name")
				->put("Time")
				->put("Creator")
				->put("Signatures")
				->put("Status");

			for(auto application : GuildUtil::get()->getGuildApplications(boost::optional<int>(), {GuildApplicationStatus::pending, GuildApplicationStatus::reviewing}))
			{
				auto playerIndex = CharacterUtil::getPlayerIndexByUserId(application->getSubmittedByUserId());
				int signatures = GuildUtil::get()->getNumberOfValidGuildApplicationSignatures(application->getId());

				builder.startRow()
					->put(cyn)->append(application->getId())->append(nrm)
					->put(application->getGuildName())
					->put(MiscUtil::formatDateYYYYdmmdddHHcMMcSS(application->getSubmittedDatetime()))
					->put(playerIndex == nullptr ? "<Deleted User>" : playerIndex->name.c_str())
					->put(signatures)->append("/")->append(GuildUtil::get()->getNumberOfRequiredSignaturesForNewGuild())
					->put(application->getStatus()->getStandardName().c_str());
			}

			i->send("%s\r\n\r\n", builder.build().c_str());

			i->send("%sC%s) Create Guild Application\r\n"
					"%sQ%s) Quit\r\n\r\n"
					"Enter an option or application ID from above:\r\n",
					cyn, nrm,
					cyn, nrm);
				
			return nullptr;
		}
	);

	//Main Menu.
	mainMenu->setParseOperator(
		[=](EditorInterfaceInstance *i)
		{
			switch(i->firstLetter)
			{
			case 'C':	return i->pushAndDisplay(guildCreationApplicationsMenu);
			case 'J':	return i->pushAndDisplay(guildJoinApplicationsMenu);
			case 'G':	return i->pushAndDisplay(guildsMenu);
			case 'Q':	return i->terminate();
			default:	return i->invalidOption();
			}
		}
	);

	mainMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			i->ch->send(" ~~ Guild Manager ~~\r\n\r\n");

			i->ch->send(
				"%sC%s) Guild Creation Applications\r\n"
				"%sJ%s) Guild Join Applications\r\n"
				"%sG%s) Guilds\r\n\r\n"
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
	guildApplicationCreateMenu->setPreReqOperator(
		[=](EditorInterfaceInstance *i) -> boost::optional<std::string>
		{
			if(GET_LEVEL(i->ch) < GuildUtil::get()->getMinimumLevelToSubmitGuildApplication())
				return std::string("You must be at least level ") + MiscUtil::toString(GuildUtil::get()->getMinimumLevelToSubmitGuildApplication()) + " to submit a guild application.";
			if(i->ch->points.gold < GuildUtil::get()->getCoppersToCreateNewGuild())
				return std::string("Creating a new Guild costs ") + MiscUtil::toString(GuildUtil::get()->getCoppersToCreateNewGuild()) + std::string(" coppers.");
			if(!GuildUtil::get()->getActiveUserGuildsByUserId(i->getUserId()).empty())
				return std::string("You are already in a guild!");
			if(GuildUtil::get()->findGuildApplication([=](GuildApplication *a) -> bool {
				return (a->getStatus() == GuildApplicationStatus::pending || a->getStatus() == GuildApplicationStatus::reviewing) && a->getSubmittedByUserId() == i->ch->getUserId();
			}) != nullptr)
				return std::string("You have another guild application that is currently pending.");

			//Check for signature for an open Guild Application.
			for(auto signature : GuildUtil::get()->getGuildApplicationSignaturesByUserId(i->getUserId()))
			{
				auto application = GuildUtil::get()->getGuildApplication(signature->getGuildApplicationId());

				if(application != nullptr && !application->getStatus()->getApplicationClosed())
					return std::string("You have an application signature open for the Guild `") + application->getGuildName() +"`, which has a status of `" + application->getStatus()->getStandardName() + "`.";
			}

			//TODO: Check to see if they have an application to join a guild.

			return boost::optional<std::string>();
		}
	);

	guildApplicationCreateMenu->setCleanupOperator(
		[](EditorInterfaceInstance *i) -> void
		{
			auto data = (GuildEditorInterfaceData*)i->getData();
			data->setNewGuildName("");
			data->setNewGuildDescription("");
		}
	);

	guildApplicationCreateMenu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
		{
			auto data = (GuildEditorInterfaceData*)i->getData();

			i->send(
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

	guildApplicationCreateMenu->setParseOperator(
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
				if(i->ch->points.gold < GuildUtil::get()->getCoppersToCreateNewGuild())
					return i->send("Creating a new Guild costs %d coppers.\r\n", GuildUtil::get()->getCoppersToCreateNewGuild());
				
				//TODO: Check to see if there is another guild with the same name.

				//Submit application & charge the player the required gold.
				GuildUtil::get()->submitGuildApplication(game->getConnection(), i->ch->getUserId(), GET_RACE(i->ch), data->getNewGuildName(), data->getNewGuildDescription(), GuildUtil::get()->getCoppersToCreateNewGuild());
				i->ch->points.gold -= GuildUtil::get()->getCoppersToCreateNewGuild();

				i->send("Congratulations! An application for the Guild `%s` has been submitted!\r\n\r\n", data->getNewGuildName().c_str());

				return i->popAndDisplay();
			case 'Q':	return i->popAndDisplay();
			default:	return i->invalidOption();
			}

			return nullptr;
		}
	);
}
