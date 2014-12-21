if( !global ) {
	var global = {};
}

function GlobalDec2014Util()
{
	this.spawnRoomVnums = [5605, 18175, 1932, 6445, 4343, 924, 21445];
	this.leaderMobVnum = 23009;
	this.followerMobVnum = 23008;

	this.points = {};
	this.points[constants.RACE_TROLLOC] = 0;
	this.points[constants.RACE_HUMAN] = 0;

	this.santaMobs = [];
	this.setup();
}

GlobalDec2014Util.prototype.setup = function()
{
	return;
	var self = this;
	this.spawnRoomVnums.forEach(function(roomVnum, index) {

		var room = getRoom(roomVnum);
		var mobVnum = self.spawnRoomVnums;

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

var script23011 = function(self, actor, here, args, extra) {

	var followers = actor.followers;

	waitpulse 1;
	if(followers.length > 0)
	{
		for(var index = 0;index < followers.length;++index)
		{
			if(followers[index].vnum == 23009)
			{
				global.globalDec2014Util.points[actor.race]++;

				getCharCols(actor);
				actor.send(bld + grn + "You have scored a point for your team!\r\n" + nrm);

				act("$n leads $N away.", false, self, null, followers[0], constants.TO_ROOM);
				followers[0].extract();

				var displayFunction = function(player)
				{
					getCharCols(player);

					var color = grn;

					if(actor.race == constants.RACE_TROLLOC)
						color = red;
					return bld + color + "\n" + " *** " + actor.name + " has scored a point! ***\n" + nrm;
				};
				gecho(displayFunction);
			}
		}
	}
};

//Santa aggro.
var script23010 = function(self, actor, here, args, extra) {

	if(self.leader)
		return;
	if(self.fighting)
		return;

	var players = here.getMobs(-1);

	if(players.length != 0)
	{
		self.setFighting(players[0]);
	}
};

//Santa capture.
var script23009 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);

	_block;

	if(actor.fighting)
	{
		actor.send("You cannot do that while fighting!");
		return;
	}

	if(actor.position < constants.POS_FIGHTING)
	{
		actor.send("You must be standing in order to do that.");
		return;
	}

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

		santa.comm("follow " + actor.name);
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