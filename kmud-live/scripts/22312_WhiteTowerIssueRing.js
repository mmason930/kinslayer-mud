var script22312 = function(self, actor, here, args, extra)
{
	//Jenkin 
//October 2010
//CLAN: White Tower
//Edit as you see fit.
var clans = actor.findClans();
var towerClanUserIsIn = null;
for(var index = 0;index < clans.length;++index)
{
	if(clans[ index ] >= 17 && clans[ index ] <= 23)
	{
		towerClanUserIsIn = clans[index];
		break;
	}
}
if(towerClanUserIsIn != null)
{
     ring = 819;
     price = -1;
	 if (actor.qp(towerClanUserIsIn) < 1) {
		waitpulse 5;
		self.say("You don't have enough quest points.");
		return;
	 }
     waitpulse 5;
     self.loadObj(ring);      
     self.comm("frown");
     waitpulse 14;
     self.say("Never forget you are an ambassador of the Tower.");
     waitpulse 5;
     self.comm("give ring " + actor.name );
     self.comm("award " + actor.name + " " + towerClanUserIsIn + " " + price + " WT Ring Issued");
}



};

