var MercUtil = (function(MercUtil) {

	MercUtil.performSleep = function(actor, target, vArgs, here, myMercObj)
	{
		if(target.position != constants.POS_SLEEPING) {
			target.comm("sleep");
		}
	};

	return MercUtil;
})(MercUtil || {});
