var MercUtil = (function(MercUtil) {

	MercUtil.performGet = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1], arg3 = vArgs[2];
		if(!arg2) {
			target.say("What do you want me to take?");
			return;
		}
		else {
			if(target.room.items.length == 0 && !arg3) {
				target.say("There's nothing here to take.");
				return;
			}
			else {
				if(!arg3) {
					arg3 = "";
				}
				else {
					var cont = target.getObjWorn(arg3);
					if(cont == null) {
						//check inv
						cont = getObjInListVis(target,arg3,target.inventory);
					}
					if(cont == null) {
						//check ground
						cont = getObjInListVis(target,arg3,target.room.items);
					}
					if(cont == null) {
						//Didnt find cont
						target.say("I don't see a " + arg3 + " to take " + arg2 + " from.");
						return;
					}
					else {
						//did find cont
						if(cont.contents.length == 0) {
							target.say("There doesn't appear to be anything in " + cont.name + ".");
						}
					}
					arg3 = " " + arg3;
				}
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var itemTarget = getObjInListVis(target,arg2,target.room.items);
					if(itemTarget == null && arg2 != "all") {
						if(arg3) {
							itemTarget = getObjInListVis(target,arg2,cont.contents);
							if(itemTarget == null) {
								target.say("I don't see a " + arg2 + " in " + cont.name + ".");
								return;
							}
						}
						else {
							target.say("I don't see a " + arg2 + " that I can take.");
							return;
						}
					}
					if(itemTarget != null) {
						if(target.maxCarrItems <= target.inventory.length || target.maxCarryWeight <= target.carr_weight) {
							target.say("I can't carry any more.");
							return;
						}
						else {
							target.comm("take " + allPoint + arg2 + arg3);
						}
					}
				}
				else {
					if(target.maxCarrItems <= target.inventory.length || target.maxCarryWeight <= target.carr_weight) {
						target.say("I can't carry any more.");
						return;
					}
					else {
						target.comm("take all" + arg3);
					}
				}
			}
		}
		MercUtil.saveMercenaryEquipment(target);
	};

	return MercUtil;
})(MercUtil || {});
