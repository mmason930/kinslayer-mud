var script20808 = function(self, actor, here, args, extra) {
	//This script makes sure people aren't wearing bows on their backs and wielding them at the same time.
	//Koradin - December 2009
	if (actor.eq(constants.WEAR_WIELD))
	{
		if (actor.eq(constants.WEAR_WIELD).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW)
		{
			actor.send("You're already wielding one...just use it instead!");
			_block;
			return;
		}
	}
	else if (actor.eq(constants.WEAR_BACK))
	{
		if (actor.eq(constants.WEAR_BACK).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW)
		{
			actor.send("Why don't you just use the one on your back?");
			_block;
			return;
		}
	}
	else
	{
		_noblock;
		return;
	}
}