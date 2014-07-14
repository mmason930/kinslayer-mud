var script14200 = function(self, actor, here, args, extra)
{
	//ENTER ARENA command - local arena only
//Koradin - June 2011
var vArgs = getArgList(args);
if (!arrContains(global.arenaPlayers, actor)) {
	_noblock;
	return;
}
_block;
if (!vArgs[1] || vArgs[2]) {
	getCharCols(actor);
	actor.send("Type "+cyn+"enter arena"+nrm+" to rejoin the battle!");
	return;
}
var respawnTime = (getSval(actor,20860,"respawnTime") - time());
if (respawnTime > 0) { // player must wait to respawn
	getCharCols(actor);
	var minuteS = " second";
	if (respawnTime != 1)
		minuteS += "s";
	actor.send("You still have "+mag+respawnTime+nrm+minuteS+" before you can rejoin the battle!");
	return;
}
if ( strn_cmp(vArgs[0],"enter",3) ) {
	if ( strn_cmp(vArgs[1],"arena",5) ) {
		if (global.arenaCurrentGame == global.arenaGameLMS) { //Last Man Standing
			if (getSval(actor,20860,"deathCount") >= global.arenaCurrentGame.limit) { //Player has no lives left
				actor.send("You're out of lives!  Rest here until the match is finished.");
				actor.send(" ");
				actor.send("Note: Leaving the match early will negatively affect your score.");
				return;
			}
		}
		var ffaRand = [];
		for (i=global.arenaCurrentMap.min; i<global.arenaCurrentMap.max+1; i++) {
			if (!arrContains(global.arenaCurrentMap.exclusions, i))
				ffaRand.push(i);
		}
		if (actor.room.vnum == 500 || actor.room.vnum == 502) { //actor has idled to crossroads
			var randomRoom = (actor.race == constants.RACE_HUMAN ? getRoom(14201) : getRoom(14299));
		}
		else if (global.arenaFreeForAll == true)
			var randomRoom = getRoom(ffaRand[random(0,ffaRand.length-1)]);
		else if (actor.arenaTeam == constants.ARENA_BLUE)
			var randomRoom = getRandomArenaRoom("blue");
		else if (actor.arenaTeam == constants.ARENA_RED)
			var randomRoom = getRandomArenaRoom("red");
		else if (actor.arenaTeam == constants.ARENA_YELLOW)
			var randomRoom = getRandomArenaRoom("yellow");
		else if (actor.arenaTeam == constants.ARENA_GREEN)
			var randomRoom = getRandomArenaRoom("green");
		actor.send("You rejoin the battle!");
		act("$n rejoins the battle, disappearing from view.",true,actor,null,null,constants.TO_ROOM);
		actor.moveToRoom(randomRoom);
		actor.comm("look");
		act("$n appears in a flash of light, rejoining the battle!",true,actor,null,null,constants.TO_ROOM);
		return;
	}
}
getCharCols(actor);
actor.send("Type "+cyn+"enter arena"+nrm+" to rejoin the fray!");

};

