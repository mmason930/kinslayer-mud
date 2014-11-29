function Global2014Util()
{
	this.kingTurkeyVnum = 23006;
	this.followerTurkeyvnum = 23005;
	this.orbs = {};
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

global.global2014 = new Global2014Util();

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
