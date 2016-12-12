var MercUtil = (function(MercUtil) {

	MercUtil.performShieldBlock = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_THIEF && myMercObj.skillsList[myMercObj.skill] != "Shield Block") {
			target.say("I don't know how to do that...");
			return;
		}
		var shield = target.eq(constants.WEAR_SHIELD);
		if(!shield) {
			target.say("I'm not even using a shield!");
			return;
		}
		if(target.position != constants.POS_FIGHTING) {
			target.say("I'm not fighting anyone though.");
		}
		else {
			target.say("I'll try to block!");
			target.comm("shield");
		}
	};

	return MercUtil;
})(MercUtil || {});