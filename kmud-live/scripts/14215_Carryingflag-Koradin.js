var script14215 = function(self, actor, here, args, extra)
{
	var newRoom = extra.direction;
if (getSval(actor, 14203, "flagWalk") == 1)
{
	_noblock;
	return;
}
_block;
actor.send("Hefting the flag makes for slow going...");
actor.sps = 0;
actor.shadowPoints = 0;
act("$n starts making $s way "+dirToText(newRoom)+".",true,actor,null,null,constants.TO_ROOM);
setSval(actor, 14203, "flagWalk", 2);
actor.startTimer(2);
var success = runTimer(actor);
if (!success || actor.fighting)
{
	actor.send("You give up trying to move.");
	setSval(actor, 14203, "flagWalk", 0);
	return;
}	
else
{
	actor.send("\nYou tighten your grip on the flag, and head "+dirToText(newRoom)+".");
	setSval(actor, 14203, "flagWalk", 1);
	if (actor.fighting)
		actor.stopFighting();
	actor.comm(dirToText(newRoom));
	setSval(actor, 14203, "flagWalk", 0);
	return;
}

};

