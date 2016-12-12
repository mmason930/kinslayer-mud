var MercUtil = (function(MercUtil) {

	MercUtil.performGive = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1], arg3 = vargs[2];
		if(!arg2) {
			target.say("What do you want me to give and to who?");
			return;
		}
		else {
			if(!arg3) {
				target.say("Who do you want me to give it to?");
				return;
			}
			if(target.inventory.length == 0) {
				target.say("I'm not holding anything to give.");
				return;
			}
			else {
				var rec = getCharInListVis(target, arg3, here.people);
				if(rec == null) {
					target.say(capFirstLetter(arg3) + " doesn't seem to be here.");
					return;
				}
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var itemTarget = getObjInListVis(target,arg2,target.inventory);
					if(itemTarget == null) {
						target.say("I don't see a " + arg2 + " that I can give.");
					}
					else {
						target.comm("give " + allPoint + arg2 + " " + arg3);
					}
				}
				else {
					target.comm("give all" + " " + arg3);
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});