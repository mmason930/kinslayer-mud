var script8153 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args.toLowerCase());
	if( vArgs.length >= 2 && vArgs[1] === "barreddoor" && actor.inClan(constants.CLAN_DHJINNEN) && !actor.affectedBy(constants.AFF_NOQUIT)) {
		wait 2;
		act("The barreddoor to the east creaks open just long enough to let $n in.", false, actor, null, null, constants.TO_ROOM);
		actor.send("The barreddoor to the east opens and you enter the room.");
		actor.moveToRoom( getRoom(8156) );
		actor.comm("look");
	}
}