var script10641 = function(self, actor, here, args, extra) {
	///Jenkin 
	//September 2010
	//Quest: Shienaran Expedition
	var vArgs = getArgList(args);
	var word = vArgs[0];
	var action = getSval(actor, 10638, "Q-TSE_assent_sval")
	var qName = 'The Shienaran Expedition';
	var speaker = self;
	var ch = actor;
	
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp && action == 2 ) {
		setSval(ch, 10638, "Q-TSE_assent_sval", 0)
		if ( word == "yes" ) {
			waitpulse 3;
			speaker.comm("sigh");
			waitpulse 10;
			speaker.comm("emote rumbles, 'Before the soldier died, he told me to give this to any...man...looking for him.'");
			speaker.loadObj(10631);
			waitpulse 14
			self.comm("give dispatch " + actor.name);
			waitpulse 10;
			ch.updateJournalTask(qName,2);
		}
		else if ( word == "no" ) {
			waitpulse 3;
			speaker.comm("emote rumbles, 'It is...ahem..a relief to hear you did not know him. Hmm.'");
			waitpulse 5;
			speaker.comm("emote pulls the shroud over the dead soldier's still form.");
		}
	}
	
	
}