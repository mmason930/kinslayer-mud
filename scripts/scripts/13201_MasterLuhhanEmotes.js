var script13201 = function(self, actor, here, args, extra) {
	if( getSval(self, 23201, "numberOfStrikes") < random(4,6) ) {
		self.comm("emote pounds his hammer against the anvil with a tremendous force.");
		if( random(1,100) <= 25 ) {
		    wait 2;
			here.echo("Sparks erupt from the point of impact before quickly sputtering out.");
		}
	}
	else {
		self.comm("emote finishes up his current piece and sets it down on a nearby table.");
		setSval(self, 23201, "numberofStrikes", 0);
	}
}