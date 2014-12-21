if( !global ) {
	var global = {};
}

function GlobalNov2014Util()
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

	this.switches = {};

	this.switches[33920] = {switchedOn: false, lastSwitched: new Date() } //SE
	this.switches[33926] = {switchedOn: false, lastSwitched: new Date() } //SW
	this.switches[33931] = {switchedOn: false, lastSwitched: new Date() } //NW
	this.switches[33921] = {switchedOn: false, lastSwitched: new Date() } //NE

	this.timeOfLastGlobal = null;
}

GlobalNov2014Util.prototype.test = function()
{
};

GlobalNov2014Util.prototype.unlockDoors = function()
{
	this.doorRoomVnums.forEach(function(doorRoomVnum) {

		var room = getRoom(doorRoomVnum.vnum);
		var dir = doorRoomVnum.dir;

		room.unlockDoor(dir, true);
		room.openDoor(dir, true);
	});
};

GlobalNov2014Util.prototype.getNumberOfIssuedMobs = function(race)
{
	var counter = 0;

	this.issuedMobs[race].forEach(function(mob) {

		if(mob.isValid)
			++counter;
	});

	return counter;
};

GlobalNov2014Util.prototype.lockDoors = function()
{
	this.doorRoomVnums.forEach(function(doorRoomVnum) {

		var room = getRoom(doorRoomVnum.vnum);
		var dir = doorRoomVnum.dir;

		room.closeDoor(dir, true);
		room.lockDoor(dir, true);
	});
};

GlobalNov2014Util.prototype.getNumberOfSwitchesOn = function()
{
	var counter = 0;

	for(var switchVnum in this.switches)
	{
		if(this.switches[switchVnum].switchedOn)
			++counter;
	}

	return counter;
};

//global.globalNov2014 = new GlobalNov2014Util();

//Flip Switch
var script33002 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);

	_block;
	if(vArgs.length < 2)
	{
		actor.send("Flip what?");
		return;
	}

	if(vArgs[1].toLowerCase() == "switch")
	{
		var roomVnum = self.room.vnum;

		var switchInfo = global.globalNov2014.switches[roomVnum];

		switchInfo.switchedOn = !switchInfo.switchedOn;
		switchInfo.lastSwitched = new Date();

		var numberOn = global.globalNov2014.getNumberOfSwitchesOn();

		act("$n flips the switch " + (switchInfo.switchedOn ? "on" : "off") + ".", false, actor, null, null, constants.TO_ROOM);
		act("You flips the switch " + (switchInfo.switchedOn ? "on" : "off") + ".", false, actor, null, null, constants.TO_CHAR);

		if(numberOn == 4)
		{
			global.globalNov2014.unlockDoors();

			self.room.zecho("You hear the loud bang of opening doors nearby.");
		}
		else if(numberOn == 0)
		{
			global.globalNov2014.lockDoors();

			self.room.zecho("You hear the loud bang of closing doors nearby.");
		}
	}
};

//Script for issuing mobs.
var script33001 = function(self, actor, here, args, extra) {

	var numberOfMobs = global.globalNov2014.getNumberOfIssuedMobs(actor.race);

	mudLog(constants.BRF, 100, "Mobs: " + numberOfMobs);

	if(self.race != actor.race)
		return;

	if(numberOfMobs >= global.globalNov2014.maxIssuedMobs)
	{
		self.say("There are already " + numberOfMobs + " units deployed. I cannot issue any more until some of them have fallen.");
	}
	else
	{
		var mob = here.loadMob(global.globalNov2014.issueMobVnums[actor.race]);
		global.globalNov2014.issuedMobs[actor.race].push(mob);

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
		var orbInfo = global.globalNov2014.orbs[actor.race];

		if(orbInfo.vnum != self.vnum) {

			actor.send("A huge jolt of energy throws you back as you try to touch " + self.name + "!");
			act("$n jumps back suddenly as $e tries to touch $p!", false, actor, self, null, constants.TO_ROOM);
			actor.damage(5);
			return;
		}

		if(actor.affectedBy(constants.AFF_NOQUIT))
		{
			actor.send("You touch " + self.name + ", but it seems to do nothing.");
			act("$n touches $p, but it appears to do nothing.", false, actor, self, null, constants.TO_ROOM);
			return;
		}

		var followers = actor.followers.filter(function(follower) {
			return follower.room.vnum == actor.room.vnum;
		});

		var performPort = function(character)
		{
			character.send("A bright white light consumes your vision.");
			act("$n shimmers into a bright white light and disappears!", false, character, null, null, constants.TO_ROOM);

			var destinationRoomVnum = orbInfo.openWorldRoomVnum;

			if(destinationRoomVnum == character.room.vnum)
				destinationRoomVnum = orbInfo.closedWorldRoomVnum;

			character.moveToRoom(getRoom(destinationRoomVnum));

			character.comm("look");

			act("A huge flash of white light appears, slowly revealing $n!", false, character, null, null, constants.TO_ROOM);
		};

		performPort(actor);

		followers.forEach(function(follower) {

			if(!follower.affectedBy(constants.AFF_NOQUIT))
				performPort(follower);
		});
	}
};

var script33003 = function(self, actor, here, args, extra) {

	var itemVnum = self.race == constants.RACE_TROLLOC ? 2115 : 2104;
	var itemsInRoom = 10;
	var itemCount = here.items.filter(function(item) { return item.vnum == itemVnum }).length;
	var itemsToLoad = itemsInRoom - itemCount;

	while(itemsToLoad-- > 0)
	{
		self.loadObj(itemVnum);
		self.comm("drop meat");
	}
};

//
//Random gobbling / emotes / fight events.
var script23005 = function(self, actor, here, args, extra) {

	var randomNumber = random(1, 100);

	if(self.vnum == 23006) {

		if( global.globalNov2014.timeOfLastGlobal == null || (new Date().getTime() - global.globalNov2014.timeOfLastGlobal.getTime()) >= (1000 * 60 * 60) )
		{
			global.globalNov2014.timeOfLastGlobal = new Date();

			//self.comm("global Show up on Sunday, December 7th at 1:00PM so you can try and fail at killing me.");
		}

		if (randomNumber < 10) {
			act("$n gobbles, 'Come on, mang...'", false, self, null, null, constants.TO_ROOM);
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
			act("$n flutters its wings and gobbles, 'Take this, mang!'", false, self, null, null, constants.TO_ROOM);
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
