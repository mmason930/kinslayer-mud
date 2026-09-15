#include "../src/MiscUtil.h"
#include <boost/uuid/string_generator.hpp>
#include <cassert>

int main()
{
    const std::string canonical = "5cff3747-03c2-4002-ae11-123456789abc";
    const auto id = boost::uuids::string_generator{}(canonical);
    // UUIDs can be formattable as ranges on C++23 toolchains. Persistence
    // must still round-trip through Boost's UUID parser.
    const auto saved = MiscUtil::toString(id);
    assert(saved == canonical);
    assert(saved.size() == 36);
    assert(boost::uuids::string_generator{}(saved) == id);
    assert(MiscUtil::toString(boost::uuids::uuid{}) == "00000000-0000-0000-0000-000000000000");
    assert(MiscUtil::toString(42) == "42");
    assert(MiscUtil::toString(std::string("unchanged")) == "unchanged");
}
