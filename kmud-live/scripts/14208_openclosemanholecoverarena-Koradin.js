var script14208 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if (!vArgs[1] || vArgs[2]) {
	_noblock;
	return;
}
if (str_cmp(vArgs[1],"manhole") || getSval(actor,14208,"manhole") == 1) {
	_noblock;
	return;
}
_block;
if (getSval(self,14208,"running") == 1) {
	actor.send("Someone's already moving the manhole cover!");
	return;
}
if (self.doorName(constants.UP) == "manhole") {
	var doorDir = constants.UP;
	var otherRoom = self.direction(constants.UP);
	var coverLoc = "below";
}
else {
	var doorDir = constants.DOWN;
	var otherRoom = self.direction(constants.DOWN);
	var coverLoc = "above";
}
if (strn_cmp(vArgs[0],"open",2)) {
	if (!self.doorIsClosed(doorDir)) {
		actor.send("The manhole is already open!");
		return;
	}
	var actorMsg = "You begin removing the heavy cover from the manhole...";
	var roomMsg = "$n squats near the manhole and begins hefting the cover...";
	var actorCom = "open manhole";
}
else if (strn_cmp(vArgs[0],"close",2)) {
	if (self.doorIsClosed(doorDir)) {
		actor.send("The manhole is already closed!");
		return;
	}
	var actorMsg = "You begin replacing the heavy cover, moving it over the manhole...";
	var roomMsg = "$n begins replacing the manhole cover...";
	var actorCom = "close manhole";
}
otherRoom.echo("The manhole cover "+coverLoc+" you begins to move...");
actor.send(actorMsg);
act(roomMsg,true,actor,null,null,constants.TO_ROOM);
setSval(self,14208,"running",1);
actor.startTimer(3);
var success = runTimer(actor);
if (!success) {
	actor.send("\nYou give up the attempt.");
	act("$n gives up the attempt, dropping the manhole cover.",true,actor,null,null,constants.TO_ROOM);
	setSval(self,14208,"running",0);
	return;
}
setSval(actor,14208,"manhole",1);
actor.send("");
actor.comm(actorCom);
setSval(actor,14208,"manhole",0);
setSval(self,14208,"running",0);

};

