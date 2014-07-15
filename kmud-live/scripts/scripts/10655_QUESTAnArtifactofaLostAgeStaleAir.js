var script10655 = function(self, actor, here, args, extra) {
	var qName = 'An Artifact of a Lost Age'
	var vict = getRandomPersonInRoom(here);
	if ( vict != undefined && vict.questInProgress(qName)) {
	     vict.send("You inhale a mouthful of stale air!");
	     waitpulse 7;
	     vict.comm("cough");
	     vict.damage( random(1, 5), self );
	}
}