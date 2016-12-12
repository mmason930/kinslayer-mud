var script18602 = function(self, actor, here, args, extra) {
	// Displays the current cost to hire a mercenary when they hear "hire" or "cost"
	// Rhollor - December 2009
	var price = MercUtil.getPrice(self);
	var owner = MercUtil.getOwner(self);

	if(owner == actor) {
		waitpulse 5;
		self.tell(actor, "You've already hired me, moron!");

		waitpulse 5;
		self.tell(actor, "Shut up and get a move on unless you wanna pay me some more coin...");
	}
	else {
		if(owner != 0 && owner.isValid) {
			waitpulse 5;
			self.tell(actor, "I'm currently with " + owner.name + ", but I could be persuaded to leave with you...");
		}
		else {
			waitpulse 5;
			self.tell(actor, "I'm currently up for grabs!");
		}

		waitpulse 5;
		self.tell(actor, "I suppose I'd follow you around for ... let's say " + price + " copper.");
	}
};
