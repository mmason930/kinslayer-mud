var MercUtil = (function(MercUtil) {

	MercUtil.performSit = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position != constants.POS_SITTING) {
			target.comm("sit");
		}
		else {
			target.say("I am already sitting.");
		}
	};

	return MercUtil;
})(MercUtil || {});
