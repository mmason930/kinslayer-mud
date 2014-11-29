function Global2014Util()
{
	this.kingTurkeyVnum = 23006;
	this.followerTurkeyvnum = 23005;
	this.orbs = {};

	this.issueMobVnums = {};

	this.issueMobVnums[constants.RACE_HUMAN] = 1701;
	this.issueMobVnums[constants.RACE_TROLLOC] = 8006;

	this.issuedMobs = {};

	this.issuedMobs[constants.RACE_HUMAN] = [];
	this.issuedMobs[constants.RACE_TROLLOC] = [];

	this.maxIssuedMobs = 7;

	this.doorRoomVnums = [
		{vnum: 33928, dir: constants.EAST},
		{vnum: 33923, dir: constants.NORTH},
		{vnum: 33917, dir: constants.WEST},
		{vnum: 33913, dir: constants.SOUTH}
	];

	this.orbs[constants.RACE_HUMAN] = {

		vnum: 33001,
		openWorldRoomVnum: 100,
		closedWorldRoomVnum: 33904
	};

	this.orbs[constants.RACE_TROLLOC] = {

		vnum: 33000,
		openWorldRoomVnum: 8000,
		closedWorldRoomVnum: 33948
	};

	var switches = {};

	this.switches[33920] = {switchedOn: false, lastSwitched: new Date() } //SE
	this.switches[33926] = {switchedOn: false, lastSwitched: new Date() } //SW
	this.switches[33931] = {switchedOn: false, lastSwitched: new Date() } //NW
	this.switches[33921] = {switchedOn: false, lastSwitched: new Date() } //NE
}

Global2014Util.prototype.test = function()
{
};

Global2014Util.prototype.unlockDoors = function()
{
	this.doorRoomVnums.forEach(function(doorRoomVnum) {

		var room = getRoom(doorRoomVnum.vnum);
		var dir = doorRoomVnum.dir;

		room.unlockDoor(dir, true);
	});
};

Global2014Util.prototype.getNumberOfIssuedMobs = function(race)
{
	var counter = 0;

	this.issuedMobs[race].forEach(function(mob) {

		if(mob.isValid)
			++counter;
	});

	return counter;
};

Global2014Util.prototype.lockDoors = function()
{
	this.doorRoomVnums.forEach(function(doorRoomVnum) {

		var room = getRoom(doorRoomVnum.vnum);
		var dir = doorRoomVnum.dir;

		room.closeDoor(dir, true);
		room.lockDoor(dir, true);
	});
};

Global2014Util.prototype.getNumberOfSwitchesOn = function()
{
	var counter = 0;

	for(var switchVnum in this.switches)
	{
		if(this.switches[switchVnum].switchedOn)
			++counter;
	}

	return counter;
};

global.global2014 = new Global2014Util();

//Flip Switch
var script33002 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);

	if(vArgs.length < 2)
	{
		actor.send("Flip what?");
		return;
	}

	if(vArgs[0].toLowerCase() == "switch")
	{
		var roomVnum = self.room.vnum;

		var switchInfo = global.global2014.switches[roomVnum];

		switchInfo.switchedOn = !switchInfo.switchedOn;
		switchInfo.lastSwitched = new Date();

		var numberOn = global.global2014.getNumberOfSwitchesOn();

		act("$n flips the switch " + (switchInfo.switchedOn ? "on" : "off") + ".", false, actor, null, null, constants.TO_ROOM);
		act("You flips the switch " + (switchInfo.switchedOn ? "on" : "off") + ".", false, actor, null, null, constants.TO_CHAR);

		if(numberOn == 4)
		{
			global.global2014.unlockDoors();
		}
		else if(numberOn == 0)
		{
			global.global2014.lockDoors();
		}
	}
};

//Script for issuing mobs.
var script33001 = function(self, actor, here, args, extra) {

	var numberOfMobs = global.global2014.getNumberOfIssuedMobs(actor.race);

	mudLog(constants.BRF, 100, "Mobs: " + numberOfMobs);

	if(self.race != actor.race)
		return;

	if(numberOfMobs >= global.global2014.maxIssuedMobs)
	{
		self.say("There are already " + numberOfMobs + " units deployed. I cannot issue any more until some of them have fallen.");
	}
	else
	{
		var mob = here.loadMob(global.global2014.issueMobVnums[actor.race]);
		global.global2014.issuedMobs[actor.race].push(mob);

		mob.comm("follow " + actor.name);
	}
};

//Script for teleporting players in and out of the zone.
var script33000 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);
	_block;

	if(vArgs.length < 2)
	{
		actor.send("Touch what?");
		return;
	}

	if(vArgs[1].toLowerCase() == "orb")
	{
		var orbInfo = global.global2014.orbs[actor.race];

		if(orbInfo.vnum != self.vnum)
			return;

		var destinationRoomVnum = orbInfo.openWorldRoomVnum;

		if(destinationRoomVnum == actor.room.vnum)
			destinationRoomVnum = orbInfo.closedWorldRoomVnum;

		actor.moveToRoom(getRoom(destinationRoomVnum));

		actor.comm("look");
	}
};

//Random gobbling / emotes / fight events.
var script23005 = function(self, actor, here, args, extra) {

	var randomNumber = random(1, 100);

	if(self.vnum == 23006) {
		if (randomNumber < 10) {
			act("$n gobbles, 'Mang...'", false, self, null, null, constants.TO_ROOM);
		}
		else if (randomNumber < 30 && self.fighting) {
			act("$n points at $N and gobbles deeply, 'Die mang!", false, self, null, self.fighting, constants.TO_NOTVICT);
			act("$n points at you and gobbles deeply, 'Die mang!'", false, self, null, self.fighting, constants.TO_VICT);
			act("You point at $N and gobble deeply, 'Die mang!'", false, self, null, self.fighting, constants.TO_CHAR);

			waitpulse 5;

			act("$n roundhouse kicks $N in the face!", false, self, null, self.fighting, constants.TO_NOTVICT);
			act("$n roundhouse kicks you right in the face!", false, self, null, self.fighting, constants.TO_VICT);
			act("You roundhouse kick $N right in the face!", false, self, null, self.fighting, constants.TO_CHAR);

			if(!self.fighting.affectedBy(constants.AFF_DISORIENT)) {
				self.fighting.affect(constants.AFF_DISORIENT, 3, 0);
				self.fighting.send("You don't feel so great...");
				act("$n begins wobbling around incoherently.", true, self.fighting, null, null, constants.TO_ROOM);
			}
			self.fighting.damage(random(10, 15), self);

		}
	}
	else
	{
		if(randomNumber < 10)
			act("$n flutters its wings and gobbles, 'Mang!'", false, self, null, null, constants.TO_ROOM);
		else if(randomNumber < 30 && self.fighting)
		{
			act("$n flutters its wings violently.", false, self, null, null, constants.TO_ROOM);

			waitpulse 5;

			act("$n begins pecking at $N violently!", false, self, null, self.fighting, cosntants.TO_NOTVICT);
			act("$n begins pecking at you violently!", false, self, null, self.fighting, constants.TO_VICT);
			act("You begin pecking at $N violently!", false, self, null, self.fighting, constants.TO_CHAR);
		}
	}
};
