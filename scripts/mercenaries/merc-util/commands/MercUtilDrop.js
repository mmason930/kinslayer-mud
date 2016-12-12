var MercUtil = (function(MercUtil) {

	MercUtil.performDrop = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1];
		if(!arg2) {
			target.say("What do you want me to drop?");
			return;
		}
		else {
			if(target.inventory.length == 0) {
				target.say("I'm not holding anything to drop.");
				return;
			}
			else {
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var itemTarget = getObjInListVis(target,arg2,target.inventory);
					if(itemTarget == null) {
						target.say("I don't see a " + arg2 + " that I can drop.");
					}
					else {
						target.comm("drop " + allPoint + arg2);
					}
				}
				else {
					target.comm("drop all");
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});
