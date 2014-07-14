var script14210 = function(self, actor, here, args, extra)
{
	//ENTER ARENA - global entry by players
//Koradin - June 2011

/** if(new Date().getDay() != 4) {

	actor.send("The arena is only open on Thursdays(server time).\r\n");
	_block;
	return;
} **/

var vArgs = getArgList(args);
getCharCols(actor);
if (!vArgs[1] || vArgs[2]) {
	_noblock;
	return;
}
if (str_cmp(vArgs[1],"arena") || actor.room.zoneVnum == global.arenaZoneVnum || ( global.arenaStage == constants.ARENA_PLAYING && arrContains(global.arenaStartRooms, actor.room.vnum) )) { //Not entering arena or already inside arena
	_noblock; // See trigger 14200 for the ENTER ARENA command used to rejoin the arena after you die
	return;
}
if (str_cmp(vArgs[1],"arena") || ( (actor.room.vnum == 500 || actor.room.vnum == 502) && arrContains(global.arenaPlayers, actor))) {
	_noblock; // See trigger 14200 for enter arena command - this is for people who have idled to crossroads
	return;
}
_block;
if (actor.level < 30) {
	actor.send("You must be level 30 or higher to enter the Arena.");
	return;
}
else if (global.arenaStage == constants.ARENA_OFF) { //Arena is closed
	actor.send("The Arena is only open on Thursdays (server time).");
	return;
}
else if (global.arenaStage == constants.ARENA_PLAYING) { //Arena already running
	actor.send("There is already an Arena match in progress.");
	actor.send(" ");
	actor.send("You can observe the match by typing "+mag+"arena watch"+nrm+".");
	return;
}
else if (global.arenaStage == constants.ARENA_VOTING) { //Arena about to begin
	actor.send("An Arena match is about to begin.  Try again later!");
	return;
}
if (actor.affectedBy(constants.AFF_NOQUIT)) { //Can't enter with NOQUIT
	actor.send("Calm down first!");
	return;
}
if (getSval(actor,14210,"arenaQueue") == 1) { //Already in queue
	actor.send("You're already waiting to enter the Arena!");
	actor.send(" ");
	actor.send("To stop waiting in line, type "+mag+"exit arena"+nrm+".");
	return;
}
actor.addToArenaQueue();
act("$n enters the Arena queue.",true,actor,null,null,constants.TO_ROOM);



};

