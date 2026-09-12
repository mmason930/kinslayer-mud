cmake_minimum_required(VERSION 3.16)
project(KinslayerMUD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Compiler warnings (matching Makefile)
add_compile_options(
    -O0
    -ggdb3
    -Wformat-security
    -Wformat
    -Wpointer-arith
    -Wcast-align
    -Wredundant-decls
)

add_compile_definitions(BOOST_PARAMETER_MAX_ARITY=10)

# SpiderMonkey 131
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOZJS REQUIRED mozjs-131)

# Boost
find_package(Boost REQUIRED COMPONENTS regex thread filesystem)

# MySQL
find_library(MYSQLCLIENT_LIB mysqlclient)

# OpenSSL
find_package(OpenSSL REQUIRED)

# Shared sources used by both targets
set(SHARED_SOURCES
    src/DateTime.cpp
    src/gateway/GatewayDescriptor.cpp
    src/gateway/GatewayDescriptorStatus.cpp
    src/gateway/GatewayDescriptorType.cpp
    src/gateway/GatewayListener.cpp
    src/ku/kuClient.cpp
    src/ku/kuDescriptor.cpp
    src/ku/kuListener.cpp
    src/ku/kuSockets.cpp
    src/MiscUtil.cpp
    src/StringUtil.cpp
    src/MudStatus.cpp
    src/SystemUtil.cpp
    src/sha1.cpp
    src/websocket/WebSocketDataFrame.cpp
    src/websocket/WebSocketClientHeaderIETF_HYBI17.cpp
    src/websocket/WebSocketClientHeader.cpp
    src/jsoncpp/json_reader.cpp
    src/jsoncpp/json_writer.cpp
    src/jsoncpp/json_value.cpp
)

# Gateway-only sources
set(GATEWAY_ONLY_SOURCES
    src/gateway/GatewayMain.cpp
    src/gateway/GatewayServer.cpp
)

