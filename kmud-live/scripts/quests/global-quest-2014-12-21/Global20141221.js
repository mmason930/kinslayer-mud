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
		actor.send("Santa has followers. Do something evil.");
		return;
	}

	actor.startTimer(10);
	var success = runTimer(actor);

	if(success)
	{
		actor.send("Timer succeeded.");
		return;
	}
	else
	{
		actor.send("Timer failed.");
		return;
	}
};

global.globalDec2014Util = new GlobalDec2014Util();