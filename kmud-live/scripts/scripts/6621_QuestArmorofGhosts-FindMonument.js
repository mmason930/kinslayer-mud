var script6621 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Armor of Ghosts
	/** Entry Trigger (6621) **/
	var qName = "Armor of Ghosts";
	if ( actor.quest(qName) == 5 ) {
		waitpulse 1;
		getCharCols(actor);
		actor.send(bld+cyn+"This statue marks the location of the lost armor of Hok'dar. The chamber must be somewhere below, and you somehow instinctively know that the means of entrance are just below the surface of the pool of water which has gathered at the statue's base."+nrm);
		waitpulse 28;
		actor.qval(qName,6);
		actor.send(bld+cyn+"When you are ready, "+nrm+"enter"+bld+cyn+" the chamber to recover the armor."+nrm);
	}
}