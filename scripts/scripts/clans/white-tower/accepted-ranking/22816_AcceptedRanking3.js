var script22816 = function(self, actor, here, args, extra) {
	let novice = getSval(self,22815,"novice");
	let targetRoom = getRoom(10575);
	let portalHere;
	let portalThere;

	// Purge any existing mobs
	targetRoom
		.getCharacters(function(characterInRoom) { return characterInRoom.isMob(); })
		.forEach(function(mob) {
			targetRoomPerson.moveToRoom(getRoom(25));
			targetRoomPerson.extract();
		});

	targetRoom.loadMob(2607);
	targetRoom.loadMob(2608);
	targetRoom.loadMob(2609);
	targetRoom.loadMob(2610);
	targetRoom.loadMob(2611);
	targetRoom.loadMob(2612);
	targetRoom.loadMob(2613);
	wait 6;

	self.say("Now that you are here " + novice.name + ", let us go to the testing chamber where the others are waiting.");
	wait 4;

	self.comm("embrace");
	wait 8;

	self.comm("emote weaves her hands before her, creating a shimmering portal.");
	portalHere = here.loadObj(10600);
	portalThere = targetRoom.loadObj(10600);
	targetRoom.echo("A shimmering portal appears.");
	wait 8;

	self.comm("emote motions for " + novice.name + " to lead the way.");
	wait 2;

	novice.send("You step through the portal.");
	act("$n steps into the portal.",false,novice,null,null,constants.TO_ROOM);
	novice.moveToRoom(targetRoom);
	act("$n steps through the portal.", false, novice, null, null, constants.TO_ROOM);
	novice.comm("look");
	wait 10;

	portalHere.extract();
	self.moveToRoom(targetRoom);
	act("$n steps through the portal.", false, self, null, null, constants.TO_ROOM);
	wait 6;

	portalThere.extract();
	targetRoom.echo("The portal closes in on itself.");
	wait 8;

	act("Sheriam pulls $n off to the side near a wooden chest.", true, novice, null, null, constants.TO_ROOM);
	actor.send("You join Sheriam and " + novice.name + ".");
	novice.send("Sheriam Sedai pulls you to the side.");
	wait 7;

	self.say("Novices are given three chances at this. You may refuse twice to enter, but at the third refusal, you are sent away from the Tower forever. That is how it is done usually, and you certainly have the right to refuse, but I do not think the Amyrlin Seat will be pleased with you if you do.");
	wait 6;

	self.say("Once you begin, you must go on to the end. Refuse at any point, and you will be put out of the Tower just as if you had refused to begin for the third time. Second. To seek, to strive, is to know danger.");
	wait 6;

	act("$N looks sternly at you.", false, novice, null, self, constants.TO_CHAR);
	act("$n looks sternly at $N.", false, self, null, novice, constants.TO_NOTVICT);
	wait 4;

	self.say("This is your last chance, " + novice.name + ". Refuse now, and it counts only as the first. You may still try twice more. If you accept now, there is no turning back. It is no shame to refuse. I could not do it, my first time. If you accept, hand over your dress.");
}