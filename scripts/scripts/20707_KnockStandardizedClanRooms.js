var script20707 = function(self, actor, here, args, extra) {
	//KNOCK - used mostly for clan doors
	//Koradin - May 2010
	var vArgs = getArgList(args);
	if (!vArgs[1] || vArgs[2])
	{
		actor.send("What door do you wish to knock on?");
		_block;
		return
	}
	var room = actor.room;
	for (i = 0; i < 6; i++)
	{
		if (room.doorExists(i))
		{
			if (!str_cmp(room.doorName(i),vArgs[1]))
			{
				var target_room = room.direction(i);
				var iDir = i;
			}
		}
	}
	if (!target_room)
	{
		_noblock;
		return;
	}
	var firstdir = dirToText(iDir);
	if (firstdir == "up")
		firstdir = "above";
	else if (firstdir == "down")
		firstdir = "below";
	else
		firstdir = "to the "+firstdir;
	var dir = dirToText(revDir(iDir));
	if (dir == "up")
		dir = "above";
	else if (dir == "down")
		dir = "below";
	else
		dir = "the "+dir;
	var door = vArgs[1];
	_block;
	actor.send("You knock on the "+door+".");
	target_room.echo("Someone knocks on the "+door+" from the other side.");
	if (global.ClanRoom[room.vnum])
	{
		var clan = global.ClanRoom[room.vnum];
		var message = "leave";
	}
	else if (global.ClanRoom[target_room.vnum])
	{
		var clan = global.ClanRoom[target_room.vnum];
		var message = "enter";
	}
	for (var _autoKey in global.manors) {
		var mr = global.manors[_autoKey];
		if (actor.room.vnum == mr.outsideGateRoomVnum || actor.room.vnum == mr.insideGateRoomVnum)
			var manor = mr;
	}
	var manorEntry = false;
	if (manor) {
		if (actor.room.vnum == manor.outsideGateRoomVnum) {
			var message = "enter";
		}
		else {
			var message = "leave";
		}
		if (actor.id == manor.ownerUserId || arrContains(manor.allowedUsers, actor.id)) {
			manorEntry = true;
		}
		else
			manorEntry = false;
	}
	if (!actor.inClan(clan) && manorEntry == false)
		return;
	wait 2;
	if (actor.affectedBy(constants.AFF_NOQUIT)) {
		if (manor) {
			if (manor.ownerUserId == actor.id || arrContains(manor.allowedUsers, actor.id)) {
				actor.send("The guards won't let you through! Calm down first.");
			}
		}
		return;
	}
	if (actor.room != room)
		return;
	actor.send("The "+door+" opens just wide enough for you to "+message+".");
	act("The "+door+" "+firstdir+" opens just wide enough for $n to "+message+".",true,actor,null,null,constants.TO_ROOM);
	actor.moveToRoom(target_room);
	actor.comm("look");
	act("$n enters from "+dir+", shutting the "+door+" behind $m.",true,actor,null,null,constants.TO_ROOM);
	if (manor) {
		if (target_room.vnum != manor.insideGateRoomVnum)
			return;
		var manorTime = manorTimeToText(timeTillManorExpires(manor));
	    if (getManorOwnerById(manor.ownerUserId) == actor)
			var msg = bld+"Welcome home, "+actor.name+".\nUpkeep has been paid for "+manorTime+"."+nrm;
		else if (manor.ownerUserId > 0) {
			if (arrContains(manor.allowedUsers, actor.id))
				var colorM = bld;
			else
				var colorM = red;
			var msg = colorM+"You are entering the "+manor.name+" of "+getUserNameByUserId(manor.ownerUserId)+"."+nrm;
		}
		waitpulse 1;
		if (msg) {
			getCharCols(actor);
			actor.send(msg);
		}
	}
	
}