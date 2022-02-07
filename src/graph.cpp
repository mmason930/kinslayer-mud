/* ************************************************************************
*   File: graph.c                                       Part of CircleMUD *
*  Usage: various graph algorithms                                        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * You can define or not define TRACK_THOUGH_DOORS, depending on whether
 * or not you want track to find paths which lead through closed or
 * hidden doors. A setting of '#if 0' means to not go through the doors
 * while '#if 1' will pass through doors to find the target.
 */
#if 1
#define TRACK_THROUGH_DOORS 1
#endif

#include "conf.h"


#include "utils.h"
#include "interpreter.h"
#include "db.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"


/* Externals */
extern Character *character_list;
extern const char *dirs[];


/* local functions */
void bfs_enqueue( Room *room, int dir, int depth = 0 );
void bfs_dequeue( void );
void bfs_clear_queue( void );
int find_first_step( Room *src, Room *target );

struct bfs_queue_struct
{
	Room *room;
	char dir;
	int depth;
	struct bfs_queue_struct *next;
};

static struct bfs_queue_struct *queue_head = 0, *queue_tail = 0;

/* Utility macros */
#define MARK(room) (SET_BITK(ROOM_FLAGS(room), (1<<ROOM_BFS_MARK)))
#define UNMARK(room) (REMOVE_BIT(ROOM_FLAGS(room), (1<<ROOM_BFS_MARK)))
#define IS_MARKED(room) (ROOM_FLAGGED(room, ROOM_BFS_MARK))
#define TOROOM(x, y) ((x)->dir_option[(y)]->getToRoom())

#define VALID_EDGE(x, y)  ((x)->dir_option[(y)] && (TOROOM((x), y)) && (!ROOM_FLAGGED(TOROOM((x), y), ROOM_NOTRACK)) && (!IS_MARKED(TOROOM((x), y))))
#define VALID_EDGE2(x, y) ((x)->dir_option[(y)] && (TOROOM((x), y)) && (!IS_MARKED(TOROOM((x), y))))

std::string DistanceString( int dist )
{
	if ( dist == 0 || dist == 1 )
		return "";
	else if ( dist == 2 )
		return "far";
	else if ( dist == 3 )
		return "very far";
	else if ( dist == 4 )
		return "very very far";
	else if ( dist == 5 )
		return "extremely far";
	else
		return "somewhere";
}

class PathNode
{
public:

	PathNode()
	{
		TheRoom = NULL;
		dir_to_here = 0;
	}
	PathNode( Room *R, const int dir )
	{
		TheRoom = R;
		dir_to_here = dir;
	}

	Room *TheRoom;
	int dir_to_here;
};

std::queue< std::stack< PathNode >* > PathQueue;

void ClearPathQueue()
{
	while( !PathQueue.empty() )
	{
		std::stack< PathNode > *Path = PathQueue.front();
		while( !Path->empty() )
		{
			UNMARK( Path->top().TheRoom );
			Path->pop();
		}
		delete PathQueue.front();
		PathQueue.pop();
	}
}

std::list<int> Room::pathToRoom( Room *OtherRoom )
{
	std::list<int> ThePath;
	std::stack< PathNode > *CurrentPath;
	int i;

	//No distance between here and the destination.
	if( OtherRoom == this ) return ThePath;

	for(i = 0;i < World.size();++i)
		UNMARK(World[i]);

	for(i = 0;i < NUM_OF_DIRS;++i)
	{
		if( VALID_EDGE2(this, i) )
		{
			CurrentPath = new std::stack< PathNode >;
			CurrentPath->push( PathNode(this->dir_option[i]->getToRoom(), i) );
			PathQueue.push( CurrentPath );

			MARK(this->dir_option[i]->getToRoom());
		}
	}

	MARK(this);

	while( !PathQueue.empty() )
	{
		CurrentPath = PathQueue.front();
		//We have found our destination.
		if( CurrentPath->top().TheRoom == OtherRoom )
		{
			while( !CurrentPath->empty() )
			{
				ThePath.push_front(CurrentPath->top().dir_to_here);
				CurrentPath->pop();
			}
			ClearPathQueue();//This will invalidate CurrentPath!
			return ThePath;
		}
		else
		{
			PathQueue.pop();
			for( i = 0;i < NUM_OF_DIRS;++i )
			{
				if( VALID_EDGE2(CurrentPath->top().TheRoom, i) )
				{
					MARK(CurrentPath->top().TheRoom->dir_option[i]->getToRoom());
					std::stack< PathNode > *NewStack = new std::stack< PathNode >( *CurrentPath );
					NewStack->push(PathNode(CurrentPath->top().TheRoom->dir_option[i]->getToRoom(), i));
					PathQueue.push( NewStack );
				}
			}
			delete CurrentPath;
		}
	}
	return ThePath;
}

