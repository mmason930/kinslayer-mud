var MercUtil = (function(MercUtil) {

	MercUtil.performPreciseStrike = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_THIEF && myMercObj.skillsList[myMercObj.skill] != "Precise Strike") {
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
			target.say("I'll try to strike them!");
			target.comm("prec");
		}
	};

	return MercUtil;
})(MercUtil || {});