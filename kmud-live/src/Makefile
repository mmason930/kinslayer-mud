
# CircleMUD 3.0 Makefile.in - Makefile template used by 'configure'
#

# C compiler to use
#CC = /usr/local/bin/i686-pc-linux-gnu-g++
CC = g++

#-std=c++0x
# Any special flags you want to pass to the compiler
MYFLAGS =  -O0 -std=c++0x -DXP_UNIX -I/usr/local/include -Wformat-security -Wformat -Wpointer-arith -Wcast-align -Wredundant-decls \
 		-L/usr/local/lib -DJS_C_STRINGS_ARE_UTF8 -DBOOST_PARAMETER_MAX_ARITY=10 \
              -I/usr/local/lib/pkgconfig/../../include -I/usr/local/include

#-Werror

#flags for profiling (see hacker.doc for more information)
PROFILE =

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

BINDIR = ../bin

CPPFLAGS = -ggdb3 $(MYFLAGS) $(PROFILE)

LIBS = -lsqlDatabase -lmysqlclient -lflusspferd -lcrypt -lstdc++ -lboost_regex -lboost_filesystem -lmozjs -lboost_thread

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
			aedit.o \
			auction.o \
			ban.o \
			cedit.o \
			CharacterUtil.o \
			ClanQuestPointTransaction.o \
			clans.o \
			ClanUtil.o \
			class.o \
			cledit.o \
			comm.o \
			commands/immortals/Locate.o \
			commands/items/Get.o \
			commands/infrastructure/CommandUtil.o \
			commands/infrastructure/Social.o \
			config.o \
			constants.o \
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
			GatewayClient.o \
			GatewayDescriptor.o \
			GatewayDescriptorStatus.o \
			GatewayDescriptorType.o \
			GatewayListener.o \
			GatewayServer.o \
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
			jedit.o \
			js.o \
			js_constants.o \
			js_functions.o \
			js_interpreter.o \
			js_utils.o \
			JSCharacter.o \
			JSObject.o \
			jsoncpp/json_reader.o \
			jsoncpp/json_value.o \
			jsoncpp/json_writer.o \
			JSQuery.o \
			JSRoom.o \
			JSRow.o \
			kedit.o \
			kuClient.o \
			kuDescriptor.o \
			kuListener.o \
			kuSockets.o \
			limits.o \
			magic.o \
			MailUtil.o \
			md5.o \
			medit.o \
			MiscUtil.o \
			mobact.o \
			MobLoadLogger.o \
			mobs.o \
			modify.o \
			MudStatus.o \
			ObjectMoveLogger.o \
			objsave.o \
			oedit.o \
			olc.o \
			OlcEditType.o \
			OLCUtil.o \
			PlayerPortalCommandProcessor.o \
			PlayerPortalDescriptor.o \
			PlayerPortalDescriptorStatus.o \
			PlayerPortalServer.o \
			poker.o \
			redit.o \
			rooms/Exit.o \
			rooms/Room.o \
			rooms/RoomSector.o \
			Script.o \
			sedit.o \
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
			WebSocketClientHeaderHixie76.o \
			WebSocketClientHeaderIETF_HYBI17.o \
			WebSocketDataFrame.o \
			wedit.o \
			zedit.o \
			zones.o \

OBJFILES := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILES))


OBJFILESGATEWAY = DateTime.o GatewayClient.o GatewayDescriptor.o GatewayDescriptorStatus.o \
	GatewayMain.o GatewayServer.o kuClient.o kuDescriptor.o kuListener.o kuSockets.o MiscUtil.o \
	StringUtil.o MudStatus.o SystemUtil.o sha1.o GatewayDescriptorType.o WebSocketDataFrame.o \
	WebSocketClientHeaderIETF_HYBI17.o WebSocketClientHeader.o WebSocketClientHeaderHixie76.o \
	GatewayListener.o jsoncpp/json_reader.o jsoncpp/json_writer.o jsoncpp/json_value.o

OBJFILESGATEWAY := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILESGATEWAY))


$(OBJDIR)/%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< | \
	sed '\''s,\($*\)\.o[ :]*,$(OBJDIR)\/\1.o $@ : ,g'\'' > $@'

default: circle

-include $(OBJFILES:.o)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -c	$<	-o $@

all:
	$(MAKE) $(BINDIR)/circle
	$(MAKE) $(BINDIR)/gateway

circle:
	$(MAKE) $(BINDIR)/circle

$(BINDIR)/circle : $(OBJFILES)
	$(CC) -o $(BINDIR)/circle $(PROFILE) $(OBJFILES) $(LIBS)

gateway:
	$(MAKE) $(BINDIR)/gateway

$(BINDIR)/gateway : $(OBJFILESGATEWAY)
	$(CC) -o $(BINDIR)/gateway $(PROFILE) $(OBJFILESGATEWAY) $(LIBS)

clean:
	rm -f $(OBJFILES) *~

full:
	clean
	all
