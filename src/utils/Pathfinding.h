#ifndef KINSLAYER_PATHFINDING_H
#define KINSLAYER_PATHFINDING_H

#include <array>
#include <cstdint>
#include <list>
#include <string>

struct RoomRoute
{
    int firstStep;
    int distance;
};

enum class PathfindingApi { FirstStep, Distance, FullPath, Hunt, Route, Count };

struct PathfindingStats
{
    std::uint64_t calls = 0;
    std::uint64_t roomsVisited = 0;
    std::uint64_t maxRoomsVisited = 0;
    std::uint64_t totalNanoseconds = 0;
    std::uint64_t maxNanoseconds = 0;
    int slowestSource = -1;
    int slowestTarget = -1;
};

// Main-game-thread counters, cumulative since process startup.
const std::array<PathfindingStats, static_cast<unsigned>(PathfindingApi::Count)> &getPathfindingStats();
std::string describePathfindingStats();

#endif
