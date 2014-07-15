var script449 = function(self, actor, here, args, extra) {
	if( random(1,100) <= 40 ) {
		waitpulse 10
		actor.send("The waters begin to surge around your legs, pulling you down!");
		act("$n is pulled by the water and starts to sink!", false, actor, null, null, constants.TO_ROOM);
		waitpulse 20
		if( actor.room == here ) {
			actor.send("As you are pulled down, water forces its way down your throat as you try to call out!");
			act("$n is fully submerged, leaving nothing but gurgles of bubbles.", false, actor, null, null, constants.TO_ROOM);
			actor.damage(1000);
		}
	}
}