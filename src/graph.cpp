/* ************************************************************************
*   File: graph.c                                       Part of CircleMUD *
*  Usage: various graph algorithms                                        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"


#include "utils.h"
#include "interpreter.h"
#include "db.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"
#include "utils/Pathfinding.h"
#include <chrono>
#include <iomanip>
#include <sstream>


/* Externals */
extern Character *character_list;
extern const char *dirs[];


// Pathfinding runs synchronously on the game thread. No callbacks or persistent
// route cache: each search reads the current exits and survives OLC changes.
namespace {
struct PathVisit
{
    Room *room;
    int firstStep;
    int depth;
    std::size_t parent;
    int direction;
};
std::vector<PathVisit> pathVisits;
std::uint64_t pathGeneration = 0;
std::array<PathfindingStats, static_cast<unsigned>(PathfindingApi::Count)> pathStats;

struct SearchMeasurement
{
    PathfindingStats &stats;
    Room *source;
    Room *target;
    std::uint64_t visited = 0;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    ~SearchMeasurement()
    {
        const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - start).count();
        ++stats.calls;
        stats.roomsVisited += visited;
        stats.maxRoomsVisited = std::max(stats.maxRoomsVisited, visited);
        stats.totalNanoseconds += elapsed;
        if (static_cast<std::uint64_t>(elapsed) >= stats.maxNanoseconds)
        {
            stats.maxNanoseconds = elapsed;
            stats.slowestSource = source ? source->getVnum() : -1;
            stats.slowestTarget = target ? target->getVnum() : -1;
        }
    }
};

RoomRoute searchRooms(Room *source, Room *target, bool respectNoTrack,
    PathfindingApi api, std::list<int> *path = nullptr)
{
    SearchMeasurement measurement{pathStats[static_cast<unsigned>(api)], source, target};
    if (!source || !target)
        return {BFS_ERROR, -1};
    if (source == target)
        return {BFS_ALREADY_THERE, 0};

    // The only full-world reset is at 64-bit generation rollover.
    if (++pathGeneration == 0)
    {
        for (Room *room : World)
            room->pathfindingGeneration = 0;
        ++pathGeneration;
    }
    pathVisits.clear();
    if (pathVisits.capacity() < World.size())
        pathVisits.reserve(World.size());
    source->pathfindingGeneration = pathGeneration;
    pathVisits.push_back({source, BFS_ERROR, 0, 0, -1});

    for (std::size_t head = 0; head < pathVisits.size(); ++head)
    {
        // Copy: discovering a room may grow the vector and invalidate references.
        const PathVisit current = pathVisits[head];
        ++measurement.visited;
        if (current.room == target)
        {
            if (path)
                for (std::size_t node = head; node != 0; node = pathVisits[node].parent)
                    path->push_front(pathVisits[node].direction);
            return {current.firstStep, current.depth};
        }
        for (int direction = 0; direction < NUM_OF_DIRS; ++direction)
        {
            Exit *exit = current.room->dir_option[direction];
            Room *neighbor = exit ? exit->getToRoom() : nullptr;
            // Preserve legacy policy: only NPC hunting excludes ROOM_NOTRACK.
            // Existing path APIs traverse closed/hidden/disabled exits too.
            if (!neighbor || neighbor->pathfindingGeneration == pathGeneration ||
                (respectNoTrack && ROOM_FLAGGED(neighbor, ROOM_NOTRACK)))
                continue;
            neighbor->pathfindingGeneration = pathGeneration;
            pathVisits.push_back({neighbor, head == 0 ? direction : current.firstStep,
                current.depth + 1, head, direction});
        }
    }
    return {BFS_NO_PATH, -1};
}
} // namespace

const std::array<PathfindingStats, static_cast<unsigned>(PathfindingApi::Count)> &getPathfindingStats()
{
    return pathStats;
}

