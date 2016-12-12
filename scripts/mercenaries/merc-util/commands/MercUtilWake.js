var MercUtil = (function(MercUtil) {

	MercUtil.performWake = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position == constants.POS_SLEEPING) {
			target.comm("wake");
		}
		else {
			target.say("I am already awake.");
		}
	};

	return MercUtil;
})(MercUtil || {});