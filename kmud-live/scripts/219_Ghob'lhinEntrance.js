var script219 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length > 1 && !str_cmp(vArgs[1],"goldendoor") && actor.inClan(11) && !actor.affectedBy(constants.AFF_NOQUIT) )
{
	waitpulse 5;
	actor.send("The goldendoor opens just wide enough for you to slip through.");
	act("$n enters the goldendoor, which shuts behind $m.", true, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom( getRoom(8159) );
	act("The goldendoor opens just wide enough for $n to enter.", true, actor, null, null, constants.TO_ROOM);
	actor.comm("look");
}

};

