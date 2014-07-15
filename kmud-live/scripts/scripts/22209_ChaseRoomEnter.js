var script22209 = function(self, actor, here, args, extra) {
	_block;
	if( actor.name == "Chase" ) {
		if( !actor.affectedBy(constants.AFF_NOQUIT) ) {
			wait 2;
			actor.send("You open the front door and step inside.");
			act("$n opens a door and enters the house to the north", true, actor, null, null, constants.TO_ROOM);
			actor.moveToRoom( getRoom(22801) );
			wait 2;
			actor.comm("look");
		}
		else {
			actor.send("You have no busines going in to rest when you're pumped for battle!");
		}
	}
	else {
		actor.send("This isn't your house!");
	}
}