var script2305 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if(vArgs.length >= 2 && !str_cmp(vArgs[1], "stump")) {
	actor.send("You knock on the stump.");
	act("$n knocks on a stump.", true, actor, null, null, constants.TO_ROOM);
	_block;
	
	wait 2;
	if( actor.inClan(constants.CLAN_WOLFBROTHER) ) {
		actor.send("An ancient wooden carving slides open before you. You step out.");
		act("An anciend wooden carving slides open before $n. $e steps out.", true, actor, null, null, constants.TO_ROOM);
		actor.moveToRoom(getRoom(9375));
		actor.comm("look");
	}
}

};

