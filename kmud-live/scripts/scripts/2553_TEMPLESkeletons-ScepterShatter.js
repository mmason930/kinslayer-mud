var script2553 = function(self, actor, here, args, extra) {
	getCharCols(actor);
	
	actor.sendRaw(red);
	act("The nerves in your hand explode as your fingers touch $p.", false, actor, self, null, constants.TO_CHAR);
	actor.sendRaw(nrm);
	act("$p disintigrates!", false, actor, self, null, constants.TO_CHAR);
	act("$p flashes brightly as $n touches it before shattering against the ground.", false, actor, self, null, constants.TO_ROOM);
	
	actor.damage(20);
	
	setTimeout(5, function() {
		
		here.echo("A panel on the wall falls to the floor, revealing three levers.");
	});
	
	self.extract();
}