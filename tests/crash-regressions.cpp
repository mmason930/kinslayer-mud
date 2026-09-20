#include "utils/CommandInput.h"
#include "utils/JsonCommand.h"
#include "items/SavedObjectValidation.h"
#include "ku/kuListener.h"
#include "ku/kuClient.h"

#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/wait.h>

static void inputAndJson()
{
    bool truncated;
    assert(prepareLegacyCommand("say $n", true, 511, truncated) == "say $$n");
    assert(!truncated);
    assert(prepareLegacyCommand(std::string(511, 'x'), true, 511, truncated).size() == 511);
    assert(!truncated);
    auto dollars = prepareLegacyCommand(std::string(300, '$'), true, 511, truncated);
    assert(truncated && dollars.size() == 510);
    assert(prepareLegacyCommand("password$", false, 511, truncated) == "password$");
    assert(!truncated);
    assert(prepareLegacyCommand("x", true, 0, truncated).empty() && truncated);
    Json::Value command;
    for (const char *invalid : {"[]", "42", "null", "{}", "{\"method\":42}", "{"})
        assert(!readJsonCommand(invalid, command));
    assert(readJsonCommand("{\"method\":\"Input\",\"data\":\"look\"}", command));
    assert(command["data"].asString() == "look");
}

static void savedObjects()
{
    assert(parseSavedObjectId("5cff3747-03c2-4002-ae00-000000000001"));
    assert(!parseSavedObjectId("[92, 255, 55, 71, 3, 194, 64, 2, 174"));
    assert(!parseSavedObjectId(""));
    SavedObjectQuarantine quarantine;
    const std::map<std::string, std::vector<std::string>> children{
        {"bad", {"child"}}, {"child", {"grandchild"}}, {"grandchild", {"bad"}}
    };
    quarantine.add("bad", [&](const std::string &id) { return children.at(id); });
    assert(quarantine.ids == std::set<std::string>({"bad", "child", "grandchild"}));
    assert(!quarantine.ids.contains("unrelated"));
    try
    {
        quarantine.add("failed", [](const std::string &) -> std::vector<std::string> {
            throw std::runtime_error("database unavailable");
        });
        assert(false);
    }
    catch (const std::runtime_error &) {}
    assert(!quarantine.ids.contains("failed"));
}

static void socketWrites()
{
    int peers[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, peers) == 0);
    kuDescriptor descriptor(nullptr);
    descriptor.sock = peers[0];
    close(peers[1]);
    // Run with the default SIGPIPE disposition, as a standalone gateway does.
    signal(SIGPIPE, SIG_DFL);
    descriptor.socketWriteInstant("closed peer");
    assert(descriptor.socketIsClosed());

    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, peers) == 0);
    kuDescriptor slow(nullptr);
    slow.sock = peers[0];
    slow.nonBlock();
    int size = 4096;
    assert(setsockopt(slow.sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0);
    const std::string large(256 * 1024, 'x');
    slow.socketWriteInstant(large);
    assert(!slow.socketIsClosed());
    assert(!slow.output.empty()); // Unsent bytes survive nonblocking backpressure.
    std::string received;
    char buffer[8192];
    while (received.size() < large.size())
    {
        const auto bytes = recv(peers[1], buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytes > 0) received.append(buffer, bytes);
        slow.socketWriteInstant("");
    }
    assert(received == large && slow.output.empty());
    close(peers[1]);
}

static void descriptorLimits()
{
    kuListener listener(0, TCP);
    assert(listener.isListening());
    std::vector<int> files;
    while (files.empty() || files.back() < FD_SETSIZE)
    {
        const int fd = open("/dev/null", O_RDONLY);
        assert(fd >= 0);
        files.push_back(fd);
    }
    kuListener high(0, TCP);
    assert(!high.isListening() && !high.canAccept());
    assert(high.acceptNewHosts().empty());
    high.pulse(); // Must also work if the listener never opened successfully.

    sockaddr_in address{};
    socklen_t length = sizeof(address);
    assert(getsockname(listener.getSocket(), reinterpret_cast<sockaddr *>(&address), &length) == 0);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    const int peer = socket(AF_INET, SOCK_STREAM, 0);
    assert(peer >= FD_SETSIZE);
    assert(connect(peer, reinterpret_cast<sockaddr *>(&address), length) == 0);
    assert(listener.acceptNewHosts().empty()); // Reject an oversized accepted fd.
    listener.pulse();
    close(peer);

    kuClient client;
    assert(!client.connect("127.0.0.1", ntohs(address.sin_port)));
    for (int file : files) close(file);
    assert(listener.isListening());
}

int main()
{
    inputAndJson();
    savedObjects();
    socketWrites();
    descriptorLimits();
    std::cout << "PASS: command bounds, JSON validation, UUID quarantine, broken/slow sockets, descriptor limits\n";
}
