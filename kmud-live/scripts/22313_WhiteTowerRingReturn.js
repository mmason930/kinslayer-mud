var script22313 = function(self, actor, here, args, extra)
{
	//Jenkin 
//October 2010
//CLAN: White Tower
//Edit as you see fit.
var rank = actor.getRank(16);
var item = extra.obj;
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
	if(item.vnum == 819)
	{
		item.moveToRoom(getRoom(20899));
		self.say("We can always use more of these.");
		waitpulse 5;
		self.comm("award " + actor.name + " " + towerClanUserIsIn + " 1 WT Ring Turned In");
	}
	else
	{
		waitpulse 5;
		self.say("Not interested.");
		waitpulse 5;
		self.comm("give ring " + actor.name);
	}
}



};

