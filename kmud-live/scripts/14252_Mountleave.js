var script14252 = function(self, actor, here, args, extra)
{
	//mount disappear
if (self.room.zoneVnum != global.arenaZoneVnum || self.riddenBy) // only for arena
	return;
var timer = getSval(self,14252,"timer");
if (!timer || timer == 0) {
	setSval(self,14252,"timer",time());
	return;
}
var elapsedTime = Math.floor((time()-timer)/60);
if (elapsedTime > 5) {
	self.room.echo(self.name+" returns to its camp.");
	self.extract();
}

};

