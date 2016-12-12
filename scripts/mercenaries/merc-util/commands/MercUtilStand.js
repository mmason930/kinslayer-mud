var MercUtil = (function(MercUtil) {

	MercUtil.performStand = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position != constants.POS_STANDING)
			target.comm("stand");
		else
			target.say("I am already standing.");
	};

	return MercUtil;
})(MercUtil || {});
