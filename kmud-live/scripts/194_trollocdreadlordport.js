var script194 = function(self, actor, here, args, extra)
{
	/* Fogel, 5/26/10 */
/* Script for porting trollocs around the world via a dreadlord mob. */
var argArray = getArgList(args);
var roomNum;
var cost = -1;
if( actor.race != constants.RACE_TROLLOC )
{
	return;
}
if( argArray[0] == "help" )
{
	waitpulse 4;
	self.comm( "tell " + actor.name + " I can send you to the cellar in FOUR KINGS, the mausoleum of DOON the Dark, the forested PATH near Tar Valon, " );
	self.comm( "tell " + actor.name + " the forest near FAL MORAN, the woods north of FAL DARA, the abandoned VILLAGE south of Caemlyn, our KEEP, or the blackened HOLD." );
	self.comm( "tell " + actor.name + " If you would like to know the cost to travel to a location, say COST [location]." );
	self.comm( "tell " + actor.name + " To travel to a location, say PORT [location]. For an additional charge, anything following you will also be ported." );
	return;
}
if( !argArray[1] )
{
	return;
}
if( argArray[1].toLowerCase() == "four" && argArray[2].toLowerCase() == "kings" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(385) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(385) );
	roomNum = 385;
}
else if( argArray[1].toLowerCase() == "doon" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(5350) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(5350) );
	roomNum = 5350;
}
else if( argArray[1].toLowerCase() == "path" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(1239) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(1239) );
	roomNum = 1239;
}
else if( argArray[1].toLowerCase() == "fal" && argArray[2].toLowerCase() == "moran" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(9321) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(9321) );
	roomNum = 9321;
}
else if( argArray[1].toLowerCase() == "fal" && argArray[2].toLowerCase() == "dara" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(20675) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(20675) );
	roomNum = 20675;
}
else if( argArray[1].toLowerCase() == "village" )
{
	if( here.vnum == 8004 )
		cost = getRoom(8004).distanceTo( getRoom(23072) );
	else if( here.vnum == 8102 )
		cost = getRoom(8102).distanceTo( getRoom(23072) );
	roomNum = 23072;
}
else if( argArray[1].toLowerCase() == "keep" )
{
	if( here.vnum == 8004 )
	{
		waitpulse 7;
		self.comm( "say You are already in the Keep!" );
		return;
	}
	else if( here.vnum == 8102 )
	{
		if( actor.clan )
			cost = 0;
		else
			cost = getRoom(8102).distanceTo( getRoom(8004) );
	}
	roomNum = 8004;
}
else if( argArray[1].toLowerCase() == "hold" )
{
	if( here.vnum == 8004 )
	{
		if( actor.clan )
			cost = 0;
		else
			cost = getRoom(8004).distanceTo( getRoom(8102) );
	}
	else if( here.vnum == 8102 )
	{
		waitpulse 7;
		self.comm( "say You are already in the Blackened Hold!" );
		return;
	}
	roomNum = 8102;
}
else if( argArray[0] != "cost" )
{
	return;
}
for each( var follower in actor.followers )
	if ( follower.vnum == -1 ) { cost = cost * 2; }
	
if( cost == -1 )
	return;
else if( argArray[0].toLowerCase() == "cost" )
{	
	waitpulse 7;
	self.comm( "say Your travel cost to that location will be " + cost + " coppers." );
	return;
}
if( actor.gold < cost )
{	
	waitpulse 5;
	self.comm( "say You don't have enough money to go there." );
	return;
}
if( actor.affectedBy(constants.AFF_NOQUIT) )
{
	self.comm("say Calm down a bit first.");
	return;
}
var room = getRoom(roomNum);
waitpulse 5;
self.comm( "say If you wish." );
actor.gold -= cost;
waitpulse 14;
here.loadObj(10600);
var portalHere = here.items[0];
room.loadObj(10600);
var portalRoom = room.items[0];
here.echo( "A shimmering portal appears before you." );
room.echo( "A shimmering portal appears." );
waitpulse 7;
if( actor.room == here )
{
	echoaround( actor, capFirstLetter(actor.name) + " is pushed through the portal." );
	actor.send( "You are pushed through the portal." );
	actor.moveToRoom( room );
	actor.comm( "look" );
	echoaround( actor, capFirstLetter(actor.name) + " steps through a shimmering portal." );
}
for each( var follower in actor.followers )
{
	if( follower.affectedBy(constants.AFF_NOQUIT) )
	{
		self.comm( "say " + capFirstLetter(follower.name) + " Needs to calm down a bit first." );
	}
	else if( follower.room != here )
	{
		continue;
	}
	else
	{
		act( "$n follows $N through the portal.", true, follower, null, actor, constants.TO_NOTVICT );
		act( "You follow $N through the portal.", true, follower, null, actor, constants.TO_CHAR );
		follower.moveToRoom( room );
		echoaround( follower, capFirstLetter(follower.name) + " steps through a shimmering portal." );
		follower.comm( "look" );
	}
}
waitpulse 5;
here.echo( "A shimmering portal closes in on itself, leaving no trace of its existence." );
room.echo( "A shimmering portal closes in on itself, leaving no trace of its existence." );
portalHere.extract();
portalRoom.extract();


};

