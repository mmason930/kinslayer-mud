#include "../../conf.h"
#include "../../sysdep.h"
#include "../../structs.h"
#include "../../db.h"
#include "../../handler.h"
#include "../../interpreter.h"
#include "../../comm.h"
#include "../../fight.h"
#include "../../screen.h"
#include "../../olc.h"
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

	typedef std::function<void(Descriptor *d, class CommandTest *ct)> CommandTestBefore;
	typedef std::function<void(Descriptor *d, class CommandTest *ct)> CommandTestAfter;

	class CommandTest
	{
	protected:
	public:

		CommandTest(Descriptor *d, const std::string &command)
		{
			this->d = d;
			commands.push_back(command);
		}
		CommandTest(Descriptor *d, const std::string &command, const CommandTestBefore &beforeHandle, const CommandTestAfter &afterHandle)
			: beforeHandle(beforeHandle),
			  afterHandle(afterHandle)
		{
			commands.push_back(command);
			this->d = d;
		}

		CommandTest(Descriptor *d, const std::vector<std::string> &commands, const CommandTestBefore &beforeHandle, const CommandTestAfter &afterHandle)
			: commands(commands),
			  beforeHandle(beforeHandle),
			  afterHandle(afterHandle),
			  d(d) {}

		CommandTest(Descriptor *d, const std::vector<std::string> &commands, const CommandTestAfter &afterHandle)
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

		boost::optional<CommandTestBefore> beforeHandle;
		boost::optional<CommandTestAfter> afterHandle;
		std::vector<std::string> commands;
		Descriptor *d;
		int nr;
	};

	std::vector<CommandTest> commandTestQueue;

	ZoneManager::GetManager().GetZoneByVnum(1)->Reset();

	auto jack = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Jack"));
	auto lamgwin = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Lamgwin"));
	auto roran = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Roran"));
	auto qmoney = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Qmoney"));
	auto galnor = std::shared_ptr<Character>(CharacterUtil::loadCharacter("Galnor"));

	auto jackDesc = std::shared_ptr<Descriptor>(new Descriptor());
	auto lamgwinDesc = std::shared_ptr<Descriptor>(new Descriptor());
	auto roranDesc = std::shared_ptr<Descriptor>(new Descriptor());
	auto qmoneyDesc = std::shared_ptr<Descriptor>(new Descriptor());
	auto galnorDesc = std::shared_ptr<Descriptor>(new Descriptor());

	jack->desc = jackDesc.get();
	jack->desc->initMock(jack.get());
	jack->MoveToRoom(FindRoomByVnum(201));

	lamgwin->desc = lamgwinDesc.get();
	lamgwin->desc->initMock(lamgwin.get());
	lamgwin->MoveToRoom(FindRoomByVnum(201));

	roran->desc = roranDesc.get();
	roran->desc->initMock(roran.get());
	roran->MoveToRoom(FindRoomByVnum(201));

	qmoney->desc = qmoneyDesc.get();
	qmoney->desc->initMock(qmoney.get());
	qmoney->MoveToRoom(FindRoomByVnum(201));

	galnor->desc = galnorDesc.get();
	galnor->desc->initMock(galnor.get());
	galnor->MoveToRoom(FindRoomByVnum(201));

	commandTestQueue.push_back(CommandTest(lamgwin->desc, "call",
		[](Descriptor *d, CommandTest *ct) -> void {
			d->character->MoveToRoom(FindRoomByVnum(112));
			GET_RACE(d->character) = RACE_HUMAN;
			d->character->in_room->setDoorBit(EAST, EX_CLOSED);
			d->character->in_room->setDoorBit(EAST, EX_LOCKED);
		},
		[](Descriptor *d, CommandTest *ct) -> void {
			ct->performAssert(d->character->in_room->dir_option[EAST]->isOpen());
		}
		));


	commandTestQueue.push_back(CommandTest(galnor->desc,
		{"aedit snicker", "y", "a", "You snicker loudly.", "q", "y"},
		[](Descriptor *d, CommandTest *ct) -> void {
			ct->performAssert(!strcmp(CommandUtil::get()->getSocialByAbbreviation("snicker")->char_no_arg, "You snicker loudly."));
		}
	));

	commandTestQueue.push_back(CommandTest(jack->desc,
		{"guilds", "c", "c", "n", "The Jackuits", "d", "IT DOTH BE IMMACULATELY CONCEIVED", "s", "q", "q"},
		[=](Descriptor *d, CommandTest *ct) -> void {
			d->character->points.gold = GuildUtil::get()->getCoppersToCreateNewGuild();
			ct->nr = GuildUtil::get()->getGuildApplications().size();

			//Deny any application that Jack currently has up.
			for(auto application : GuildUtil::get()->getGuildApplications(boost::optional<int>(), {GuildApplicationStatus::pending, GuildApplicationStatus::reviewing}))
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
		[](Descriptor *d, CommandTest *ct) -> void {
			ct->performAssert(GuildUtil::get()->getGuildApplications().size() > ct->nr);

			auto application = GuildUtil::get()->getGuildApplications().back();

			ct->performAssert(!strcmp(application->getGuildDescription().c_str(), "IT DOTH BE IMMACULATELY CONCEIVED"));
			ct->performAssert(!strcmp(application->getGuildName(), "The Jackuits"));
			ct->performAssert(application->getStatus() == GuildApplicationStatus::pending);
			ct->performAssert(application->getCoppersCharged() == GuildUtil::get()->getCoppersToCreateNewGuild());
			ct->performAssert(!application->getDeniedReason().is_initialized());
			ct->performAssert(!application->isNew());
			ct->performAssert(application->getSubmittedByUserId() == d->character->getUserId());
			ct->performAssert(!application->getCompletedDatetime().is_initialized());
			ct->performAssert(!application->getReviewerUserId().is_initialized());
			ct->performAssert(application->getSubmittedByUserRace() == GET_RACE(d->character));
			ct->performAssert(d->character->points.gold == 0);
		}
	));
	

	CommandTest guildSignatureTesting( lamgwin->desc,
		{"guilds", "c", "GUILD_ID", "s", "q", "q", "q"},
		[](Descriptor *d, CommandTest *ct) -> void
		{
			ct->nr = GuildUtil::get()->getGuildApplicationSignaturesByUserId(d->character->getUserId()).size();
			ct->commands[2] = MiscUtil::toString(GuildUtil::get()->getGuildApplicationsSorted().back()->getId());
		},
		[](Descriptor *d, CommandTest *ct) -> void
		{
			ct->performAssert(GuildUtil::get()->getGuildApplicationSignaturesByUserId(d->character->getUserId()).size() > ct->nr);
		}
	);

	commandTestQueue.push_back(guildSignatureTesting);

	guildSignatureTesting.d = roran->desc;
	commandTestQueue.push_back(guildSignatureTesting);

	guildSignatureTesting.d = qmoney->desc;
	commandTestQueue.push_back(guildSignatureTesting);

	commandTestQueue.push_back(CommandTest(jack->desc,
		{ "guilds", "c", "GUILD_ID", "p", "ID", "p", "ID", "p", "ID", "q", "q", "q"},
		[](Descriptor *d, CommandTest *ct) -> void
		{
			ct->nr = GuildUtil::get()->getGuildApplicationsSorted().back()->getId();
			ct->commands[2] = MiscUtil::toString(ct->nr);

			auto signatures = GuildUtil::get()->getGuildApplicationSignaturesByGuildApplicationId(ct->nr);

			ct->commands[4] = MiscUtil::toString(signatures[0]->getId());
			ct->commands[6] = MiscUtil::toString(signatures[1]->getId());
			ct->commands[8] = MiscUtil::toString(signatures[2]->getId());
		},
		[](Descriptor *d, CommandTest *ct) -> void
		{
			//Test the status of the Guild, mainly. It should now be Reviewing after the signatures are accepted.
			auto application = GuildUtil::get()->getGuildApplication(ct->nr);
			ct->performAssert(application->getStatus() == GuildApplicationStatus::reviewing);
		}
	));

	commandTestQueue.push_back(CommandTest(galnor->desc,
		{"guilds", "c", "GUILD_ID", "a", "y", "q", "q", "q"},
		[](Descriptor *d, CommandTest *ct) -> void
		{
			ct->nr = GuildUtil::get()->getGuildApplicationsSorted().back()->getId();
			ct->commands[2] = MiscUtil::toString(ct->nr);
		},
		[=](Descriptor *d, CommandTest *ct) -> void
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
	}
}

void GeneralTestCase::cleanup()
{
	Log("Cleaning Up Generic Test Case.");
}