std::string describePathfindingStats()
{
    static const char *names[] = {"firstStep", "distanceTo", "pathToRoom", "NPC hunt", "routeTo"};
    std::ostringstream out;
    out << "\r\nPathfinding (since boot; included in the routine totals above):\r\n"
        << "API              Calls   Rooms visited   Max rooms   Total ms   Avg ms   Max ms   Slowest from/to\r\n";
    for (unsigned i = 0; i < static_cast<unsigned>(PathfindingApi::Count); ++i)
    {
        const auto &stats = pathStats[i];
        const double milliseconds = stats.totalNanoseconds / 1000000.0;
        out << std::left << std::setw(12) << names[i] << std::right
            << std::setw(10) << stats.calls << std::setw(16) << stats.roomsVisited
            << std::setw(12) << stats.maxRoomsVisited << std::fixed << std::setprecision(3)
            << std::setw(11) << milliseconds
            << std::setw(9) << (stats.calls ? milliseconds / stats.calls : 0.0)
            << std::setw(9) << stats.maxNanoseconds / 1000000.0
            << "   " << stats.slowestSource << "/" << stats.slowestTarget << "\r\n";
    }
    return out.str();
}

std::string DistanceString(int dist)
{
    if (dist == 0 || dist == 1) return "";
    if (dist == 2) return "far";
    if (dist == 3) return "very far";
    if (dist == 4) return "very very far";
    if (dist == 5) return "extremely far";
    return "somewhere";
}

std::list<int> Room::pathToRoom(Room *otherRoom)
{
    std::list<int> path;
    searchRooms(this, otherRoom, false, PathfindingApi::FullPath, &path);
    return path;
}

int Room::findFirstStep(Room *otherRoom)
{
    if (!otherRoom)
        Log("Illegal value %p or %p passed to find_first_step. (%s)",
            static_cast<void*>(this), static_cast<void*>(otherRoom), __FILE__);
    return searchRooms(this, otherRoom, false, PathfindingApi::FirstStep).firstStep;
}

int Room::getDistanceToRoom(Room *otherRoom)
{
    return searchRooms(this, otherRoom, false, PathfindingApi::Distance).distance;
}

RoomRoute Room::routeToRoom(Room *otherRoom)
{
    return searchRooms(this, otherRoom, false, PathfindingApi::Route);
}

int find_first_step(Room *source, Room *target)
{
    if (!source || !target)
        Log("Illegal value %p or %p passed to find_first_step. (%s)",
            static_cast<void*>(source), static_cast<void*>(target), __FILE__);
    return searchRooms(source, target, true, PathfindingApi::Hunt).firstStep;
}


/********************************************************
* Functions and Commands which use the above functions. *
********************************************************/

CommandHandler  do_track  = DEFINE_COMMAND
{
	char arg[ MAX_INPUT_LENGTH ];

	OneArgument( argument, arg );

	if ( !ch->CanTrack(ch->in_room) )
	{
		ch->send( "You can't seem to make out any tracks here.\r\n" );
		return ;
	}

	if ( !ch->command_ready )
		ch->command_ready = true;
	else
	{
		if(*arg)
			ch->PrintTracks( ch->in_room, std::string(arg) );
		else
			ch->PrintTracks( ch->in_room, false);
	}
};

void Character::HuntVictim()
{
	int dir;
	Character *tmp;

	if (FIGHTING(this) || !IS_NPC(this) || !this->player.hunting)
		return;
	if( this->player.hunting && this->player.hunting->IsPurged() )
	{
		this->player.hunting = 0;
		return;
	}

	/* make sure the char still exists */
	for (tmp = character_list; tmp; tmp = tmp->next )
	{
		if ( this->player.hunting == tmp )
			break;
	}

	if ( !tmp || ( dir = find_first_step( this->in_room, this->player.hunting->in_room ) ) < 0 )
		this->player.hunting = 0;
	else
	{
		if (!this->master && !MOB_FLAGGED(this, MOB_SENTINEL) &&
			(!ROOM_FLAGGED(EXIT(this, dir)->getToRoom(), ROOM_NOMOB) &&
			!ROOM_FLAGGED(EXIT(this, dir)->getToRoom(), ROOM_DEATH) &&
			!IS_SET(this->MobData->nsects, (1 << EXIT(this, dir)->getToRoom()->getSector()->getValue())) &&
			(!MOB_FLAGGED(this, MOB_STAY_ZONE) ||
			(EXIT(this, dir)->getToRoom()->getZoneNumber() == this->in_room->getZoneNumber()))))

		{
			perform_move( this, dir, 1 );
		}
	}
}
