#pragma once

#include <boost/uuid/string_generator.hpp>
#include <optional>
#include <set>
#include <string>
#include <vector>

inline std::optional<boost::uuids::uuid> parseSavedObjectId(const std::string &id)
{
    try
    {
        return boost::uuids::string_generator()(id);
    }
    catch (const std::runtime_error &)
    {
        return std::nullopt;
    }
}

// Keep malformed records and their descendants out of memory and out of save
// deletions. The original database rows remain available for manual recovery.
class SavedObjectQuarantine
{
public:
    std::set<std::string> ids;

    template<class LoadChildren>
    void add(const std::string &id, LoadChildren loadChildren)
    {
        auto discovered = ids;
        std::vector<std::string> pending{id};
        while (!pending.empty())
        {
            auto current = std::move(pending.back());
            pending.pop_back();
            if (!discovered.insert(current).second)
                continue;
            for (const auto &child : loadChildren(current))
                pending.push_back(child);
        }
        ids.swap(discovered);
    }
};
