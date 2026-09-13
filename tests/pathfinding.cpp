#include "utils/Pathfinding.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <sstream>
#include <unordered_map>
#include <vector>

constexpr int NUM_OF_DIRS = 6, ROOM_NOTRACK = 6, ROOM_BFS_MARK = 15;
constexpr int BFS_ERROR = -1, BFS_ALREADY_THERE = -2, BFS_NO_PATH = -3;
struct Room;
struct Exit {
    Room *to;
    bool closed = false, hidden = false, disabled = false;
    Room *getToRoom() { return to; }
};
struct Room {
    int vnum;
    Exit *dir_option[6]{};
    int room_flags = 0;
    std::uint64_t pathfindingGeneration = 0;
    int getVnum() const { return vnum; }
    std::list<int> pathToRoom(Room *);
    int findFirstStep(Room *);
    int getDistanceToRoom(Room *);
    RoomRoute routeToRoom(Room *);
};
std::vector<Room *> World;
#define ROOM_FLAGGED(room, flag) ((room)->room_flags & (1 << (flag)))
void Log(const char *, ...) {}

// All production search methods, queue management, and counters, unchanged.
#include "pathfinding-under-test.inc"

// Independent reference: a FIFO queue with a separate visited map. This favors
// clarity over speed and never uses production stamps or scratch storage.
static std::pair<RoomRoute, std::list<int>> reference(Room *source, Room *target, bool noTrack)
{
    if (!source || !target) return {{BFS_ERROR, -1}, {}};
    if (source == target) return {{BFS_ALREADY_THERE, 0}, {}};
    std::unordered_map<Room *, std::pair<Room *, int>> parents;
    std::deque<Room *> queue{source};
    parents[source] = {nullptr, -1};
    while (!queue.empty())
    {
        Room *room = queue.front(); queue.pop_front();
        if (room == target)
        {
            std::list<int> path;
            for (; room != source; room = parents.at(room).first)
                path.push_front(parents.at(room).second);
            return {{path.front(), static_cast<int>(path.size())}, path};
        }
        for (int d = 0; d < 6; ++d)
        {
            auto *exit = room->dir_option[d];
            if (!exit || !exit->to || parents.count(exit->to) ||
                (noTrack && ROOM_FLAGGED(exit->to, ROOM_NOTRACK))) continue;
            parents[exit->to] = {room, d};
            queue.push_back(exit->to);
        }
    }
    return {{BFS_NO_PATH, -1}, {}};
}

static void compare(Room *a, Room *b)
{
    const auto expected = reference(a, b, false);
    assert(a->pathToRoom(b) == expected.second);
    assert(a->findFirstStep(b) == expected.first.firstStep);
    assert(a->getDistanceToRoom(b) == expected.first.distance);
    const auto route = a->routeToRoom(b);
    assert(route.firstStep == expected.first.firstStep && route.distance == expected.first.distance);
    assert(find_first_step(a, b) == reference(a, b, true).first.firstStep);
}

static void randomGraphs()
{
    std::mt19937 rng(20260913);
    for (int trial = 0; trial < 40; ++trial)
    {
        std::vector<Room> rooms(24);
        std::vector<std::unique_ptr<Exit>> exits;
        World.clear();
        std::vector<int> flags;
        for (int i = 0; i < 24; ++i)
        {
            rooms[i].vnum = i;
            rooms[i].room_flags = (rng() % 3 ? 0 : 1 << ROOM_NOTRACK) | (1 << ROOM_BFS_MARK);
            flags.push_back(rooms[i].room_flags);
            World.push_back(&rooms[i]);
        }
        for (auto &room : rooms)
            for (int d = 0; d < 6; ++d)
                if (rng() % 4 == 0)
                {
                    auto exit = std::make_unique<Exit>();
                    exit->to = rng() % 8 ? &rooms[rng() % rooms.size()] : nullptr;
                    exit->closed = exit->hidden = exit->disabled = true;
                    room.dir_option[d] = exit.get();
                    exits.push_back(std::move(exit));
                }
        for (auto &a : rooms) for (auto &b : rooms) compare(&a, &b);
        for (int i = 0; i < 24; ++i) assert(rooms[i].room_flags == flags[i]);
    }
    World.clear();
}

static void liveChangesAndMetrics()
{
    Room a{1}, b{2}, c{3}, disconnected{4};
    World = {&a, &b, &c, &disconnected};
    Exit ab{&b}, bc{&c}, ac{&c};
    a.dir_option[0] = &ab; b.dir_option[1] = &bc;
    compare(&a, &c);
    assert(a.routeToRoom(&c).distance == 2);
    a.dir_option[5] = &ac; // OLC adds a shorter route
    assert(a.routeToRoom(&c).firstStep == 5);
    a.dir_option[5] = nullptr; bc.to = nullptr; // OLC removes it and a destination
    compare(&a, &c);
    bc.to = &c;
    b.room_flags = 1 << ROOM_NOTRACK;
    assert(find_first_step(&a, &c) == BFS_NO_PATH);
    assert(a.findFirstStep(&c) == 0);
    b.room_flags = 0;
    pathGeneration = std::numeric_limits<std::uint64_t>::max();
    compare(&a, &c); // generation rollover
    compare(&a, &a);
    compare(&a, nullptr);
    assert(find_first_step(nullptr, &a) == BFS_ERROR);

    const auto api = static_cast<unsigned>(PathfindingApi::Route);
    const auto before = getPathfindingStats()[api];
    a.routeToRoom(&b);
    const auto after = getPathfindingStats()[api];
    assert(after.calls == before.calls + 1);
    assert(after.roomsVisited == before.roomsVisited + 2);
    assert(after.totalNanoseconds >= before.totalNanoseconds);
    assert(describePathfindingStats().find("routeTo") != std::string::npos);

    // An adjacent lookup must not touch flags across a large unrelated world.
    std::vector<Room> unrelated(12000);
    for (auto &room : unrelated) { room.room_flags = 1 << ROOM_BFS_MARK; World.push_back(&room); }
    compare(&a, &b);
    for (const auto &room : unrelated) assert(room.room_flags == (1 << ROOM_BFS_MARK));
    World.clear();
}

int main()
{
    randomGraphs();
    liveChangesAndMetrics();
    std::cout << "PASS: all path APIs, random directed graphs, tie ordering, loops, unreachable rooms, NOTRACK policy, live exit changes, rollover and metrics\n";
}
