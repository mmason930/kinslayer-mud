var MercUtil = (function(MercUtil) {

	MercUtil.performRest = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position != constants.POS_RESTING)
			target.comm("rest");
		else
			target.say("I am already resting.");
	};

	return MercUtil;
})(MercUtil || {});
