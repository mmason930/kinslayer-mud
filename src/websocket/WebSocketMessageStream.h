#pragma once
#include "WebSocketDataFrame.h"
#include "WebSocketException.h"
#include "../ku/kuDescriptor.h"
#include <memory>
#include <optional>

// Server-side text messages, including fragmented messages and control frames.
class WebSocketMessageStream {
    std::string fragments;
    bool fragmented = false;
public:
    static bool validUtf8(const std::string &text) {
        std::size_t i = 0;
        while (i < text.size()) {
            unsigned char first = text[i++];
            if (first < 0x80) continue;
            unsigned length; uint32_t value, minimum;
            if (first >= 0xc2 && first <= 0xdf) { length = 1; value = first & 0x1f; minimum = 0x80; }
            else if (first >= 0xe0 && first <= 0xef) { length = 2; value = first & 0x0f; minimum = 0x800; }
            else if (first >= 0xf0 && first <= 0xf4) { length = 3; value = first & 7; minimum = 0x10000; }
            else return false;
            if (text.size() - i < length) return false;
            while (length--) {
                const unsigned char next = text[i++];
                if ((next & 0xc0) != 0x80) return false;
                value = (value << 6) | (next & 0x3f);
            }
            if (value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) return false;
        }
        return true;
    }

    std::optional<std::string> accept(const WebSocketDataFrame &frame) {
        const auto opcode = frame.getOpCode();
        if (opcode >= 8) return std::nullopt; // Control frames handled by read().
        if (opcode == 2 || (opcode == 0 && !fragmented) || (opcode == 1 && fragmented))
            throw WebSocketException("Invalid text message sequence");
        const auto payload = frame.getPayloadData();
        if (payload.size() > WebSocketDataFrame::maxPayloadSize - fragments.size())
            throw WebSocketException("WebSocket message too large");
        fragments += payload;
        if (!frame.getFin()) { fragmented = true; return std::nullopt; }
        if (!validUtf8(fragments)) throw WebSocketException("Invalid UTF-8 message");
        fragmented = false;
        std::string message;
        message.swap(fragments);
        return message;
    }

    std::optional<std::string> read(kuDescriptor &socket) {
        // A stream of control frames must not monopolize a gateway/game tick.
        for (int count = 0; count < 32; ++count) {
            unsigned int consumed = 0;
            std::unique_ptr<WebSocketDataFrame> frame(WebSocketDataFrame::parse(socket.getInputBuffer(), consumed));
            if (!frame) return std::nullopt;
            socket.eraseInput(0, consumed);
            const auto opcode = frame->getOpCode();
            if (opcode == 8 || opcode == 9) {
                const auto payload = frame->getPayloadData();
                if (opcode == 8 && payload.size() >= 2) {
                    const unsigned code = (static_cast<unsigned char>(payload[0]) << 8) | static_cast<unsigned char>(payload[1]);
                    if (code < 1000 || code >= 5000 || code == 1004 || code == 1005 || code == 1006 ||
                        (code >= 1015 && code < 3000) || !validUtf8(payload.substr(2)))
                        throw WebSocketException("Invalid WebSocket close status");
                }
                WebSocketDataFrame response;
                response.setFin(true);
                response.setOpCode(opcode == 9 ? 10 : 8);
                response.setPayloadData(payload);
                socket.socketWriteInstant(response.prepareNetworkPacket());
                if (opcode == 8) throw WebSocketException("Socket Closed");
            }
            else if (auto message = accept(*frame)) return message;
        }
        return std::nullopt;
    }
};
