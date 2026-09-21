#include "utils/GatewaySecurity.h"
#include "websocket/WebSocketMessageStream.h"
#include "ku/kuClient.h"
#include "ku/kuListener.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

static std::string packet(std::string payload, unsigned char opcode = 1, bool fin = true)
{
    WebSocketDataFrame frame;
    frame.setFin(fin); frame.setOpCode(opcode); frame.setIsMasked(true);
    frame.setMaskCode({0x37, 0xfa, 0x21, 0x3d}); frame.setPayloadData(payload);
    return frame.prepareNetworkPacket();
}

static void framing()
{
    for (std::size_t size : {0, 125, 126, 65535, 65536, 65537, 1048576}) {
        std::string payload(size, 'x');
        if (size) payload[size / 2] = '\0';
        const auto wire = packet(payload);
        assert(static_cast<unsigned char>(wire[0]) == 0x81);
        assert((static_cast<unsigned char>(wire[1]) & 127) == (size < 126 ? size : size <= 65535 ? 126 : 127));
        if (size == 65536) assert(wire.substr(2, 8) == std::string("\0\0\0\0\0\1\0\0", 8));
        unsigned int consumed = 99;
        if (wire.size() > 2) {
            std::unique_ptr<WebSocketDataFrame> incomplete(WebSocketDataFrame::parse(wire.substr(0, wire.size() - 1), consumed));
            assert(!incomplete && consumed == 0);
        }
        std::unique_ptr<WebSocketDataFrame> frame(WebSocketDataFrame::parse(wire + "trailing", consumed));
        assert(frame && consumed == wire.size() && frame->getPayloadData() == payload && frame->getFin() && frame->getIsMasked());
    }
    for (auto invalid : {std::string("\x81\x00", 2), std::string("\xc1\x80", 2),
            std::string("\x09\x80", 2), std::string("\x89\xfe", 2), std::string("\x83\x80", 2),
            std::string("\x81\xff\x80\0\0\0\0\0\0\0", 10),
            std::string("\x81\xff\0\0\0\0\0\x10\0\1", 10),
            std::string("\x81\xfe\0\x7d", 4)}) {
        unsigned int consumed;
        try { std::unique_ptr<WebSocketDataFrame> frame(WebSocketDataFrame::parse(invalid, consumed)); assert(false); }
        catch (const WebSocketException &) {}
    }
    WebSocketMessageStream stream;
    auto accept = [&](std::string wire) {
        unsigned int consumed;
        std::unique_ptr<WebSocketDataFrame> frame(WebSocketDataFrame::parse(wire, consumed));
        return stream.accept(*frame);
    };
    assert(!accept(packet("hel", 1, false)));
    assert(!accept(packet("ping", 9)));
    assert(accept(packet("lo", 0)) == "hello");
    try { accept(packet("bad continuation", 0)); assert(false); } catch (const WebSocketException &) {}
    assert(WebSocketMessageStream::validUtf8("\xf0\x9f\x98\x80"));
    assert(!WebSocketMessageStream::validUtf8("\xed\xa0\x80"));
    assert(!WebSocketMessageStream::validUtf8("\xc0\x80"));
    int peers[2]; assert(socketpair(AF_UNIX, SOCK_STREAM, 0, peers) == 0);
    kuDescriptor socket(nullptr); socket.sock = peers[0]; socket.nonBlock();
    socket.input = packet("hello", 9) + packet("look\6");
    assert(stream.read(socket) == "look\6");
    char reply[32]; const auto length = recv(peers[1], reply, sizeof(reply), 0);
    assert(std::string(reply, length) == std::string("\x8a\x05hello", 7));
    close(peers[1]);
}

static void transport()
{
    signal(SIGPIPE, SIG_DFL);
    int peers[2]; assert(socketpair(AF_UNIX, SOCK_STREAM, 0, peers) == 0);
    kuClient client; client.sock = peers[0]; client.connected = true;
    assert(fcntl(client.sock, F_SETFL, O_NONBLOCK) == 0);
    int size = 4096; assert(setsockopt(client.sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0);
    std::string sent(512 * 1024, 'a'); sent[33] = '\0';
    assert(client.send(sent) && client.pendingOutputSize() > 0);
    assert(client.send("second message")); sent += "second message";
    std::string received;
    for (int i = 0; received.size() < sent.size() && i < 10000; ++i) {
        char buffer[8192]; auto n = recv(peers[1], buffer, sizeof(buffer), MSG_DONTWAIT);
        if (n > 0) received.append(buffer, n);
        std::stringstream ignored; assert(client.receive(ignored) >= 0);
    }
    assert(sent == received && client.pendingOutputSize() == 0);
    assert(::send(peers[1], "a\0b", 3, 0) == 3);
    std::stringstream binary; assert(client.receive(binary) == 3 && binary.str() == std::string("a\0b", 3));
    close(peers[1]); assert(!client.send("closed") && !client.isConnected());

    kuListener listener(0, TCP, true);
    sockaddr_in address{}; socklen_t count = sizeof(address);
    assert(getsockname(listener.getSocket(), reinterpret_cast<sockaddr *>(&address), &count) == 0);
    assert(address.sin_addr.s_addr == htonl(INADDR_LOOPBACK));
    assert(client.connect("127.0.0.1", ntohs(address.sin_port)));
    assert(fcntl(client.sock, F_GETFL) & O_NONBLOCK);
    assert(!client.send(std::string(4 * 1024 * 1024 + 1, 'x')) && !client.isConnected());
}

static void addressesAndAuthentication()
{
    using namespace GatewaySecurity;
    assert(normalizeAddress("2001:0db8:ffff:ffff:ffff:ffff:ffff:ffff") == "2001:db8:ffff:ffff:ffff:ffff:ffff:ffff");
    assert(normalizeAddress("127.0.0.1") == "127.0.0.1");
    for (auto invalid : {"", "example.com", "1.2.3.4, 5.6.7.8", "1.2.3.4\n", "999.1.1.1"}) assert(!normalizeAddress(invalid));
    assert(!normalizeAddress(std::string(10000, 'a')));
    unsetenv("KINSLAYER_TRUSTED_PROXY"); assert(!trustedProxy("127.0.0.1"));
    setenv("KINSLAYER_TRUSTED_PROXY", "127.0.0.1", 1);
    assert(trustedProxy("127.0.0.1") && !trustedProxy("127.0.0.2"));
    const auto token = randomToken(); assert(token.size() == 64 && randomToken() != token);
    const auto path = std::string("/tmp/kinslayer-test-key-") + std::to_string(getpid());
    { std::ofstream key(path); key << token; }
    setenv("KINSLAYER_GATEWAY_SECRET_FILE", path.c_str(), 1);
    assert(!validAuthentication({}) && !validAuthentication({"Validate"}));
    assert(!validAuthentication({"Validate", std::string(64, '0')}));
    assert(!validAuthentication({"Validate", token, "extra"}));
    assert(validAuthentication({"Validate", token}));
    unlink(path.c_str());
}

int main() {
    framing(); transport(); addressesAndAuthentication();
    std::cout << "PASS: frame boundaries, fragmentation, controls, UTF-8, queued/binary writes, loopback, IP trust, authentication\n";
}
