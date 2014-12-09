var script193 = function(self, actor, here, args, extra) {
	/* Fogel, 5/26/10 */
	/* Script for porting humans around the world via an Aes Sedai mob. */
	var argArray = getArgList(args);
	var roomNum;
	var cost;
	if( actor.race != constants.RACE_HUMAN )
	{
		return;
	}
	if( argArray[0] == "help" )
	{
		waitpulse 4;
		self.comm( "tell " + actor.name + " I can send you to the BASIN in Tarwin's Gap, the OBSIDIAN TOWER and BLIGHTED TOWER in Blight, " );
		self.comm( "tell " + actor.name + " the STEDDING near Tarwin's Gap, FAL DARA, FAL MORAN, TAR VALON, LUGARD, CAEMLYN, WHITEBRIDGE or BAERLON." );
		self.comm( "tell " + actor.name + " If you would like to know the cost to travel to a location, say COST [location]." );
		self.comm( "tell " + actor.name + " To travel to a location, say PORT [location]. For an additional charge, anything following you will also be ported." );
		return;
	}
	if( !argArray[1] )
	{
		return;
	}
	if( argArray[1].toLowerCase() == "basin" )
	{
		if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(20622) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(20622) );
		roomNum = 20622;
	}
	else if( argArray[1].toLowerCase() == "obsidian" && argArray[2].toLowerCase() == "tower" )
	{
		if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(2047) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(2047) );
		roomNum = 2047;
	}
	else if( argArray[1].toLowerCase() == "blighted" && argArray[2].toLowerCase() == "tower" )
	{
		if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(1091) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(1091) );
		roomNum = 1091;
	}
	else if( argArray[1].toLowerCase() == "stedding" )
	{
		if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(18198) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(18198) );
		roomNum = 18198;
	}
	else if( argArray[1].toLowerCase() == "fal" && argArray[2].toLowerCase() == "dara" )
	{
		if( here.vnum == 166 )
		{
			if( actor.clan )
				cost = 0;
			else
				cost = getRoom(166).distanceTo( getRoom(1735) );
		}
		else if( here.vnum == 1726 )
		{
			waitpulse 7;
			self.comm( "say You are already in Fal Dara!" );
			return;
		}
		roomNum = 1735;
	}
	else if( argArray[1].toLowerCase() == "fal" && argArray[2].toLowerCase() == "moran" )
	{
		if( actor.inClan( constants.CLAN_SHIENARAN ) )
			cost = 0;
		else if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(10335) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(10335) );
		roomNum = 10335;
	}
	else if( argArray[1].toLowerCase() == "tar" && argArray[2].toLowerCase() == "valon" )
	{
		if( actor.inClan( constants.CLAN_GAIDIN ) || actor.inClan( constants.CLAN_WHITE_TOWER ) || actor.inClan( 6 ) )
			cost = 0;
		else if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(21304) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(21304) );
		roomNum = 21304;
	}
	else if( argArray[1].toLowerCase() == "lugard" )
	{
		if( actor.inClan( constants.CLAN_MURANDIAN ) )
			cost = 0;
		else if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(20101) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(20101) );
		roomNum = 20101;
	}
	else if( argArray[1].toLowerCase() == "caemlyn" )
	{
		if( here.vnum == 166 )
		{
			waitpulse 7;
			self.comm( "say You are already in Caemlyn!" );
			return;
		}
		else if( actor.inClan( constants.CLAN_ANDORAN ) )
			cost = 0;
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(100) );
		roomNum = 100;
	}
	else if( argArray[1].toLowerCase() == "baerlon" )
	{
		if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(4530) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(4530) );
		roomNum = 4530;
	} else if( argArray[1] == "whitebridge" || (argArray[1] == "white" && argArray[2] == "bridge")) {
        if( here.vnum == 166 )
			cost = getRoom(166).distanceTo( getRoom(22700) );
		else if( here.vnum == 1726 )
			cost = getRoom(1726).distanceTo( getRoom(22700) );
		roomNum = 22700;
    }
	else if( argArray[0] != "cost" )
	{
		return;
	}
	for (var _autoKey in actor.followers) {
		var follower = actor.followers[_autoKey];
		if ( follower.vnum == -1 ) { cost = cost * 2; }
	}
		
	if( cost == -1 )
		return;
	else if( argArray[0].toLowerCase() == "cost" )
	{	
		waitpulse 7;

		if(isNaN(cost) || cost == null)
			self.say("I cannot move you to that location, " + actor.name + ".");
		else
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
	for (var _autoKey in actor.followers ) {
		var follower = actor.followers [_autoKey];
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