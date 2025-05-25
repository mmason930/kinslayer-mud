var script14250 = function(self, actor, here, args, extra) {
	if (self.vnum < 14208)
		var rVnum = 14201;
	else if (self.vnum < 14216)
		var rVnum = 14299;
	else if (self.vnum < 14224)
		var rVnum = 14415;
	else
		var rVnum = 14499;
	self.room.zecho("Your blood freezes as you hear "+self.name+"'s death cry.");
	self.room.echo(self.name+" is dead! R.I.P.");
	self.moveToRoom(getRoom(rVnum));
	self.room.loadMob(self.vnum);
	var player = self.room.people[0];
	self.moveToRoom(getRoom(20899));
	var newTeam = self.arenaTeam;
	player.arenaTeam = newTeam;
	self.arenaTeam = null;
	setSval(player,14210,"arenaQueue",getSval(self,14210,"arenaQueue"));
	setSval(player,20860,"killCount",getSval(self,20860,"killCount"));
	setSval(player,20860,"deathCount",getSval(self,20860,"deathCount"));
	setSval(player,20860,"assistCount",getSval(self,20860,"assistCount"));
	setSval(player,20799,"khTime",getSval(self,20799,"khTime"));
	setSval(player,20930,"flagCaps",getSval(self,20930,"flagCaps"));
	setSval(player,20930,"flagRets",getSval(self,20930,"flagReps"));
	setSval(player,20860,"smobKills",getSval(self,20860,"smobKills"));
	setSval(player,20860,"doubleKills",getSval(self,20860,"doubleKills"));
	setSval(player,20860,"tripleKills",getSval(self,20860,"tripleKills"));
	setSval(player,20860,"arenaScore",getSval(self,20860,"arenaScore"));
	setSval(player,20860,"respawnTime",global.arenaCurrentGame.respawn);
	for (i=0;i<global.arenaPlayers.length;i++) {
		if (global.arenaPlayers[i] == self) {
			global.arenaPlayers.splice(i,1,player);
			break;
		}
	}
	
}