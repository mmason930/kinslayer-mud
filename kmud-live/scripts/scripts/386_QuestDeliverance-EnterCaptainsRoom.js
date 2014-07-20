var script386 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Deliverance
	/** Send Fade and Mobs to Player When Finding Captain **/
	var qName =  "Deliverance";
	if ( actor.quest(qName) && actor.questTaskProgress(qName)[2].comp && !actor.questTaskProgress(qName)[3].comp ) {
		waitpulse 1;
		var captain = self;
		var fade = getSval(actor,385,"DELIVERANCE_FADE");
		fade.moveToRoom(here);
		for (var _autoKey in fade.followers) {
			var follower = fade.followers[_autoKey];
			follower.moveToRoom(here);
		}
		getCharCols(actor);
		actor.send(bld+cyn+"The fade and his troops have arrived to assist you."+nrm);
		setSval(captain,386,"DELIVERANCE_POINTMAN",actor);
		fade.comm("kill TARGET_QUEST_DELIVERANCE");
		actor.updateJournalTask(qName,2);
		fade.say("We have him now "+actor.name+"!");
		waitpulse 5;
		here.echo(capFirstLetter(self.name)+" shrieks and hisses as he swings his sword.");
	}
		
}