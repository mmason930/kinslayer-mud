var script22345 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//CLAN: White Tower
	//Edit as you see fit.
	var rank = actor.getRank(16);
	if( actor.inClan(16) ) {
	     if ( rank < 3 ) {
	         self.say("You are not ready yet!");
	         return;
	     }
	     if ( rank >= 5 ) {
	          self.say("I am not in charge of you anymore, " + actor.name + ".");
	          return;
	     }
	     if ( rank >= 3 && rank < 5 ) {
	          var ring = 815;  //Accepted Ring
	          var price = -2;
	     }
	          waitpulse 5;
	     self.loadObj(ring);      
	     self.comm("frown");
	     waitpulse 14;
	     self.say("Never forget you are an ambassador of the Tower.");
	     waitpulse 5;
	     self.comm("give ring " + actor.name );
	     self.comm("award " + actor.name + " 16 " + price + " White Tower Ring Issued");
	}
	
}