var script10658 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//Quest: An Artifact of a Lost Age
	var qName = 'An Artifact of a Lost Age';
	if( actor.position != constants.POS_RESTING && actor.questInProgress(qName) ) {
	     _block;
	     actor.send("As you enter the platform, something feels wrong.");
	     waitpulse 14;
	     actor.send("You feel the room lurch and spin...");
	     waitpulse 14;
	     actor.moveToRoom(getRoom(20879));
	     actor.comm("look");
	     waitpulse 5;
	     actor.send("The last grain of sand hits the bottom of the hourglass.");
	}
}