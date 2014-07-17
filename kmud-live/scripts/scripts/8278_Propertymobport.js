var script8278 = function(self, actor, here, args, extra) {
	/* Koradin, 11/20/12 */
	/* Script for porting humans or trollocs via property mobs */
	var argArray = getArgList(args);
	var roomNum;
	var cost;
	var manor = getManor(self.room);
	//self.comm("say I'll be providing teleportation services soon...");
	//return;
	if (argArray[0].toLowerCase() != "help" && argArray[0].toLowerCase() != "cost" && argArray[0].toLowerCase() != "port")
		return;
	if (actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) {
		waitpulse 3;
		self.comm("say I refuse to provide you with services, "+actor.name+"!");
		return;
	}
	if( argArray[0] == "help" )
	{
		waitpulse 4;
		self.comm( "tell " + actor.name + " I can send you to a secluded swamp in the BLIGHT, the plains in SHIENAR, a pond near TAR VALON, a pasture near FOUR KINGS, or the mountains near BAERLON. " );
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
		cost = actor.room.distanceTo( getRoom(2066) );
		roomNum = 2066;
	}
	else if( argArray[1].toLowerCase() == "shienar" )
	{
		cost = actor.room.distanceTo( getRoom(19356) );
		roomNum = 19356;
	}
	else if( argArray[1].toLowerCase() == "tar" && strn_cmp(argArray[2], "valon", 5))
	{
		cost = actor.room.distanceTo( getRoom(8227) );
		roomNum = 8227;
	}
	else if( argArray[1].toLowerCase() == "four" && strn_cmp(argArray[2], "kings", 5) )
	{
		cost = actor.room.distanceTo( getRoom(347) );
		roomNum = 347;
	}
	else if( argArray[1].toLowerCase() == "baerlon" )
	{
		cost = actor.room.distanceTo( getRoom(13546) );
		roomNum = 13546;
	}
	else if( argArray[0] != "cost" )
	{
		return;
	}
	cost *= 9;
	for each( var follower in actor.followers ) {
		if( follower.id != manor.ownerUserId && !arrContains(manor.allowedUsers, follower.id) )
		{
			self.comm( "say I won't provide services to " + capFirstLetter(follower.name) + "!" );
			return;
		}
		if ( follower.vnum == -1 ) { cost = cost * 2; }
	}
		
	if( cost == -1 )
		return;
	else if( argArray[0].toLowerCase() == "cost" )
	{	
		waitpulse 7;
		if (isNaN(cost)) {
			self.comm("say I can't send you there.");
			return;
		}
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
	manor.goldOwed += Math.floor(cost/4);
	saveManor(manor);
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
	
	
}