#include "../../conf.h"
#include "../../sysdep.h"
#include "../../structs.h"
#include "../../db.h"
#include "../../handler.h"
#include "../../interpreter.h"
#include "../../comm.h"
#include "../../fight.h"
#include "../../screen.h"
#include "../../olc/olc.h"
#include "../../utils.h"
#include "../../StringUtil.h"
#include "../../MiscUtil.h"
#include "../../Descriptor.h"
#include "../../zones.h"
#include "../../rooms/Room.h"
#include "../../rooms/Exit.h"
#include "../../commands/infrastructure/CommandUtil.h"
#include "../../commands/infrastructure/CommandInfo.h"
#include "../../commands/infrastructure/Social.h"
#include "../../mobs.h"
#include "../../CharacterUtil.h"
#include "../../guilds/GuildUtil.h"
#include "../../guilds/Guild.h"
#include "../../guilds/GuildApplication.h"
#include "../../guilds/GuildApplicationSignature.h"
#include "../../guilds/GuildJoinApplication.h"
#include "../../guilds/GuildJoinApplicationStatus.h"
#include "../../guilds/UserGuild.h"
#include "../../Exception.h"
#include "../../Game.h"

#include "GeneralTestCase.h"

extern Descriptor *descriptor_list;

void GeneralTestCase::setup()
{
	Log("Setting Up Generic Test Case.");
}

