var script9375 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if(vArgs[1] == 'rose'){
		var chance = random(1, 2);
		actor.startTimer(2.0);
		var success = runTimer(actor);
		if(success) {
			if( chance == 1 ) {
				actor.send("You successfully pick a rose from amongst the thorns!");
				act("$n reaches $s hand into the rosebush and plucks out a large rose!", true,actor, null, null, constants.TO_ROOM);
				actor.loadObj(9376);
				return;
			}
			else {
				actor.send("Ouch! you prick your hand on a large thorn!");
				act("$n reaches $s hand into the rosebush but recoils in pain as $e pricks $s hand on a thorn!", true,actor, null, null, constants.TO_ROOM);
				actor.damage(10);
				return;
			}
		}
		else {}
	}
			
}