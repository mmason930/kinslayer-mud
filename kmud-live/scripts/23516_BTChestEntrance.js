var script23516 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length >=2 && !str_cmp(vArgs[1],"irondoor") && actor.inClan(15) )
{
	wait 2;
	actor.send("The irondoor swings open and you step inside.");
	act("The irondoor swings open and $n steps inside.", false, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom( getRoom(23519) );
	wait 1;
	actor.comm("look");
}

};

