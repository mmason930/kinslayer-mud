var MercUtil = (function(MercUtil) {

	MercUtil.performInventory = function(actor, target, vArgs, here, myMercObj)
	{
		actor.send(capFirstLetter(target.name) + "'s Inventory:");
		var inventory = target.inventory;
		if(inventory.length == 0) {
			actor.send(" Nothing.");
		}

		target.inventory.forEach(function(item) {
			actor.send(item.name);
		});
	};

	return MercUtil;
})(MercUtil || {});

