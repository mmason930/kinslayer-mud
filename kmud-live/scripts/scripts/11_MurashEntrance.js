var script11 = function(self, actor, here, args, extra) {
	waitpulse 6;
	if( actor.race == constants.RACE_TROLLOC )
		self.say("Allow me to get that for you. Our master doesn't like to be surprised.");
	else
		self.say("Allow me to get that for you. I am sure our Master will want to kill you himself.");
	waitpulse 15;
	self.comm("unlock irondoor");
	self.comm("open irondoor");
	waitpulse 10;
	self.say("He will see you now.");
	waitpulse 50;
	self.comm("close irondoor");
	self.comm("lock irondoor");
};