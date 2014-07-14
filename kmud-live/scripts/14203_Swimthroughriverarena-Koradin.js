var script14203 = function(self, actor, here, args, extra)
{
	var newRoom = extra.direction;
if (getSval(actor, 14203, "riverSwim") == 1)
{
_noblock;
return;
}
_block;
if (dirToText(newRoom).toLowerCase() == "down") {
	actor.send("You swim desperately, but the rushing water carries you over the brink.");
	act("$n's screams die away as $e is carried over the waterfall.",true,actor,null,null,constants.TO_ROOM);
	setSval(actor, 14203, "riverSwim", 1);
	actor.comm("down");
	setSval(actor, 14203, "riverSwim", 0);
	if (actor.room.vnum == 14200) {
	actor.send("You hardly notice the SPLAT your body makes against the rocks before the torrent washes over you.");
	act("$n smashes against the rocks, the water carrying $s lifeless body out to sea.",true,actor,null,null,constants.TO_ROOM);
	actor.damage(10000);
	}
	return;
}
else if (dirToText(newRoom).toLowerCase() == "south") {
	var timer = 1;
	var swimMsg = "You start swimming downriver...";
	var successMessage = "\nYou easily swim with the current, drifting south.";
	var successToAll = "$n lets the current do the work, easily swimming south.";
}
else if (dirToText(newRoom).toLowerCase() == "north") {
	var timer = 2;
	var swimMsg = "You start swimming upriver...";
	var successMessage = "\nYou fight your way against the current, swimming north.";
	var successToAll = "$n struggles against the current, swimming north.";
}
else if (dirToText(newRoom).toLowerCase() == "up") {
	var timer = 0.5;
	var swimMsg = "You grab onto a ladder on the side of the bridge, and begin to climb.";
	var successMessage = "\nYou hastily scale the ladder, and climb onto the bridge above.";
	var successToAll = "$n hastily scales the ladder, climbing onto the bridge above.";
}
actor.send(swimMsg);
act("$n starts swimming...",true,actor,null,null,constants.TO_ROOM);
setSval(actor, 14203, "riverSwim", 2);
actor.startTimer(timer);
var success = runTimer(actor);
if (success)
{
actor.send(successMessage);
act(successToAll, true, actor, null, null, constants.TO_ROOM);
setSval(actor, 14203, "riverSwim", 1);
if (actor.fighting)
actor.stopFighting();
actor.comm(dirToText(newRoom));
setSval(actor, 14203, "riverSwim", 0);
return;
}
else
{
actor.send("You give up trying to swim out of the area.");
return;
}

};

