#pragma once

#include "../ku/kuSockets.h"
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace GatewaySecurity {
inline std::optional<std::string> normalizeAddress(const std::string &input)
{
    if (input.empty() || input.size() >= INET6_ADDRSTRLEN || input.find('\0') != std::string::npos)
        return std::nullopt;
    std::array<unsigned char, 16> address{};
    char output[INET6_ADDRSTRLEN];
    for (int family : {AF_INET, AF_INET6})
        if (inet_pton(family, input.c_str(), address.data()) == 1 &&
            inet_ntop(family, address.data(), output, sizeof(output)))
            return std::string(output);
    return std::nullopt;
}

inline bool trustedProxy(const std::string &peer)
{
    const char *hostname = std::getenv("KINSLAYER_TRUSTED_PROXY");
    if (!hostname || !*hostname) return false; // No implicit trust of private networks.
    const auto normalized = normalizeAddress(peer);
    if (!normalized) return false;
    addrinfo hints{}, *addresses = nullptr;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(hostname, nullptr, &hints, &addresses) != 0) return false;
    bool trusted = false;
    for (auto p = addresses; p; p = p->ai_next) {
        char text[INET6_ADDRSTRLEN];
        const void *address = p->ai_family == AF_INET
            ? static_cast<const void *>(&reinterpret_cast<sockaddr_in *>(p->ai_addr)->sin_addr)
            : p->ai_family == AF_INET6
                ? static_cast<const void *>(&reinterpret_cast<sockaddr_in6 *>(p->ai_addr)->sin6_addr) : nullptr;
        if (address && inet_ntop(p->ai_family, address, text, sizeof(text)) && *normalized == text)
            trusted = true;
    }
    freeaddrinfo(addresses);
    return trusted;
}

inline std::string randomToken()
{
    std::array<unsigned char, 32> bytes;
    if (RAND_bytes(bytes.data(), bytes.size()) != 1)
        throw std::runtime_error("Unable to generate gateway credential");
    const char hex[] = "0123456789abcdef";
    std::string token;
    for (auto b : bytes) { token += hex[b >> 4]; token += hex[b & 15]; }
    return token;
}

inline const std::string &secret()
{
    static const std::string value = [] {
        const char *configured = std::getenv("KINSLAYER_GATEWAY_SECRET_FILE");
        std::ifstream file(configured ? configured : "/run/kinslayer/gateway.key");
        std::string token, extra;
        if (!(file >> token) || (file >> extra) || token.size() != 64 ||
            token.find_first_not_of("0123456789abcdef") != std::string::npos)
            throw std::runtime_error("Missing or invalid private gateway credential file");
        return token;
    }();
    return value;
}

inline bool validAuthentication(const std::vector<std::string> &args)
{
    const auto &expected = secret();
    return args.size() == 2 && args[0] == "Validate" && args[1].size() == expected.size() &&
        CRYPTO_memcmp(args[1].data(), expected.data(), expected.size()) == 0;
}
}
