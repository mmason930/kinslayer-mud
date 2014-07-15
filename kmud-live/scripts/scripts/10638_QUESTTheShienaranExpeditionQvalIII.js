var script10638 = function(self, actor, here, args, extra) {
	//Jenkin 
	//September 2010
	//Quest: Shienaran Expedition
	if (getSval(actor, 10638, "Q-TSE_assent_sval") != 2) {
		var qName = 'The Shienaran Expedition';
		if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp ) {
			 setSval(actor, 10638, "Q-TSE_assent_sval", 2);
			 waitpulse 5;
			 getCharCols(actor);
			 self.comm("emote rumbles, 'Ahem...it is...hmm...a rare thing to see your kind here.'");
			 waitpulse 21;
			 self.comm("emote rumbles, 'Forgive me my surprise. I have indeed seen a soldier in the Stedding.'");
			 waitpulse 21;
			 self.comm("emote rumbles, 'He came to us gravely wounded, and despite our efforts, he did not live.'");
			 waitpulse 21;
			 self.comm("emote walks over to a still, draped form, and lifts the sheet covering it."); 
			 getCharCols(actor);
			 actor.send(bld+cyn+"Upon seeing the body you spot the camoflauge of the Scouts."+nrm);
			 waitpulse 21;
			 self.comm("emote rumbles, 'I would be...hmm...sorry to be the bearer of bad news...'");
			 self.comm("emote rumbles, 'Is this the man you seek?'");
		}
	}
}