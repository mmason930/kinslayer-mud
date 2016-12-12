var MercUtil = (function(MercUtil) {

	MercUtil.performFlee = function(actor, target, vArgs, here, myMercObj)
	{
		target.comm("flee");
	};

	return MercUtil;
})(MercUtil || {});