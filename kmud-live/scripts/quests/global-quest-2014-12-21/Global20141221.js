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
	this.pointsToWin = Math.ceil(this.spawnRoomVnums.length / 2);

	this.santaLoadFunction = function(santa)
	{
		santa.loadObj(2717);
		santa.loadObj(2718);
		santa.loadObj(2719);
	};

	this.santaMobs = [];
	this.setup();
}

GlobalDec2014Util.prototype.cleanup = function()
{
	var self = this;

	this.santaMobs.forEach(function(santa) {

		santa.followers.forEach(function(elf) {

			if(elf.vnum == self.followerMobVnum)
				elf.extract();
		});

		santa.extract();
	});
};

GlobalDec2014Util.prototype.setup = function()
{
	var self = this;
	this.spawnRoomVnums.forEach(function(roomVnum, index) {

		var room = getRoom(roomVnum);
		var mobVnum = self.leaderMobVnum;

		mudLog(constants.BRF, 100, "Room #" + roomVnum + ", Mob Vnum: " + mobVnum);

		var santa = room.loadMob(mobVnum);
		self.santaLoadFunction(santa);
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
				if(followers[index].inventory.length > 0) {
					followers[index].say("Well, I guess I won't be needing this anymore, so you can have it.");
					followers[index].comm("drop all")
					followers[index].comm("emote belches, 'HOOOO! HOOOO! HOOOO!'");
				}

				wait 2

				global.globalDec2014Util.points[actor.race]++;

				var pointsNow = global.globalDec2014Util.points[actor.race];

				getCharCols(actor);
				actor.send(bld + grn + "You have scored a point for your team!\r\n" + nrm);

				act("$n leads $N away.", false, self, null, followers[0], constants.TO_ROOM);
				followers[0].extract();

				var displayFunction = null;

				displayFunction = function(player)
				{
					getCharCols(player);

					var color = grn;

					if(actor.race == constants.RACE_TROLLOC)
						color = red;
					return bld + color + "\n" + " *** " + actor.name + " has scored a point! ***" + nrm;
				};
				gecho(displayFunction);

				if(pointsNow == global.globalDec2014Util.pointsToWin)
				{
					displayFunction = function(player)
					{
						getCharCols(player);

						var side = actor.race == constants.RACE_TROLLOC ? "Shadow" : "Light";
						var color = actor.race == constants.RACE_TROLLOC ? red : grn;

						return bld + color + "The " + side + " has won!" + nrm;
					};

					gecho(displayFunction);

					var playerToGiveTo = actor;

					var goodies = [];

					var bag = playerToGiveTo.loadObj(2061);

					bag.setRetoolSDesc("a decorated stocking");
					bag.setRetoolDesc("A decorated stocking has been left behind.");
					bag.setRetoolName("decorated stocking");
					bag.setRetoolExDesc("This decorated stocking is covered with a colorful ornate design. It appears to be the perfect container for a bunch of gifts!");

					bag.loadObj(917);
					bag.loadObj(713);
					bag.loadObj(1013);
					bag.loadObj(2716);
					bag.loadObj(2717);
					bag.loadObj(2718);
					bag.loadObj(2719);
					bag.loadObj(2720);
					bag.loadObj(2721);
					bag.loadObj(2722);
					bag.loadObj(2723);
					bag.loadObj(2724);

					goodies.push(bag);

					goodies.forEach(function(item) {

						act("$n has received $p!", true, playerToGiveTo, item, null, constants.TO_ROOM);
						act("You have received $p!", true, playerToGiveTo, item, null, constants.TO_CHAR);
					});
				}
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

if(global.globalDec2014Util)
{
	global.globalDec2014Util.cleanup();
	global.globalDec2014Util = new GlobalDec2014Util();
}