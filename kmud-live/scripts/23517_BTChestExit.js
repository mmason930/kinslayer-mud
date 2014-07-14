var script23517 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length >= 2 && !str_cmp(vArgs[1],"irondoor") && actor.inClan(15) )
{
	wait 2;
	actor.send("The irondoor swings open and you leave west.");
	act("The irondoor swings open and $n leaves west.", false, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom( getRoom(23515) );
	actor.comm("look");
}

};

