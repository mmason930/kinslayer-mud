if( !global ) {
	var global = {};
}

function GlobalDec2014Util()
{
	this.spawnRoomVnums = [18120];
	this.leaderMobVunms = [23009];
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

global.globalDec2014Util = new GlobalDec2014Util();