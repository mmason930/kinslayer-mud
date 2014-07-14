var script8277 = function(self, actor, here, args, extra)
{
	/* Fogel, 10/07/10 */
/* Script for porting humans or trollocs via the paranoid man mobs */
var argArray = getArgList(args);
var roomNum;
var cost;
if( argArray[0] == "help" )
{
	waitpulse 4;
	self.comm( "tell " + actor.name + " I can send you to an abandoned watchtower in northern SHIENAR, or to a secluded swamp in the BLIGHT. " );
	self.comm( "tell " + actor.name + " If you would like to know the cost to travel to a location, say COST [location]." );
	self.comm( "tell " + actor.name + " To travel to a location, say PORT [location]. For an additional charge, anything following you will also be ported." );
	return;
}
if( !argArray[1] )
{
	return;
}
if( argArray[1].toLowerCase() == "blight" )
{
	if( here.vnum == 8277 )
		cost = getRoom(8277).distanceTo( getRoom(2066) );
	else if( here.vnum == 5631 )
		cost = getRoom(5631).distanceTo( getRoom(2066) );
	roomNum = 2066;
}
else if( argArray[1].toLowerCase() == "shienar" )
{
	if( here.vnum == 8277 )
		cost = getRoom(8277).distanceTo( getRoom(19390) );
	else if( here.vnum == 5631 )
		cost = getRoom(5631).distanceTo( getRoom(19390) );
	roomNum = 19390;
}
else if( argArray[0] != "cost" )
{
	return;
}
cost *= 4;
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

