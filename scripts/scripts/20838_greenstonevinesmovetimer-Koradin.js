var script20838 = function(self, actor, here, args, extra) {
	var newRoom = extra.direction;
	if (getSval(actor, 20838, "vines") == 1 || actor.room.zoneVnum == global.arenaZoneVnum)
	{
	_noblock;
	return;
	}
	_block;
	var timer = random(0,50) / 10;
	actor.send("You start fighting your way through the thick mess of vines...");
	act("$n starts fighting $s way through the thick forest of vines...",true,actor,null,null,constants.TO_ROOM);
	setSval(actor, 20838, "vines", 2);
	actor.startTimer(timer);
	var success = runTimer(actor);
	if (success)
	{
	actor.send("You finally make it through and keep heading where you were going.");
	setSval(actor, 20838, "vines", 1);
	if (actor.fighting)
	actor.stopFighting();
	actor.comm(dirToText(newRoom));
	setSval(actor, 20838, "vines", 0);
	return;
	}
	else
	{
	actor.send("The vines hold fast, and you give up the attempt.");
	return;
	}
}