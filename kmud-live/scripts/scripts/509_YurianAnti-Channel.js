var script509 = function(self, actor, here, args, extra) {
	_block;
	var rand = random(1,7);
	if( rand > 1 )
		return;
	actor.send("Ok.");
	wait 1;
	act("$n directs $s attack at you, interrupting your channeling!", false, self, null, actor, constants.TO_VICT);
	actor.damage(20,self);
	actor.send("Ouch!");
	act("$n directs $s attack at $N!", true, self, null, actor, constants.TO_NOTVICT);
}