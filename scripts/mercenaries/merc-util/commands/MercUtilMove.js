var MercUtil = (function(MercUtil) {

	MercUtil.performMove = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position != constants.POS_STANDING) {
			target.say("I can't do that right now.");
			return;
		}

		var dir = dirFromTextAbbreviated(vArgs[0][0]);

		if(!here.direction(dir))
			target.say("I cannot go that way.");
		else if(here.doorIsClosed(dir)) //TODO: Loophole here where player can search for door names!
			target.say("The " + here.doorName(dir) + " is closed.");
		else
			target.comm(dirToText(dir));
	};

	return MercUtil;
})(MercUtil || {});