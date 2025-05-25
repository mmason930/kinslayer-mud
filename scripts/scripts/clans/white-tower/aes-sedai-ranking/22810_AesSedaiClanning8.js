var script22810 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( vArgs.length >= 2 ) {
		_block;
		
		if( !str_cmp(vArgs[ 1 ],"star") ) {
			if( actor.position != constants.POS_STANDING ) {
				actor.send("You aren't even on your feet!");
				return;
			}
			wait 2;
			actor.send("You step into the center of the star.");
			var obj = here.loadObj(22811);
			obj.moveToChar(actor);
		}
		else {
			wait 2;
			actor.send("Remember what must be remembered.");
			return;
		}
	}
}