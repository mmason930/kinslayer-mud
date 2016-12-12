var MercUtil = (function(MercUtil) {

	MercUtil.performSneak = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_THIEF) {
			target.say("I don't know how to do that...");
			return;
		}
		target.say("I'll try to move quietly.");
		target.comm("sneak");
	};

	return MercUtil;
})(MercUtil || {});