var MercUtil = (function(MercUtil) {

	MercUtil.performFollow = function(actor, target, vArgs, here, myMercObj)
	{
		if(actor == target.leader)
			target.say("I'm already following you.");
		else
			target.comm("follow " + actor.name);
	};

	return MercUtil;
})(MercUtil || {});