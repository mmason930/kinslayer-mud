
# KinslayerMUD Makefile
#

# C/C++ compiler to use
CC = g++

# Any special flags you want to pass to the compiler
MYFLAGS = -O0 -std=c++2a -DXP_UNIX -Wformat-security -Wformat -Wpointer-arith -Wcast-align -Wredundant-decls \
 		-L/usr/local/lib -DJS_C_STRINGS_ARE_UTF8 -DBOOST_PARAMETER_MAX_ARITY=10 -I/usr/local/include

#-Werror

#flags for profiling (see hacker.doc for more information)
PROFILE =

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

BINDIR = ../bin

CPPFLAGS = -ggdb3 $(MYFLAGS) $(PROFILE)

LIBS = -lsqlDatabase -lflusspferd -lstdc++ -lboost_regex -lmozjs -lmysqlclient -lboost_thread -lboost_filesystem -lcrypt -lboost_system -lpthread -ldl

OBJDIR = obj

OBJFILES =	accounts.o \
			act.comm.o \
			act.informative.o \
			act.item.o \
			act.movement.o \
			act.offensive.o \
			act.other.o \
			act.social.o \
			act.wizard.o \
			olc/aedit.o \
			auction.o \
			ban.o \
			olc/cedit.o \
			CharacterUtil.o \
			ClanQuestPointTransaction.o \
			clans.o \
			ClanUtil.o \
			class.o \
			olc/cledit.o \
			comm.o \
			commands/immortals/Locate.o \
			commands/items/Get.o \
			commands/infrastructure/CommandUtil.o \
			commands/infrastructure/Social.o \
			config.o \
			constants.o \
			db-migrations/DatabaseMigrationStatus.o \
			db-migrations/DatabaseMigrationUtil.o \
			DatabaseUtil.o \
			DateTime.o \
			db.o \
			dbutils/QueryUtil.o \
			Descriptor.o \
			dg_event.o \
			dotmode/DotMode.o \
			dotmode/DotModeArea.o \
			dotmode/DotModeTarget.o \
			dotmode/DotModeTargetType.o \
			dotmode/DotModeUtil.o \
			editor-interface/EditorInterfaceBase.o \
			editor-interface/EditorInterfaceInstance.o \
			editor-interface/EditorInterfaceMenu.o \
			EntityType.o \
			fight.o \
			ForumUtil.o \
			Game.o \
			gateway/GatewayDescriptor.o \
			gateway/GatewayDescriptorStatus.o \
			gateway/GatewayDescriptorType.o \
			gateway/GatewayListener.o \
			graph.o \
			guilds/Guild.o \
			guilds/GuildApplication.o \
			guilds/GuildApplicationSignature.o \
			guilds/GuildApplicationSignatureStatus.o \
			guilds/GuildApplicationStatus.o \
			guilds/GuildEditorInterface.o \
			guilds/GuildJoinApplication.o \
			guilds/GuildJoinApplicationStatus.o \
			guilds/GuildPrivilege.o \
			guilds/GuildRank.o \
			guilds/GuildRankRole.o \
			guilds/GuildRankStatus.o \
			guilds/GuildStatus.o \
			guilds/GuildUtil.o \
			guilds/UserGuild.o \
			guilds/UserGuildStatus.o \
			handler.o \
			interpreter.o \
			items/ItemUtil.o \
			olc/jedit.o \
			js/js.o \
			js/js_constants.o \
			js/js_functions.o \
			js/js_interpreter.o \
			js/js_utils.o \
			js/JSCharacter.o \
			js/JSObject.o \
			jsoncpp/json_reader.o \
			jsoncpp/json_value.o \
			jsoncpp/json_writer.o \
			js/JSQuery.o \
			js/JSRoom.o \
			js/JSRow.o \
			kedit.o \
			ku/kuClient.o \
			ku/kuDescriptor.o \
			ku/kuListener.o \
			ku/kuSockets.o \
			limits.o \
			magic.o \
			MailUtil.o \
			md5.o \
			olc/medit.o \
			MiscUtil.o \
			mobact.o \
			MobLoadLogger.o \
			mobs.o \
			modify.o \
			MudStatus.o \
			ObjectMoveLogger.o \
			objsave.o \
			olc/oedit.o \
			olc/olc.o \
			olc/OlcEditType.o \
			PlayerPortalCommandProcessor.o \
			PlayerPortalDescriptor.o \
			PlayerPortalDescriptorStatus.o \
			PlayerPortalServer.o \
			poker.o \
			olc/redit.o \
			rooms/Exit.o \
			rooms/Room.o \
			rooms/RoomSector.o \
			js/Script.o \
			olc/sedit.o \
			sha1.o \
			shop.o \
			spec_assign.o \
			spec_procs.o \
			spell_parser.o \
			spells.o \
			SQLUtil.o \
			statedit.o \
			stats.o \
			StringUtil.o \
			SystemUtil.o \
			test/general/GeneralTestCase.o \
			test/TestUtil.o \
			TextTableBuilder.o \
			threaded_jobs.o \
			TimeUtil.o \
			UserClan.o \
			UserEmailEditor.o \
			UserLogoutType.o \
			UserType.o \
			utils.o \
			utils/ThreadedLogFile.o \
			weather.o \
			weave.o \
			WebSocketClientHeader.o \
			WebSocketClientHeaderIETF_HYBI17.o \
			WebSocketDataFrame.o \
			olc/wedit.o \
			olc/zedit.o \
			zones.o \

OBJFILES := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILES))


OBJFILESGATEWAY = DateTime.o gateway/GatewayDescriptor.o gateway/GatewayDescriptorStatus.o \
	gateway/GatewayMain.o gateway/GatewayServer.o kuClient.o kuDescriptor.o kuListener.o \
	kuSockets.o MiscUtil.o StringUtil.o MudStatus.o SystemUtil.o sha1.o GatewayDescriptorType.o \
	WebSocketDataFrame.o WebSocketClientHeaderIETF_HYBI17.o WebSocketClientHeader.o \
	gateway/GatewayListener.o jsoncpp/json_reader.o jsoncpp/json_writer.o jsoncpp/json_value.o

OBJFILESGATEWAY := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILESGATEWAY))


$(OBJDIR)/%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< | \
	sed '\''s,\($*\)\.o[ :]*,$(OBJDIR)\/\1.o $@ : ,g'\'' > $@'

default: kinslayer

-include $(OBJFILES:.o)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c	$<	-o $@

all:
	@mkdir -p ../bin
	$(MAKE) $(BINDIR)/kinslayer
	$(MAKE) $(BINDIR)/gateway

kinslayer:
	@mkdir -p ../bin
	$(MAKE) $(BINDIR)/kinslayer

$(BINDIR)/kinslayer : $(OBJFILES)
	$(CC) -o $(BINDIR)/kinslayer $(PROFILE) $(OBJFILES) $(LIBS)

gateway:
	@mkdir -p ../bin
	$(MAKE) $(BINDIR)/gateway

$(BINDIR)/gateway : $(OBJFILESGATEWAY)
	$(CC) -o $(BINDIR)/gateway $(PROFILE) $(OBJFILESGATEWAY) $(LIBS)

clean:
	rm -f $(OBJFILES) *~

full:
	clean
	all
