var script10678 = function(self, actor, here, args, extra)
{
	var rank = actor.getRank(7);
var mult = Math.floor(rank*2);
if ( actor.inClan(7) ) { 
     self.heal(mult);
}

};

