var MercUtil = (function(MercUtil) {

	MercUtil.performRide = function(actor, target, vArgs, here, myMercObj)
	{
		if (!arg2)
			target.say("What do you want me to ride?");
		else {
			var horse = getCharInListVis(target, arg2, here.people);
			if(!horse)
				target.say("I don't see any '" + arg2 + "' here!");
			if (!horse.mobFlagged(constants.MOB_MOUNT))
				target.say("I can't ride " + horse.name + "!");
			else if (horse.riddenBy)
				target.say(capFirstLetter(horse.name) + " is already being ridden.");
			else if (here.sector == constants.SECT_INSIDE)
				target.say("I can't ride inside...");
			else
				target.comm("ride " + arg2);
		}
	};

	return MercUtil;
})(MercUtil || {});