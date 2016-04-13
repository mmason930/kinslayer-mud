var script22300 = function(self, actor, here, args, extra) {
	var dmg;
	dmg = actor.hps * 0.05;
	here.echoaround(actor, actor.name + " is hps with an invisible force!");
	actor.send("You are hit with an invisible object square in the chest!");
	actor.damage(dmg);
}