var script21404 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if(vArgs.length >= 2 && !str_cmp(vArgs[1], "stump")) {
	actor.send("You knock on the stump.");
	act("$n knocks on a stump.", true, actor, null, null, constants.TO_ROOM);
	_block;
	
	wait 2;
	if( actor.inClan(constants.CLAN_WOLFBROTHER) ) {
	
		if(actor.affectedBy(constants.AFF_NOQUIT)) {
			actor.send("Your heart is pounding too hard right now!");
			return;
		}
		else {
			actor.send("A hidden passage opens before you. You step in.");
			act("A hidden passage opens before $n. $e steps in.", true, actor, null, null, constants.TO_ROOM);
			actor.moveToRoom(getRoom(2305));
			actor.comm("look");
		}
	}
}

};

