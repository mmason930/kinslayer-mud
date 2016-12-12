var MercUtil = (function(MercUtil) {

	MercUtil.performWield = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1];
		if(!arg2) {
			target.say("What do you want me to wield?");
			return;
		}
		else {
			if(target.inventory.length == 0) {
				target.say("I'm not holding anything to wield.");
				return;
			}
			else {
				if(arg2 != "all") {
					var itemTarget = getObjInListVis(target,arg2,target.inventory);
					if(itemTarget == null) {
						target.say("I don't see a " + arg2 + " that I can wield.");
					}
					else {
						var weapon = target.eq(constants.WEAR_WIELD);
						if(weapon) {
							target.say("I'm already wielding " + weapon.name + ".");
						}
						else {
							if(itemTarget.canWear(constants.ITEM_WEAR_WIELD)) {
								target.comm("wield " + arg2);
							}
							else {
								target.say("I cannot wield " + itemTarget.name + ".");
							}
						}
					}
				}
				else {
					target.comm("wield all");
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});
