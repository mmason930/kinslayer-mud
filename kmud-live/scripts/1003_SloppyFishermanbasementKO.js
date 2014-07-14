var script1003 = function(self, actor, here, args, extra)
{
	waitpulse 5;
	if (actor.race == constants.RACE_HUMAN)
		actor.send("A brutish footpad says, 'Hello stranger, goodnight.'");
		waitpulse 2;
		if (actor.race == constants.RACE_HUMAN)
			actor.send("A cunning thief smacks you on the back of the head with a blackjack!");
		if (actor.race == constants.RACE_HUMAN)	
			actor.comm("give 100 copper footpad")
		if (actor.race == constants.RACE_HUMAN)	
			actor.comm("u");
		if (actor.race == constants.RACE_HUMAN)
			actor.damage(50);
		if (actor.race == constants.RACE_HUMAN)
			actor.send("You wake up some time later, with an incredible headache.");

};

