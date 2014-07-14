var script18602 = function(self, actor, here, args, extra)
{
	// Displays the current cost to hire a mercenary when he hears "hire" or "cost"
// Rhollor - December 2009
var price = getSval(self,18600,"price");
var owner = getSval(self,18600,"owner");
if ((owner == actor)) {
	waitpulse 5;
	self.comm("tell " + actor.name + " You've already hired me moron!");
	waitpulse 5;
	self.comm("tell " + actor.name + " Shut up and get a move on unless you wanna pay me some more coin...");
}
else {
	if(owner != 0 && owner.isValid) {
		waitpulse 5;
		self.comm("tell " + actor.name + " I'm currently with " + owner.name + ", but I could be persuaded to leave with you...");
	}
	else {
		waitpulse 5;
		self.comm("tell " + actor.name + " I'm currently up for grabs!");
	}
	waitpulse 5;
	self.comm("tell " + actor.name + " I suppose I'd follow you around for ... let's say " + price + " copper.");
}

};