# Kinslayer-only sources (everything not in gateway-only)
set(KINSLAYER_ONLY_SOURCES
    src/accounts.cpp
    src/act.comm.cpp
    src/act.informative.cpp
    src/act.item.cpp
    src/act.movement.cpp
    src/act.offensive.cpp
    src/act.other.cpp
    src/act.social.cpp
    src/act.wizard.cpp
    src/olc/aedit.cpp
    src/auction.cpp
    src/ban.cpp
    src/olc/cedit.cpp
    src/CharacterUtil.cpp
    src/ClanQuestPointTransaction.cpp
    src/clans.cpp
    src/ClanUtil.cpp
    src/class.cpp
    src/olc/cledit.cpp
    src/comm.cpp
    src/commands/immortals/Locate.cpp
    src/commands/items/Get.cpp
    src/commands/infrastructure/CommandUtil.cpp
    src/commands/infrastructure/Social.cpp
    src/config.cpp
    src/constants.cpp
    src/db-migrations/DatabaseMigrationStatus.cpp
    src/db-migrations/DatabaseMigrationUtil.cpp
    src/DatabaseUtil.cpp
    src/db.cpp
    src/dbutils/QueryUtil.cpp
    src/Descriptor.cpp
    src/dg_event.cpp
    src/dotmode/DotMode.cpp
    src/dotmode/DotModeArea.cpp
    src/dotmode/DotModeTarget.cpp
    src/dotmode/DotModeTargetType.cpp
    src/dotmode/DotModeUtil.cpp
    src/editor-interface/EditorInterfaceBase.cpp
    src/editor-interface/EditorInterfaceInstance.cpp
    src/editor-interface/EditorInterfaceMenu.cpp
    src/EntityType.cpp
    src/fight.cpp
    src/ForumUtil.cpp
    src/Game.cpp
    src/graph.cpp
    src/guilds/Guild.cpp
    src/guilds/GuildApplication.cpp
    src/guilds/GuildApplicationSignature.cpp
    src/guilds/GuildApplicationSignatureStatus.cpp
    src/guilds/GuildApplicationStatus.cpp
    src/guilds/GuildEditorInterface.cpp
    src/guilds/GuildJoinApplication.cpp
    src/guilds/GuildJoinApplicationStatus.cpp
    src/guilds/GuildPrivilege.cpp
    src/guilds/GuildRank.cpp
    src/guilds/GuildRankRole.cpp
    src/guilds/GuildRankStatus.cpp
    src/guilds/GuildStatus.cpp
    src/guilds/GuildUtil.cpp
    src/guilds/UserGuild.cpp
    src/guilds/UserGuildStatus.cpp
    src/handler.cpp
    src/interpreter.cpp
    src/items/ItemUtil.cpp
    src/olc/jedit.cpp
    src/js/flusspferd.cpp
    src/js/js.cpp
    src/js/js_constants.cpp
    src/js/js_functions.cpp
    src/js/js_interpreter.cpp
    src/js/js_utils.cpp
    src/js/JSCharacter.cpp
    src/js/JSCharacter_bindings.cpp
    src/js/JSObject.cpp
    src/js/JSObject_bindings.cpp
    src/js/JSQuery.cpp
    src/js/JSQuery_bindings.cpp
    src/js/JSRoom.cpp
    src/js/JSRoom_bindings.cpp
    src/js/JSRow.cpp
    src/js/JSRow_bindings.cpp
    src/kedit.cpp
    src/limits.cpp
    src/magic.cpp
    src/MailUtil.cpp
    src/md5.cpp
    src/olc/medit.cpp
    src/mobact.cpp
    src/MobLoadLogger.cpp
    src/mobs.cpp
    src/modify.cpp
    src/ObjectMoveLogger.cpp
    src/objsave.cpp
    src/olc/oedit.cpp
    src/olc/olc.cpp
    src/olc/OlcEditType.cpp
    src/openai/OpenAIClient.cpp
    src/openai/OpenAIUtil.cpp
    src/playerportal/PlayerPortalCommandProcessor.cpp
    src/playerportal/PlayerPortalDescriptor.cpp
    src/playerportal/PlayerPortalDescriptorStatus.cpp
    src/playerportal/PlayerPortalServer.cpp
    src/poker.cpp
    src/PvalManager.cpp
    src/olc/redit.cpp
    src/rooms/Exit.cpp
    src/rooms/Room.cpp
    src/rooms/RoomSector.cpp
    src/js/Script.cpp
    src/olc/sedit.cpp
    src/shop.cpp
    src/spec_assign.cpp
    src/spec_procs.cpp
    src/spell_parser.cpp
    src/spells.cpp
    src/SQLUtil.cpp
    src/statedit.cpp
    src/stats.cpp
    src/test/general/GeneralTestCase.cpp
    src/test/TestUtil.cpp
    src/TextTableBuilder.cpp
    src/threaded_jobs.cpp
    src/UserClan.cpp
    src/UserEmailEditor.cpp
    src/UserLogoutType.cpp
    src/UserType.cpp
    src/utils.cpp
    src/utils/ThreadedLogFile.cpp
    src/weather.cpp
    src/weave.cpp
    src/olc/wedit.cpp
    src/olc/zedit.cpp
    src/zones.cpp
)

# Common include/link settings shared by both targets
function(configure_target target_name)
    target_include_directories(${target_name} PRIVATE
        src
        ${MOZJS_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
    )

    target_link_directories(${target_name} PRIVATE
        ${MOZJS_LIBRARY_DIRS}
    )

    target_link_libraries(${target_name} PRIVATE
        sqlDatabase
        ${MOZJS_LIBRARIES}
        ${MYSQLCLIENT_LIB}
        Boost::regex
        Boost::thread
        Boost::filesystem
        OpenSSL::SSL
        OpenSSL::Crypto
        crypt
        pthread
        dl
    )
endfunction()

# kinslayer executable
add_executable(kinslayer ${KINSLAYER_ONLY_SOURCES} ${SHARED_SOURCES})
configure_target(kinslayer)

# gateway executable
add_executable(gateway ${GATEWAY_ONLY_SOURCES} ${SHARED_SOURCES})
configure_target(gateway)
