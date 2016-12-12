var MercUtil = (function(MercUtil) {

	MercUtil.performAssist = function(actor, target, vArgs, here, myMercObj)
	{
		if(!vArgs[1])
		{
			if(actor.position != constants.POS_FIGHTING)
				target.say("And who am I assisting you against?!");
			else
				target.comm("assist " + owner.name);
		}
		else
		{
			if(vArgs[1] == "me")
			{
				if(actor.position != constants.POS_FIGHTING)
					target.say("And who am I assisting you against?!");
				else
					target.comm("assist " + actor.name);
			}
			else {
				var assistTarget = getCharInListVis(target, vArgs[1], here.people);
				if(assistTarget != null)
				{
					if(assistTarget.position != constants.POS_FIGHTING)
						target.say(capFirstLetter(assistTarget.name) + " isn't fighting anyone though.");
					else {
						echoaround(assistTarget, capFirstLetter(target.name) + " assists " + assistTarget.name + ".");
						target.comm("assist " + assistTarget.name);
					}
				}
				else
					target.say("Who am I assisting?");
			}
		}
	};

	return MercUtil;
})(MercUtil || {});