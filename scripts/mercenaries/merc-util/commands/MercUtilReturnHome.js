var MercUtil = (function(MercUtil) {

	MercUtil.performReturnHome = function(actor, target, vArgs, here, myMercObj)
	{
		var homeRoom = MercUtil::getHomeBase(target);
		var mercArray = MercUtil.getMercArray(homeRoom);

		var price = MercUtil.getPrice(target);
		target.say("OK, since you've seen me home alive I'll thank you with this bit of coin back.");
		actor.gold += parseInt(price*0.2);
		actor.send(target.name + " gives you" + actor.numCopperToText(Math.floor(price*0.2), true) + ".");

		target.say("I'll leave your things here for you.");

		target.comm("rem all");
		target.comm("give all " + actor.name);
		target.comm("rem all");
		target.comm("drop all");
		target.comm("follow target");
		here.echo(capFirstLetter(target.name) + " sets off to return home.");

		MercUtil.clearSavedMercenaryEquipment(target);
		MercUtil.removeMercenaryFromOwner(target, actor);
		MercUtil.extractMercenary(target);
	};

	return MercUtil;

})(MercUtil || {});