var script1020 = function(self, actor, here, args, extra) {
	_block;
	actor.send("The eyes of the bodyguards turn towards you.");
	waitpulse 2;
	actor.send("The Slumlord tells you, 'By the light! What do you think you're doing, fool?'");
	waitpulse 2;
	actor.send("A tough bodyguard smacks you on the head! All you see are stars!");
	waitpulse 2;
	actor.moveToRoom(getRoom (7124));
	actor.comm("look");
	waitpulse 2;
	actor.send("you wake up as you hit the ground with a thud, ouch!");
	actor.damage(15, actor);
	waitpulse 2;
	actor.send("A tough bodyguard says as he spits in your general direction and shuts a door, 'You're lucky we didn't kill you.'");
}