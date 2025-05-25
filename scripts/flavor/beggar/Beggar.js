var script177 = function(self, actor, here, args, extra) {
	self.comm("Alms for the poor! Alms for the poor!");
};

var script178 = function(self, actor, here, args, extra) {
	if( actor.vnum === -1 && random(1,100) <= 15 && self.canSee(actor) ) {
		llmResponse({
			model: "gpt-4.1-nano",
			prompt: "You're an NPC in a game. Your name is: " + self.name + ", you are in a room named " + here.name
				+ ". A player named " + actor.name + " just entered your room. Provide me the dialog you'd say."
				+ " Provide only the dialog, nothing else, and not in quotes. Max 1-2 sentence. Include begging or"
			    + " some kind of pathetic plead",
			onSuccess: function (result) {
				self.say(result.response);
			}
		});
		//
		//	self.say("Please spare me a few coins, " + actor.name + "!");
		//}
	}

};

var script179 = function(self, actor, here, args, extra) {
	if( extra.amount > 0 ) {
		wait 1;
		self.say("Thank you, " + actor.name + ". Thank you so much!");
		wait 2;
		self.comm("emote slips the coins into an old brown sack.");
		wait 1;
		self.comm("emote smiles, producing a mouthful of rotting teeth.");
	}
};
