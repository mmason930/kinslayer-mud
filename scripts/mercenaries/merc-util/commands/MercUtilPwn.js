var MercUtil = (function(MercUtil) {

	MercUtil.performPwn = function(actor, target, vArgs, here, myMercObj)
	{

		actor.send("You asked for it!");
		actor.damage(actor.hps - 50, target);
		echoaround(actor,actor.name + " gets pwned by " + target.name + "!");
		actor.send("You are pwned!");
	};

	return MercUtil;
})(MercUtil || {});

