var script20797 = function(self, actor, here, args, extra)
{
	self.room.loadMob(202);
self.room.echo("Old array:");
for each (var player in global.arenaPlayers)
	self.room.echo(player.name);
for (i=0;i<global.arenaPlayers.length;i++) {
	if (global.arenaPlayers[i] == self) {
		global.arenaPlayers.splice(i,1,self.room.people[0]);
		break;
	}
}
self.room.echo("New array:");
for each (var player in global.arenaPlayers)
	self.room.echo(player.name);

};

