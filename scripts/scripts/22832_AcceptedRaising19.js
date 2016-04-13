var script22832 = function(self, actor, here, args, extra) {
	_block;
	wait 4;
	var novice = actor;
	novice.send("The Light burns you to the bone, searing you to the soul. There seems to be nothing. Nothing but the light. And the pain. Finally, you step out of the Light, back into the domed chamber.");
	act("$N steps out of the arch for the final time.", false, novice, null, novice, constants.TO_ROOM);
	here.loadMob(1151);
	_block;
	actor.comm("periodone");
	wait 6;
	novice.send("You kneel before the Amyrlin, and she slowly empties the last chalice over your head. 'You are washed clean of " + novice.name + ". You are washed clean of all ties that bind you to the world. You come to us washed clean, in heart and soul. You are " + novice.name + ", Accepted of the White Tower.");
	act(novice.name + " kneels before the Amyrlin, who slowly empties the last chalice over her head. 'You are washed clean of '" + novice.name + "'You are washed clean of all ties that bind you to the world. You come to us washed clean, in heart and soul. You are ' + novice.name + ', Accepted of the White Tower.", false, novice, null, null, constants.TO_ROOM);
	wait 8;
	novice.send("The last drop splashes onto your hair. 'You are sealed to us, now.' The Amyrlin says. She thrusts the chalice at the Red sister and produces a ring in the shape of a serpent biting its own tail. She places it on the third finger of your left hand.");
	act("The last drop splashes onto " + novice.name + "s hair. 'You are sealed to us, now.' The Amyrlin says, before handing the chalice over and producing a Great Serpent ring that she slips onto " + novice.name + "'s left hand.", false, novice, null, null, constants.TO_ROOM);
	actor.loadObj(815);
	wait 3;
	actor.comm("wear ring");
	wait 8;
	novice.send("The Amyrlin pulls you to your feet. 'Welcome Daughter,' she says as she kisses both of your cheeks. 'Welcome. It is time for you to learn to protect yourself.'");
	act("The Amyrlin pulls " + novice.name + " to her feet, saying 'Welcome Daughter' before kissing both of her cheeks. 'Welcome. It is time for you to learn to protect yourself.", false, novice, null, null, constants.TO_NOTVICT);
	wait 30;
	here.purgeItems(false);
}