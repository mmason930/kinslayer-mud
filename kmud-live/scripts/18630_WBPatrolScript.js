var script18630 = function(self, actor, here, args, extra)
{
	// Patrol Script for Whitebridge
// Rhollor January 2010
if(self.position != constants.POS_STANDING) {
	self.send(self.red(constants.CL_OFF) + "Incorrect position to continue patrol!" + self.normal(constants.CL_OFF));
	return;
}
var scriptNum = 18630;
var numWaypoints = 9;
/* Change allowBackTracking to 'true' if you wish to allow the patrol to return to the path the came from */
	/*** This would be _required_ for a patrol with only 2 waypoints ***/
var allowBackTracking = false;
if(!allowBackTracking && numWaypoints <= 2) {
	mudLog(2,100,"JScript " + scriptNum + " ERR: Patrol does not allow back tracking and has 2 or fewer waypoints!");
	return;
}
var targetRoom = getSval(self,scriptNum,"targetRoom");
var prevRoom = getSval(self,scriptNum,"prevRoom");
var options = [];
//List of Waypoints each with an array of possible Target Rooms (options) to head toward (also Waypoints)
/**** Waypoint 1 ****/
if(here.vnum == 18673) {
	/* List of Possible Next Waypoints */
	options = [18629,18659,18616];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 2 ****/
else if(here.vnum == 18629) {
	options = [18673,18611,18637];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 3 ****/
else if(here.vnum == 18659) {
	options = [18673,18647,18616,18606];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 4 ****/
else if(here.vnum == 18616) {
	options = [18673,18659,18611,18606];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 5 ****/
else if(here.vnum == 18611) {
	options = [18616,18647,18629,18606];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 6 ****/
else if(here.vnum == 18647) {
	options = [18659,18611,18637,18606];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 7 ****/
else if(here.vnum == 18637) {
	options = [18647,18629];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 8 ****/
else if(here.vnum == 18606) {
	options = [18647,18659,18616,18611,18601];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
}
/**** Waypoint 9 ****/
else if(here.vnum == 18601) {
	options = [18606];
	setSval(self,scriptNum,"prevRoom",targetRoom);
	setSval(self,scriptNum,"targetRoom","");
	targetRoom = "";
	/*** This location is a dead end so allow backtracking for this waypoint only! ***/
	allowBackTracking = true;
}
/* Add as many waypoints as you like! */
//Convert vnums to JSRooms
for(var i = 0; i < options.length; i++) {
	options[i] = getRoom(options[i]);
}
//If this is a targetRoom pick a new direction
if(targetRoom == "" || !targetRoom || targetRoom == "undefined") {
	var rand;
	if(!allowBackTracking && numWaypoints > 2) {
		//You cannoy return to the path you came from
		do {
			rand = random(0,options.length-1);
		} while (options[rand] == prevRoom); //Make sure we aren't headed back the dir we came
	}
	else {
		//May return you down the same path you came from
		rand = random(0,options.length-1);
	}
	setSval(self,scriptNum,"targetRoom",options[rand]);
	targetRoom = options[rand];
	if(prevRoom && targetRoom.vnum == prevRoom.vnum) {
		self.send(self.red(constants.CL_OFF) + "Taking reverse route!" + self.normal(constants.CL_OFF));
	}
	setSval(self,scriptNum,"prevRoom",here);
}
var dir = here.firstStep(targetRoom);
var door = 0;
if(here.doorIsClosed(dir)) {
	self.comm("open " + here.doorName(dir));
	door = 1;
}
self.comm(dirToText(dir));
if(door) {
	self.comm("close " + here.direction(dir).doorName(revDir(dir)));
}

};

