var script22314 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//CLAN: White Tower
	//Edit as you see fit.
	if( actor.inClan(22) ) {
	     ring = 819;
	     price = -1;
	     waitpulse 5;
	     self.loadObj(ring);      
	     self.comm("frown");
	     waitpulse 14;
	     self.say("Never forget you are an ambassador of the Tower.");
	     waitpulse 5;
	     self.comm("give ring " + actor.name );
	     self.comm("award " + actor.name + " 22 " + price + " Green Ajah Ring Issued");
	}
	
}