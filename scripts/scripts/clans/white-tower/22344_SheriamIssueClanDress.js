var script22344 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//CLAN: White Tower
	//Edit as you see fit.
	var rank = actor.getRank(16);
	if( actor.inClan(16) && rank < 5) {
	     if ( rank <= 2 ) {
	          var dress = 22305; //Novice Dress
	     }
	     if ( rank >= 3 && rank <= 4 ) {
	          var dress = 2019;  //Accepted Dress
	     }
	     waitpulse 5;
	     self.loadObj(dress);      
	     self.comm("frown");
	     waitpulse 14;
	     self.say("Never forget that you are an ambassador of the Tower.");
	     waitpulse 5;
	     self.comm("give dress " + actor.name );
	     self.comm("award " + actor.name + " 16 -1 White Tower Dress Issued");
	}
	
	
}