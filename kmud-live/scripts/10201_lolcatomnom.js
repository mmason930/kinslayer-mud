var script10201 = function(self, actor, here, args, extra)
{
	var randNum = random(1,4);
waitpulse randNum;
act("$n noms on ur face!", false, self, null, actor, constants.TO_VICT);
act("$n noms on $N's face!", false, self, null, actor, constants.TO_NOTVICT);
act("You nom on $N's face!", false, self, null, actor, constants.TO_CHAR);
actor.hps -= random(5, 25);

};

