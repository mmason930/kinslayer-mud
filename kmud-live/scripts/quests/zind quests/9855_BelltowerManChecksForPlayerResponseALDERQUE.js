var script9855 = function(self, actor, here, args, extra) {
	setSval(self,9855,"Wait",getSval(self,9855,"Wait")+1);
	// here.echo("Wait sval: "+getSval(self,9855,"Wait"));
	if ( getSval(self,9855,"Wait") == 2 ) {
		// Don't unlock the next quest if they don't respond
		actor = getSval(self,9854,"Actor");

		actor.qval("ZIND_UNLOCK_SECOND_QUEST", 0);

		getCharCols(actor,constants.CL_OFF);
		actor.send(capFirstLetter(self.name)+" cuts loose a small pouch and disappears over the balcony railing before the clinking of coin is heard as the pouch hits the ground.");
		self.moveToRoom(getRoom(501));
		waitpulse 14;
		setSval(actor,9854,"TOWER CHECK",true);
		actor.updateJournalTask("Just Think...",0);
		self.detach(9855);
		self.extract();
	}
	
}