var MercUtil = (function(MercUtil) {

	MercUtil.performHold = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1];
		if(!arg2) {
			target.say("What do you want me to hold?");
			return;
		}
		else {
			if(target.inventory.length == 0) {
				target.say("I'm not carrying anything to hold.");
				return;
			}
			else {
				if(arg2 != "all") {
					var itemTarget = getObjInListVis(target,arg2,target.inventory);
					if(itemTarget == null) {
						target.say("I don't see a " + arg2 + " that I can hold.");
					}
					else {
						var held = target.eq(constants.WEAR_HOLD);
						if(held) {
							target.say("I'm already holding " + held.name + ".");
						}
						else {
							if(itemTarget.canWear(constants.ITEM_WEAR_HOLD)) {
								target.comm("hold " + arg2);
							}
							else {
								target.say("I cannot hold " + itemTarget.name + ".");
							}
						}
					}
				}
				else {
					target.comm("hold all");
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});
