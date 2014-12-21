if( !global ) {
	var global = {};
}

function GlobalDec2014Util()
{
	this.spawnRoomVnums = [18120];
	this.leaderMobVnums = [23009];
	this.followerMobVnum = 23008;

	this.santaMobs = [];
	this.setup();
}

GlobalDec2014Util.prototype.setup = function()
{
	var self = this;
	this.spawnRoomVnums.forEach(function(roomVnum, index) {

		var room = getRoom(roomVnum);
		var mobVnum = self.leaderMobVnums[index];

		var santa = room.loadMob(mobVnum);
		room.loadMob(self.followerMobVnum).comm("follow santa");
		room.loadMob(self.followerMobVnum).comm("follow santa");
		room.loadMob(self.followerMobVnum).comm("follow santa");
		room.loadMob(self.followerMobVnum).comm("follow santa");
		room.loadMob(self.followerMobVnum).comm("follow santa");
		room.loadMob(self.followerMobVnum).comm("follow santa");

		self.santaMobs.push(santa);

		santa.comm("group all");

		mudLog(constants.BRF, 100, "Loading `" + santa.name + "` in room #" + roomVnum);
	});
};

var script23009 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);

	_block;

	if(vArgs.length < 2)
	{
		actor.send("Capture who?");
		return;
	}

	var nameArgument = vArgs[1];
	var santa = getCharInListVis(actor, nameArgument, here.people);

	if(santa == null)
	{
		actor.send("You don't see them here!");
		return;
	}

	if(santa.vnum != self.vnum)
	{
		actor.send("You can't capture " + santa.name + "!");
		return;
	}

	if(santa.followers.length != 0)
	{
		var elf = santa.followers[0];

		act("$n kicks $N in the face with the point of $s boot!", false, elf, null, actor, constants.TO_NOTVICT);
		act("$n kicks you in the face with the point of $s boot!", false, elf, null, actor, constants.TO_VICT);
		act("You kick $N in the face with the point of your boot!", false, elf, null, actor, constants.TO_CHAR);

		if(!actor.affectedBy(constants.AFF_DISORIENT)) {
			actor.affect(constants.AFF_DISORIENT, 3, 0);
			actor.send("You don't feel so great...");
			act("$n begins wobbling around incoherently.", true, actor, null, null, constants.TO_ROOM);
		}
		return;
	}

	actor.startTimer(10);
	var success = runTimer(actor);

	if(success)
	{
		act("You tackle $N to the ground and force $m into submission!", false, actor, null, santa, constants.TO_CHAR);
		act("$n tackles $N to the ground and forces $m into submission!", false, actor, null, santa, constants.TO_NOTVICT);
		act("$n tackles you to the ground and forces you into submission!", false, actor, null, santa, constants.TO_VICT);

		santa.act("follow " + actor.name);
	}
	else if(santa.isValid)
	{
		act("You give up your attempt to capture $N.", true, actor, null, santa, constants.TO_CHAR);
		act("$n gives up $s attempt to capture $N.", true, actor, null, santa, constants.TO_NOTVICT);
		act("$n gives up $s attempt to capture you.", true, actor, null, santa, constants.TO_VICT);
		return;
	}
};

global.globalDec2014Util = new GlobalDec2014Util();