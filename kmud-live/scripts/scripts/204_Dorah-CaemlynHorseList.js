var script204 = function(self, actor, here, args, extra) {
	if( actor.race == self.race ) {
		_block;
		wait 1;
		self.say("I'm glad you asked! Here is a list of the currently available horses to buy.");
		wait 2;
		actor.send("You can buy a black warhorse for 15 silver coins.");
		actor.send("You can buy a black-and-white Domani razor for 12 silver coins.");
	}
}