int Room::findFirstStep( Room *OtherRoom )
{
	int curr_dir, curr_room;

	if ( !OtherRoom )
	{
		Log( "Illegal value %p or %p passed to find_first_step. (%s)", static_cast<void*>(this), static_cast<void*>(OtherRoom), __FILE__ );
		return BFS_ERROR;
	}

	if ( this == OtherRoom )
		return BFS_ALREADY_THERE;

	/* clear marks first, some OLC systems will save the mark. */
	for ( curr_room = 0; (unsigned int)curr_room < World.size(); ++curr_room )
		UNMARK( ( World[ curr_room ] ) );

	MARK( this );

	/* first, enqueue the first steps, saving which direction we're going. */
	for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
	{
		if ( VALID_EDGE2( this, curr_dir ) )
		{
			MARK( TOROOM( this, curr_dir ) );
			bfs_enqueue( TOROOM( this, curr_dir ), curr_dir );
		}
	}
	/* now, do the classic BFS. */
	while ( queue_head )
	{
		if ( queue_head->room == OtherRoom )
		{
			curr_dir = queue_head->dir;
			bfs_clear_queue();
			return curr_dir;
		}

		else
		{
			for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
			{
				if ( VALID_EDGE2( queue_head->room, curr_dir ) )
				{
					MARK( TOROOM( queue_head->room, curr_dir ) );
					bfs_enqueue( TOROOM( queue_head->room, curr_dir ), queue_head->dir );
				}
			}
			bfs_dequeue();
		}
	}
	return BFS_NO_PATH;
}
int Room::getDistanceToRoom( Room* OtherRoom )
{
	int curr_dir, curr_room, curr_depth;

	if ( this == OtherRoom )
		return 0;

	/* clear marks first, some OLC systems will save the mark. */
	for ( curr_room = 0; (unsigned int)curr_room < World.size(); ++curr_room )
		UNMARK( ( World[ curr_room ] ) );

	MARK( this );

	/* first, enqueue the first steps, saving which direction we're going. */
	for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
	{
		if ( VALID_EDGE2( this, curr_dir ) )
		{
			MARK( (this)->dir_option[(curr_dir)]->getToRoom() );
			bfs_enqueue( (this)->dir_option[(curr_dir)]->getToRoom(), curr_dir, 1 );
		}
	}
	/* now, do the classic BFS. */
	while ( queue_head )
	{
		if ( queue_head->room == OtherRoom )
		{
			curr_depth = queue_head->depth;
			bfs_clear_queue();
			return curr_depth;
		}

		else
		{
			for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
			{
				if ( VALID_EDGE2( queue_head->room, curr_dir ) )
				{
					MARK( (queue_head->room)->dir_option[(curr_dir)]->getToRoom() );
					bfs_enqueue( (queue_head->room)->dir_option[(curr_dir)]->getToRoom(), queue_head->dir, queue_head->depth+1 );
				}
			}
			bfs_dequeue();
		}
	}
	return -1;
}

void bfs_enqueue( Room *room, int dir, int depth )
{
	struct bfs_queue_struct * curr;

	CREATE( curr, struct bfs_queue_struct, 1 );
	curr->room = room;
	curr->dir = dir;
	curr->next = 0;
	curr->depth = depth;

	if ( queue_tail )
	{
		queue_tail->next = curr;
		queue_tail = curr;
	}

	else
		queue_head = queue_tail = curr;
}


void bfs_dequeue( void )
{
	struct bfs_queue_struct * curr;

	curr = queue_head;

	if ( !( queue_head = queue_head->next ) )
		queue_tail = 0;

	delete ( curr );
}


void bfs_clear_queue( void )
{
	while ( queue_head )
		bfs_dequeue();
}


/*
 * find_first_step: given a source room and a target room, find the first
 * step on the shortest path from the source to the target.
 *
 * Intended usage: in mobileActivity, give a mob a dir to go if they're
 * tracking another mob or a PC.  Or, a 'track' skill for PCs.
 */
int find_first_step( Room *src, Room *target )
{
	int curr_dir, curr_room;

	if ( !src || !target )
	{
		Log( "Illegal value %p or %p passed to find_first_step. (%s)", static_cast<void*>(src), static_cast<void*>(target), __FILE__ );
		return BFS_ERROR;
	}

	if ( src == target )
		return BFS_ALREADY_THERE;

	/* clear marks first, some OLC systems will save the mark. */
	for ( curr_room = 0; (unsigned int)curr_room < World.size(); ++curr_room )
		UNMARK( ( World[ curr_room ] ) );

	MARK( src );

	/* first, enqueue the first steps, saving which direction we're going. */
	for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
	{
		if ( VALID_EDGE( src, curr_dir ) )
		{
			MARK( TOROOM( src, curr_dir ) );
			bfs_enqueue( TOROOM( src, curr_dir ), curr_dir );
		}
	}
	/* now, do the classic BFS. */
	while ( queue_head )
	{
		if ( queue_head->room == target )
		{
			curr_dir = queue_head->dir;
			bfs_clear_queue();
			return curr_dir;
		}

		else
		{
			for ( curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++ )
			{
				if ( VALID_EDGE( queue_head->room, curr_dir ) )
				{
					MARK( TOROOM( queue_head->room, curr_dir ) );
					bfs_enqueue( TOROOM( queue_head->room, curr_dir ), queue_head->dir );
				}
			}
			bfs_dequeue();
		}
	}
	return BFS_NO_PATH;
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
