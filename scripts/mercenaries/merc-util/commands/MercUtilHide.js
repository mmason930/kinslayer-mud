var MercUtil = (function(MercUtil) {

	MercUtil.performHide = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_THIEF)
			self.say("I don't know how to do that...");
		if(self.position != constants.POS_STANDING)
			self.say("I can't hide in this position.");
		else {
			self.comm("nod");
			self.comm("hide");
		}
	};

	return MercUtil;
})(MercUtil || {});