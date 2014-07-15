var script13255 = function(self, actor, here, args, extra) {
	if( actor.quest( "Special Delivery" ) == 2 && extra.obj.vnum == 1608 ) {
		waitpulse 1;
		extra.obj.extract();
		wait 1;
		self.tell(actor, "Thanks, " + actor.name + "!!");
		wait 3;
		self.tell(actor, "I'll let Bran know what a help you've been!");
		wait 3;
		actor.questSend("Return to Bran al'Vere at the Winespring Inn to complete the quest.");
		wait 3;
		actor.questSend("Once you've returned to him, simply type, 'quest complete <questNum>'");
		actor.updateJournalTask("Special Delivery", 0);
	}
	
}