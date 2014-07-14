var script8071 = function(self, actor, here, args, extra)
{
	if( actor.inClan(27) )
{
	_block;
	wait 1;
	self.say("Do the will of the Gread Lord of the Dark!");
	wait 2;
	act("$n opens the dreaddoor and ushes you out.", false, self, null, null, constants.TO_ROOM);
	act("$n opens the dreaddoor and ushers $N out.", false, self, null, actor, constants.TO_NOTVICT);
	var otherRoom = getRoom(8070);
	actor.moveToRoom( otherRoom );
	actor.comm("look");
	act("The dreaddoor opens, and $n emerges into the room", false, actor, null, null, constants.TO_ROOM);
}

};

