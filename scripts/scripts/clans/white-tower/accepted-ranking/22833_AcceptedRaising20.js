var script22833 = function(self, actor, here, args, extra) {
	//Only the novice this Sheriam is currently testing may drive the closing stage. Set by
	//22815 when a sister presents her; absent on any Sheriam not running a test.
	let expectedNovice = getSval(self, 22815, "novice");

	if( !expectedNovice || str_cmp(expectedNovice.name, actor.name) ) {
		return;
	}

	//Set by 22832 once she has actually stepped out of the third arch.
	if( !getSval(actor, 22833, "raised") ) {
		return;
	}

	setSval(actor, 22833, "raised", 0);

	wait 30;

	let novice = actor;
	self.comm("give all " + novice.name);
	wait 6;

	here.echo("Sheriam says, 'Let us return to the Hall.'");
	wait 2;

	self.comm("embrace");
	wait 6;

	let targetRoom = getRoom(22304);
	let portalHere;
	let portalThere;
	wait 4;

	portalHere = here.loadObj(10600);
	portalThere = targetRoom.loadObj(10600);
	targetRoom.echo("A shimmering portal appears.");
	wait 8;

	self.comm("emote motions for " + novice.name + " to lead the way.");
	wait 8;

	novice.send("You step through the portal.");
	act("$n steps into the portal.",false,novice,null,null,constants.TO_ROOM);

	novice.moveToRoom(targetRoom);
	act("$n steps through the portal.", false, novice, null, null, constants.TO_ROOM);

	novice.comm("look");
	wait 6;

	portalHere.extract();
	self.moveToRoom(targetRoom);
	act("$n steps through the portal.", false, self, null, null, constants.TO_ROOM);
	wait 6;

	portalThere.extract();
	targetRoom.echo("The portal closes in on itself.");
	wait 8;

	if(novice.getRank(constants.CLAN_WHITE_TOWER) < 3) {
		self.comm("rank " + novice.name + " " + constants.CLAN_WHITE_TOWER);
	}

	gecho("**" + novice.name + " has been promoted to an Accepted!");
	wait 4;

	self.comm("shout " + novice.name + " has been raised to Accepted!");
	wait 8;

	self.say("Go now, and learn as the Mother has commanded.");
	wait 12;

	targetRoom = getRoom(10575);
	self.moveToRoom(targetRoom);
	self.room.purgeItems(false);
}