var MercUtil = (function(MercUtil) {

	MercUtil.performBash = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj && myMercObj.className != constants.CLASS_WARRIOR && myMercObj.className != constants.CLASS_RANGER && myMercObj.skillsList[myMercObj.skill] != "Bash") {
			target.say("I don't know how to do that...");
			return;
		}
		var weapon = target.eq(constants.WEAR_WIELD);
		if(!weapon) {
			target.say("I'm not even wielding a weapon!");
			return;
		}
		if(target.position != constants.POS_FIGHTING) {
			target.say("I'm not fighting anyone though.");
		}
		else {
			if(!target.fighting.isBashed) {
				target.say("I'll try to sit 'em down!");
				target.comm("bash");
			}
			else {
				target.say(capFirstLetter(target.fighting.name) + " is already bashed!");
			}
		}
	};

	return MercUtil;
})(MercUtil || {});