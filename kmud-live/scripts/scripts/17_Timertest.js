var script17 = function(self, actor, here, args, extra) {
	actor.start_timer(3);
	var success = runTimer(actor);
	if (success)
	self.say("Yes!");
	else
	self.say("No!");
}