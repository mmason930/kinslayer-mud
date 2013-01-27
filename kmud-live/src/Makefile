
# CircleMUD 3.0 Makefile.in - Makefile template used by 'configure'
#

# C compiler to use
#CC = /usr/local/bin/i686-pc-linux-gnu-g++
CC = g++

#-std=c++0x
# Any special flags you want to pass to the compiler
MYFLAGS =  -O -std=c++0x -DXP_UNIX -I/usr/local/include -Wformat-security -Wformat -Wpointer-arith -Wcast-align -Wredundant-decls \
 		-L/usr/local/lib -DKINSLAYER_JAVASCRIPT -DJS_C_STRINGS_ARE_UTF8 -DBOOST_PARAMETER_MAX_ARITY=10 \
              -I/usr/local/lib/pkgconfig/../../include -I/usr/local/include

#-Werror

#flags for profiling (see hacker.doc for more information)
PROFILE =

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

BINDIR = ../bin

CPPFLAGS = -ggdb3 $(MYFLAGS) $(PROFILE)

LIBS = -lmysqlclient -lsqlDatabase -lflusspferd -lcrypt -lstdc++ -lboost_regex -lboost_filesystem -lmozjs -lboost_thread

OBJDIR = obj

OBJFILES = comm.o act.comm.o act.informative.o act.movement.o act.item.o \
	act.offensive.o act.other.o act.social.o act.wizard.o ban.o \
	clans.o class.o config.o constants.o db.o fight.o graph.o handler.o \
	interpreter.o limits.o magic.o mobact.o modify.o objsave.o \
	olc.o poker.o shop.o spec_assign.o spec_procs.o zones.o \
	spell_parser.o spells.o utils.o weather.o redit.o oedit.o zedit.o \
	medit.o sedit.o cedit.o md5.o accounts.o dg_event.o aedit.o kedit.o \
	cledit.o wedit.o stats.o statedit.o weave.o \
	mobs.o js.o js_interpreter.o js_functions.o jedit.o js_constants.o \
	JSRoom.o JSObject.o JSQuery.o JSRow.o JSCharacter.o js_utils.o \
	auction.o kuSockets.o kuListener.o kuDescriptor.o kuClient.o threaded_jobs.o \
	MiscUtil.o StringUtil.o TimeUtil.o DateTime.o UserLogoutType.o \
	GatewayClient.o GatewayDescriptor.o GatewayDescriptorStatus.o GatewayServer.o Descriptor.o \
	MudStatus.o SystemUtil.o CharacterUtil.o ForumUtil.o DatabaseUtil.o UserType.o ClanUtil.o \
	ClanQuestPointTransaction.o OLCUtil.o UserClan.o OlcEditType.o UserEmailEditor.o MailUtil.o \
	GatewayListener.o WebSocketDataFrame.o WebSocketClientHeaderIETF_HYBI17.o WebSocketClientHeader.o \
	HttpServer.o WebSocketClientHeaderHixie76.o sha1.o TestResource.o HttpResponse.o HttpQueue.o \
	HttpProtocol.o HttpUtil.o HttpDescriptor.o HttpRequest.o GatewayDescriptorType.o \
	src/jsoncpp/json_reader.o src/jsoncpp/json_writer.o src/jsoncpp/json_value.o

OBJFILES := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILES))


OBJFILESGATEWAY = DateTime.o GatewayClient.o GatewayDescriptor.o GatewayDescriptorStatus.o \
	GatewayMain.o GatewayServer.o kuClient.o kuDescriptor.o kuListener.o kuSockets.o MiscUtil.o \
	StringUtil.o MudStatus.o SystemUtil.o sha1.o GatewayDescriptorType.o WebSocketDataFrame.o \
	WebSocketClientHeaderIETF_HYBI17.o WebSocketClientHeader.o WebSocketClientHeaderHixie76.o \
	GatewayListener.o src/jsoncpp/json_reader.o src/jsoncpp/json_writer.o src/jsoncpp/json_value.o

OBJFILESGATEWAY := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILESGATEWAY))


$(OBJDIR)/%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< | \
	sed '\''s,\($*\)\.o[ :]*,$(OBJDIR)\/\1.o $@ : ,g'\'' > $@'

default: all

-include $(OBJFILES:.o)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -c	$<	-o $@

all:
	$(MAKE) $(BINDIR)/circle

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
