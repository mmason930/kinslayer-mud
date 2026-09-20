#pragma once
#include <map>
#include <string>
#include <ctime>
#include <openssl/rand.h>
// Process-local credentials: expire at restart, password change or after twelve hours.
// Accessed only on the game thread. No plaintext passwords are retained.
namespace ClientSession {
struct Entry { std::string user, passwordHash; time_t expires; };
inline std::map<std::string, Entry> entries;
inline void prune() {
    for (auto it = entries.begin(); it != entries.end();)
        if (it->second.expires <= time(nullptr)) it = entries.erase(it); else ++it;
}
inline void revoke(const std::string &user) {
    for (auto it = entries.begin(); it != entries.end();)
        if (it->second.user == user) it = entries.erase(it); else ++it;
}
inline std::string issue(const std::string &user, const std::string &hash) {
    prune();
    unsigned char bytes[32];
    if (RAND_bytes(bytes, sizeof(bytes)) != 1) return "";
    revoke(user);
    static const char hex[] = "0123456789abcdef";
    std::string token;
    for (auto byte : bytes) { token += hex[byte >> 4]; token += hex[byte & 15]; }
    entries[token] = {user, hash, time(nullptr) + 12 * 60 * 60};
    return token;
}
inline bool accepts(const std::string &token, const std::string &user, const std::string &hash) {
    prune();
    auto it = entries.find(token);
    return it != entries.end() && it->second.user == user && it->second.passwordHash == hash;
}
}
