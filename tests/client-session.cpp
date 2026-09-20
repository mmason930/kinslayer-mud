#include "utils/ClientSession.h"
#include <cassert>
#include <iostream>
int main() {
    auto first = ClientSession::issue("Tester", "hash");
    assert(first.size() == 64);
    assert(ClientSession::accepts(first, "Tester", "hash"));
    assert(!ClientSession::accepts(first, "Other", "hash"));
    assert(!ClientSession::accepts(first, "Tester", "changed-password"));
    auto second = ClientSession::issue("Tester", "hash");
    assert(second != first);
    assert(!ClientSession::accepts(first, "Tester", "hash"));
    assert(ClientSession::accepts(second, "Tester", "hash"));
    ClientSession::entries[second].expires = time(nullptr) - 1;
    assert(!ClientSession::accepts(second, "Tester", "hash"));
    auto third = ClientSession::issue("Tester", "hash");
    ClientSession::revoke("Tester");
    assert(!ClientSession::accepts(third, "Tester", "hash"));
    std::cout << "Client sessions: random tokens, identity/password binding, rotation, expiry and logout passed.\n";
}
