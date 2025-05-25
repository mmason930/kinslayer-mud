var script4330 = function(self, actor, here, args, extra) {
	//Alder
	//June 2011
	/** Loads Mashadar at 9 PM **/
	if ( getHour() === 21 ) {
		here.zecho("An ominous air permeates the city...");
		waitpulse 7;
		self.loadMob(920).victims = [];
	}
}