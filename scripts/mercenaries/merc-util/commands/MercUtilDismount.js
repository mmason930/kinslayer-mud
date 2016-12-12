var MercUtil = (function(MercUtil) {

	MercUtil.performDismount = function(actor, target, vArgs, here, myMercObj)
	{
		if(!target.mount)
			target.say("I'm not riding anything...");
		else
			target.comm("dismount");
	};

	return MercUtil;
})(MercUtil || {});