void GeneralTestCase::process()
{
	/***
	class TestDescriptor : public Descriptor
	{
	protected:
		std::string outputSeen;
	public:

		TestDescriptor() : Descriptor() {}

		virtual void appendToOutputBuffer(const std::string &str)
		{
			outputSeen.append(str);
		}

		virtual void appendToOutputBuffer(const char *str)
		{
			outputSeen.append(str);
		}

		bool saw(const std::string pattern)
		{
			return outputSeen.find(pattern) != std::string::npos;
		}

		void clearOutputSeen()
		{
			outputSeen.clear();
		}

		void initMock(class Character *ch)
		{
			descriptor = new kuDescriptor(nullptr);
			strcpy(host, "127.0.0.1");
			setGatewayDescriptorType(GatewayDescriptorType::unknown);
			connected = CON_PLAYING;
			idle_tics = 0;
			wait = 0;
			loginTime = DateTime();
			hadInput = false;
			hadOutput = false;
			desc_num = -1;
			character = ch;
			next = nullptr;
		}
	};

	typedef std::function<void(TestDescriptor *d, class CommandTest *ct)> CommandTestBefore;
	typedef std::function<void(TestDescriptor *d, class CommandTest *ct)> CommandTestAfter;

	class CommandTest
	{
	protected:
	public:

		CommandTest(TestDescriptor *d, const std::string &command)
		{
			this->d = d;
			commands.push_back(command);
		}
		CommandTest(TestDescriptor *d, const std::string &command, const CommandTestBefore &beforeHandle, const CommandTestAfter &afterHandle)
			: beforeHandle(beforeHandle),
			  afterHandle(afterHandle)
		{
			commands.push_back(command);
			this->d = d;
		}

		CommandTest(TestDescriptor *d, const std::vector<std::string> &commands, const CommandTestBefore &beforeHandle, const CommandTestAfter &afterHandle)
			: commands(commands),
			  beforeHandle(beforeHandle),
			  afterHandle(afterHandle),
			  d(d) {}

		CommandTest(TestDescriptor *d, const std::vector<std::string> &commands, const CommandTestAfter &afterHandle)
			: commands(commands),
			  afterHandle(afterHandle),
			  d(d) {}

		void performAssert(bool assertion)
		{
			if(!assertion)
			{
				core_dump();
				throw Exception("Assertion failed.");
			}
		}

		std::optional<CommandTestBefore> beforeHandle;
		std::optional<CommandTestAfter> afterHandle;
		std::vector<std::string> commands;
		TestDescriptor *d;
		int nr;
	};

	std::vector<CommandTest> commandTestQueue;

	ZoneManager::GetManager().GetZoneByVnum(1)->Reset();

	auto jack = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Jack"));
	auto lamgwin = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Lamgwin"));
	auto roran = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Roran"));
	auto qmoney = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Qmoney"));
	auto galnor = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Galnor"));
	auto crunch = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Crunch"));
	auto lamb = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Lamb"));
	auto shepard = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Shepard"));

	auto characters = {jack, lamgwin, roran, qmoney, galnor, crunch, lamb, shepard};

	auto jackDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto lamgwinDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto roranDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto qmoneyDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto galnorDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto crunchDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto lambDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());
	auto shepardDesc = std::shared_ptr<TestDescriptor>(new TestDescriptor());

	jack->desc = jackDesc.get();
	lamgwin->desc = lamgwinDesc.get();
	roran->desc = roranDesc.get();
	qmoney->desc = qmoneyDesc.get();
	galnor->desc = galnorDesc.get();
	crunch->desc = crunchDesc.get();
	lamb->desc = lambDesc.get();
	shepard->desc = shepardDesc.get();
	
	for(auto character : characters)
	{
		((TestDescriptor*)character->desc)->initMock(character.get());
		character->MoveToRoom(201);
		
		for(auto application : GuildUtil::get()->getGuildApplications(character->player.idnum, GuildApplicationStatus::allExceptDenied()))
		{
			application->setStatus(GuildApplicationStatus::denied);
			GuildUtil::get()->putGuildApplication(game->getConnection(), application);
		}
	}

	//Test gate calling.
	commandTestQueue.push_back(CommandTest(lamgwinDesc.get(), "call",
		[](TestDescriptor *d, CommandTest *ct) -> void {
			d->character->MoveToRoom(FindRoomByVnum(112));
			GET_RACE(d->character) = RACE_HUMAN;
			d->character->in_room->setDoorBit(EAST, EX_CLOSED);
			d->character->in_room->setDoorBit(EAST, EX_LOCKED);
		},
		[](TestDescriptor *d, CommandTest *ct) -> void {
			ct->performAssert(d->character->in_room->dir_option[EAST]->isOpen());
			ct->performAssert(d->saw("opens the gate."));
		}
		));

	//Test action editing.
	commandTestQueue.push_back(CommandTest(galnorDesc.get(),
		{"aedit snicker", "y", "a", "You snicker loudly.", "q", "y"},
		[](TestDescriptor *d, CommandTest *ct) -> void {
			ct->performAssert(!strcmp(CommandUtil::get()->getSocialByAbbreviation("snicker")->char_no_arg, "You snicker loudly."));
		}
	));

	//Test Guild Creation Application submission.
	commandTestQueue.push_back(CommandTest(jackDesc.get(),
		{"guilds", "c", "c", "n", "The Jackuits", "d", "IT DOTH BE IMMACULATELY CONCEIVED", "s", "q", "q"},
		[=](TestDescriptor *d, CommandTest *ct) -> void {
			d->character->points.gold = GuildUtil::get()->getCoppersToCreateNewGuild();
			ct->nr = GuildUtil::get()->getGuildApplications().size();

			//Deny any application that Jack currently has up.
			for(auto application : GuildUtil::get()->getGuildApplications(std::optional<int>(), {GuildApplicationStatus::pending, GuildApplicationStatus::reviewing}))
			{
				if(application->getSubmittedByUserId() == d->character->getUserId())
					GuildUtil::get()->denyGuildApplication(game->getConnection(), application->getId(), application->getSubmittedByUserId(), "Testing.");
			}

			//Remove the players involved from all Guilds.
			for(int userId : {jack->getUserId(), lamgwin->getUserId(), roran->getUserId(), qmoney->getUserId()})
			{
				for(auto userGuild : GuildUtil::get()->getUserGuildsByUserId(userId, {UserGuildStatus::active}))
				{
					GuildUtil::get()->removeUserFromGuild(game->getConnection(), userGuild->getId());
				}
			}

			//Remove all Guilds.
			for(auto guild : GuildUtil::get()->getGuilds({GuildStatus::active}))
			{
				guild->setStatus(GuildStatus::abandoned);
				guild->setStatusLastModifiedDatetime(DateTime());
				GuildUtil::get()->putGuild(game->getConnection(), guild);
			}
		},
		[](TestDescriptor *d, CommandTest *ct) -> void {
			ct->performAssert(GuildUtil::get()->getGuildApplications().size() > ct->nr);

			auto application = GuildUtil::get()->getGuildApplications().back();

			ct->performAssert(!strcmp(application->getGuildDescription().c_str(), "IT DOTH BE IMMACULATELY CONCEIVED"));
			ct->performAssert(!strcmp(application->getGuildName(), "The Jackuits"));
			ct->performAssert(application->getStatus() == GuildApplicationStatus::pending);
			ct->performAssert(application->getCoppersCharged() == GuildUtil::get()->getCoppersToCreateNewGuild());
			ct->performAssert(!application->getDeniedReason().has_value());
			ct->performAssert(!application->isNew());
			ct->performAssert(application->getSubmittedByUserId() == d->character->getUserId());
			ct->performAssert(!application->getCompletedDatetime().has_value());
			ct->performAssert(!application->getReviewerUserId().has_value());
			ct->performAssert(application->getSubmittedByUserRace() == GET_RACE(d->character));
			ct->performAssert(d->character->points.gold == 0);
		}
	));
	
	//Test signing guild join application.
	CommandTest guildSignatureTesting( lamgwinDesc.get(),
		{"guilds", "c", "GUILD_ID", "s", "q", "q", "q"},
		[](TestDescriptor *d, CommandTest *ct) -> void
		{
			MudLog(BRF, LVL_APPR, TRUE, "%s is about to sign the guild application.", GET_NAME(d->character));
			ct->nr = GuildUtil::get()->getGuildApplicationSignaturesByUserId(d->character->getUserId()).size();
			ct->commands[2] = MiscUtil::toString(GuildUtil::get()->getGuildApplicationsSorted().back()->getId());
		},
		[](TestDescriptor *d, CommandTest *ct) -> void
		{
			ct->performAssert(GuildUtil::get()->getGuildApplicationSignaturesByUserId(d->character->getUserId()).size() > ct->nr);
		}
	);

	commandTestQueue.push_back(guildSignatureTesting);

	guildSignatureTesting.d = roranDesc.get();
	commandTestQueue.push_back(guildSignatureTesting);

	guildSignatureTesting.d = qmoneyDesc.get();
	commandTestQueue.push_back(guildSignatureTesting);

	//Test approving guild signatures.
	commandTestQueue.push_back(CommandTest(jackDesc.get(),
		{ "guilds", "c", "GUILD_ID", "p", "ID", "p", "ID", "p", "ID", "q", "q", "q"},
		[](TestDescriptor *d, CommandTest *ct) -> void
		{
			ct->nr = GuildUtil::get()->getGuildApplicationsSorted().back()->getId();
			ct->commands[2] = MiscUtil::toString(ct->nr);

			auto signatures = GuildUtil::get()->getGuildApplicationSignaturesByGuildApplicationId(ct->nr);

			ct->commands[4] = MiscUtil::toString(signatures[0]->getId());
			ct->commands[6] = MiscUtil::toString(signatures[1]->getId());
			ct->commands[8] = MiscUtil::toString(signatures[2]->getId());
		},
		[](TestDescriptor *d, CommandTest *ct) -> void
		{
			//Test the status of the Guild, mainly. It should now be Reviewing after the signatures are accepted.
			auto application = GuildUtil::get()->getGuildApplication(ct->nr);
			ct->performAssert(application->getStatus() == GuildApplicationStatus::reviewing);
		}
	));

	commandTestQueue.push_back(CommandTest(galnorDesc.get(),
		{"guilds", "c", "GUILD_ID", "a", "y", "q", "q", "q"},
		[](TestDescriptor *d, CommandTest *ct) -> void
		{
			ct->nr = GuildUtil::get()->getGuildApplicationsSorted().back()->getId();
			ct->commands[2] = MiscUtil::toString(ct->nr);
		},
		[=](TestDescriptor *d, CommandTest *ct) -> void
		{
			auto userGuild = GuildUtil::get()->getUserGuildsByUserId(jack->getUserId(), {UserGuildStatus::active})[0];
			auto guild = GuildUtil::get()->getGuild(userGuild->getGuildId());

			ct->performAssert(guild->getCreatedByUserId() == jack->getUserId());
			ct->performAssert(!strcmp(guild->getName().c_str(), "The Jackuits"));
			ct->performAssert(!guild->isNew());
			ct->performAssert(guild->getStatus() == GuildStatus::active);
			ct->performAssert(guild->getRace() == GET_RACE(jack));
			
			ct->performAssert(userGuild->getUserId() == jack->getUserId());
			ct->performAssert(userGuild->getStatus() == UserGuildStatus::active);

			ct->performAssert(!GuildUtil::get()->getUserGuildsByUserId(lamgwin->getUserId(), {UserGuildStatus::active}).empty());
			ct->performAssert(!GuildUtil::get()->getUserGuildsByUserId(roran->getUserId(), {UserGuildStatus::active}).empty());
			ct->performAssert(!GuildUtil::get()->getUserGuildsByUserId(qmoney->getUserId(), {UserGuildStatus::active}).empty());
		}
	));

	//Test Lamb submitting a Guild join application for the newly created Guild.
	//First test him with a level that is too low.
	commandTestQueue.push_back(CommandTest(lambDesc.get(),
		{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
		[=](TestDescriptor *d, CommandTest *ct) -> void
		{
			GET_LEVEL(d->character) = GuildUtil::get()->getMinimumLevelToJoinGuild() - 1;
			ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
		},
		[=](TestDescriptor *d, CommandTest *ct) -> void
		{
			std::string expectedMessage = std::string("You must be at least level ")
				+ MiscUtil::toString(GuildUtil::get()->getMinimumLevelToJoinGuild())
				+ " to join a Guild.";

			ct->performAssert(d->saw(expectedMessage));
			ct->performAssert(GuildUtil::get()->getActiveUserGuildsByUserId(lamb->player.idnum).empty());
		}
	));

	//Test Lamb joining Guild with a pending Guild Create Application.
	commandTestQueue.push_back(CommandTest(lambDesc.get(),
	{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		GET_LEVEL(d->character) = GuildUtil::get()->getMinimumLevelToJoinGuild();
		ct->nr = GuildUtil::get()->submitGuildApplication(game->getConnection(), d->character->player.idnum, GET_RACE(lamb), "Lamb Wannabe Guild", "Desc", GuildUtil::get()->getCoppersToCreateNewGuild())->getId();
		ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
	},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		ct->performAssert(GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).empty());
		ct->performAssert(d->saw("You have another Guild Creation Application that is pending/reviewing."));
	}));

	//Test Shepard joining Guild with a signature.
	commandTestQueue.push_back(CommandTest(shepardDesc.get(),
	{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		GET_LEVEL(d->character) = GuildUtil::get()->getMinimumLevelToJoinGuild();
		ct->nr = GuildUtil::get()->getGuildApplications(lamb->player.idnum, {GuildApplicationStatus::pending}).back()->getId();
		GuildUtil::get()->submitGuildApplicationSignature(game->getConnection(), shepard->player.idnum, ct->nr)->getId();
		ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
	},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		//We're done with the creation application now.
		GuildUtil::get()->denyGuildApplication(game->getConnection(), ct->nr, galnor->player.idnum, "Denied.");
		ct->performAssert(GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).empty());
		ct->performAssert(d->saw("You have a signature on a Guild Creation Application that is still open."));
	}));

	//Test Lamgwin trying to join the Guild. He is in another Guild already and should not be allowed to join.
	commandTestQueue.push_back(CommandTest(lamgwinDesc.get(),
	{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		ct->nr = GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).size();
		ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
	},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		ct->performAssert(ct->nr == GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).size());
		ct->performAssert(d->saw("You are already in another Guild."));
	}));

	//Test Shepard trying to join a Guild despite having another application out there.
	commandTestQueue.push_back(CommandTest(shepardDesc.get(),
	{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		GuildUtil::get()->submitGuildJoinApplication(game->getConnection(), d->character->player.idnum, GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId(), "Ohai");
		ct->nr = GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).size();
		ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
	},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		for(auto application : GuildUtil::get()->getGuildJoinApplications(d->character->player.idnum, std::optional<int>()))
		{
			if(application->getStatus() != GuildJoinApplicationStatus::denied)
			{
				application->setStatus(GuildJoinApplicationStatus::denied);
				GuildUtil::get()->putGuildJoinApplication(game->getConnection(), application);
			}
		}
		ct->performAssert(ct->nr == GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).size());
		ct->performAssert(d->saw("You have another pending application to join a Guild."));
	}));

	//Test Shepard successfully joining a Guild.
	commandTestQueue.push_back(CommandTest(shepardDesc.get(),
	{"guilds", "j", "<GUILD_OPTION>", "a", "q", "q", "q"},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		//Applying should still work in this scenario. The application is denied!
		auto application = GuildUtil::get()->submitGuildApplication(game->getConnection(), lamb->player.idnum, GET_RACE(d->character), "Some Guild Name", "Some Desc.", 10000);
		GuildUtil::get()->submitGuildApplicationSignature(game->getConnection(), shepard->player.idnum, application->getId());
		GuildUtil::get()->denyGuildApplication(game->getConnection(), application->getId(), galnor->player.idnum, "Denied!");

		ct->commands[2] = std::string("G") + MiscUtil::toString(GuildUtil::get()->getActiveUserGuildsByUserId(jack->player.idnum).back()->getGuildId());
	},
	[=](TestDescriptor *d, CommandTest *ct) -> void
	{
		ct->performAssert(GuildUtil::get()->getActiveUserGuildsByUserId(d->character->player.idnum).size() == 1);
	}));

	while(!commandTestQueue.empty())
	{
		CommandTest commandTest = commandTestQueue.front();
		commandTestQueue.erase(commandTestQueue.begin());

		if(commandTest.beforeHandle)
			commandTest.beforeHandle.get()(commandTest.d, &commandTest);

		for(auto command : commandTest.commands)
		{
			commandTest.d->commandQueue.push_back(command);
			commandTest.d->processInput();
		}

		if(commandTest.afterHandle)
		{
			try
			{
				commandTest.afterHandle.get()(commandTest.d, &commandTest);
			}
			catch(Exception e)
			{
				MudLog(BRF, TRUE, LVL_APPR, "General Test Case failed. Command entered `%s` by `%s`.", StringUtil::implode(commandTest.commands, ", ").c_str(), GET_NAME(commandTest.d->character));
				break;
			}
		}

		for(auto character : characters)
		{
			((TestDescriptor*)character->desc)->clearOutputSeen();
		}
	}
	***/
}

void GeneralTestCase::cleanup()
{
	Log("Cleaning Up Generic Test Case.");
}
