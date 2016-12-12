var MercUtil = (function(MercUtil) {

	MercUtil.performRescue = function(actor, target, vArgs, here, myMercObj)
	{
		if(arg2 == "me" || !arg2)
		{
			if(actor.position != constants.POS_FIGHTING)
				target.say("And who am I rescuing you from?!");
			else
				target.comm("rescue " + owner.name);
		}
		else
		{
			var rescueTarget = getCharInListVis(self,arg2,here.people);
			if(rescueTarget)
			{
				if(rescueTarget.position != constants.POS_FIGHTING)
				{
					if(rescueTarget == actor)
						target.say("You aren't fighting anyone though.");
					else
						target.say(capFirstLetter(rescueTarget.name) + " isn't fighting anyone though.");
				}
				else
					target.comm("rescue " + rescueTarget.namelist);
			}
			else
				target.say("Who am I rescuing?");
		}
	};

	return MercUtil;
})(MercUtil || {});