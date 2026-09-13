#include "playerportal/PlayerPortalServer.h"

#include <cassert>
#include <iostream>
#include <vector>

// Command execution needs the game/DB/JS runtime. These unused processors are
// stubbed; the portal and socket lifecycle code is compiled from production.
PlayerPortalCommandProcessor::PlayerPortalCommandProcessor() = default;
PlayerPortalCommandProcessor::~PlayerPortalCommandProcessor() = default;
PlayerPortalJavaScriptCommandProcessor::PlayerPortalJavaScriptCommandProcessor() = default;
PlayerPortalJavaScriptCommandProcessor::~PlayerPortalJavaScriptCommandProcessor() = default;
void PlayerPortalJavaScriptCommandProcessor::process(PlayerPortalDescriptor *, Json::Value) {}
PlayerPortalLoadItemFlagsCommandProcess::PlayerPortalLoadItemFlagsCommandProcess() = default;
PlayerPortalLoadItemFlagsCommandProcess::~PlayerPortalLoadItemFlagsCommandProcess() = default;
void PlayerPortalLoadItemFlagsCommandProcess::process(PlayerPortalDescriptor *, Json::Value) {}
PlayerPortalLoadObjectListCommandProcess::PlayerPortalLoadObjectListCommandProcess() = default;
PlayerPortalLoadObjectListCommandProcess::~PlayerPortalLoadObjectListCommandProcess() = default;
void PlayerPortalLoadObjectListCommandProcess::process(PlayerPortalDescriptor *, Json::Value) {}

class TestPortal : public PlayerPortalServer
{
public:
    explicit TestPortal(kuListener *listener) : PlayerPortalServer(listener) {}

    kuDescriptor *acceptConnection()
    {
        auto accepted = listener->acceptNewHosts();
        assert(accepted.size() == 1);
        auto *socket = accepted.front();
        auto *portal = new PlayerPortalDescriptor(socket, this);
        descriptors.push_back(portal);
        kuDescriptorUidToPlayerPortalDescriptorMap[socket->getUid()] = portal;
        return socket;
    }

    void addProcessor(PlayerPortalCommandProcessor *processor)
    {
        commandProcessors["Test cleanup"] = processor;
    }
};

class CleanupProcessor : public PlayerPortalCommandProcessor
{
    bool &destroyed;
public:
    explicit CleanupProcessor(bool &destroyed) : destroyed(destroyed) {}
    ~CleanupProcessor() { destroyed = true; }
    void process(PlayerPortalDescriptor *, Json::Value) override {}
};

static int connectPeer(kuListener *listener)
{
    sockaddr_in address{};
    socklen_t size = sizeof(address);
    assert(getsockname(listener->getSocket(), reinterpret_cast<sockaddr *>(&address), &size) == 0);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int peer = socket(AF_INET, SOCK_STREAM, 0);
    assert(peer >= 0);
    assert(connect(peer, reinterpret_cast<sockaddr *>(&address), size) == 0);
    return peer;
}

static void shutdownWithConnections(int count)
{
    auto *listener = new kuListener(0, TCP);
    assert(listener->isListening());
    auto *server = new TestPortal(listener);
    std::vector<int> peers;
    for (int i = 0; i < count; ++i)
    {
        peers.push_back(connectPeer(listener));
        server->acceptConnection();
    }
    assert(server->numberOfDescriptors() == static_cast<unsigned int>(count));
    delete server;
    for (int peer : peers)
    {
        char byte;
        assert(recv(peer, &byte, 1, MSG_DONTWAIT) == 0);
        close(peer);
    }
}

static void disconnectBeforeShutdown()
{
    auto *listener = new kuListener(0, TCP);
    auto *server = new TestPortal(listener);
    int first = connectPeer(listener);
    server->acceptConnection();
    int second = connectPeer(listener);
    auto *secondSocket = server->acceptConnection();

    // A peer disconnect is reaped by pulse, which must delete both objects once.
    close(first);
    listener->pulse();
    assert(server->numberOfDescriptors() == 1);
    assert(listener->getDescriptors().size() == 1);

    // Repeated callbacks must not insert null entries or delete the wrapper twice.
    server->onDescriptorClose(secondSocket);
    server->onDescriptorClose(secondSocket);
    assert(server->numberOfDescriptors() == 0);
    listener->pulse();
    assert(listener->getDescriptors().empty());
    close(second);

    // Closing connections must leave the server able to accept a new one.
    int third = connectPeer(listener);
    server->acceptConnection();
    assert(server->numberOfDescriptors() == 1);
    delete server;
    close(third);
}

int main()
{
    shutdownWithConnections(0);
    shutdownWithConnections(1);
    shutdownWithConnections(19);
    disconnectBeforeShutdown();

    bool destroyed = false;
    auto *server = new TestPortal(new kuListener(0, TCP));
    server->addProcessor(new CleanupProcessor(destroyed));
    delete server;
    assert(destroyed);

    PlayerPortalDescriptorStatus::cleanup();
    std::cout << "PASS: portal shutdown with 0/1/19 connections; disconnects, repeated callbacks, reconnects, and processor cleanup\n";
}
