var script79 = function(self, actor, here, args, extra) {
	waitpulse 1;//Make sure it goes off after they've moved
	if( random(1,100) <= 10 )
	{//10% chance of toggling sneak
	    self.comm("sneak");
	}
	if( random(1,100) <= 80 ) 
	{//80% chance of hiding.
	    self.comm("hide");
	}
}