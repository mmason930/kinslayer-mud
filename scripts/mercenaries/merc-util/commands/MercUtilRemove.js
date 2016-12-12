var MercUtil = (function(MercUtil) {

	MercUtil.performRemove = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1];
		if(!arg2) {
			target.say("What do you want me to remove?");
			return;
		}
		else {
			if(arg2 != "all") {
				var allPoint = "";
				if(arg2.split(".")[0] == "all") {
					arg2 = arg2.split(".")[1];
					allPoint = "all.";
				}
				var itemTarget = target.getObjWorn(arg2);
				if(itemTarget == null) {
					target.say("I don't see a " + arg2 + " that I can remove.");
				}
				else {
					if(target.maxCarrItems <= target.inventory.length || target.maxCarryWeight <= target.carr_weight) {
						target.say("I can't carry any more.");
						return;
					}
					else {
						target.comm("remove " + allPoint + arg2);
					}
				}
			}
			else {
				if(target.maxCarrItems <= target.inventory.length || target.maxCarryWeight <= target.carr_weight) {
					target.say("I can't carry any more.");
					return;
				}
				else {
					target.comm("remove all");
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});