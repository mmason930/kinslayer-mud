var script23513 = function(self, actor, here, args, extra) {
	//dismiss in BT clan room
	//Koradin - May 2010
	_block;
	if (!actor.inClan(constants.CLAN_BLACK_TOWER))
	{
	self.say("Hah! Over my dead body!");
	self.comm("kill " + actor.name);
	return;
	}
	self.comm("nod " +actor.name);
	wait 2;
	actor.send("The door opens, and you leave the meeting chamber.");
	act("$n leaves the meeting chamber, closing the door behind $m.",true,actor,null,null,constants.TO_ROOM);
	actor.moveToRoom(getRoom(23513));
	actor.comm("look");
	act("$n arrives from the north, closing the door behind $m.",true,actor,null,null,constants.TO_ROOM);
}