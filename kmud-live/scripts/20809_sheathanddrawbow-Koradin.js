var script20809 = function(self, actor, here, args, extra)
{
	//For use with sheathing a bow onto your back, and drawing it from your back to wield it.
//Koradin - December 2009
var arg_array = getArgList(args);
if (arg_array[1])
{
	if (!isName(arg_array[1], self.namelist))
	{
		_noblock;
		return;
	}
}
if (actor.eq(constants.WEAR_BACK) == self && strn_cmp(arg_array[0], "draw", 3))
{
	if (actor.eq(constants.WEAR_WIELD))
	{
		actor.send("You already have something wielded.");
		_block;
		return;
	}
	else
	{
		act("You slide " + self.name + " off your back in one swift motion.", true, actor, null, null, constants.TO_CHAR);
		act("$n slides " + self.name + " off $s back in one swift motion.", true, actor, null, null, constants.TO_ROOM);
		self.moveToChar(actor);
		actor.comm("wield bow");
		_block;
		return;
	}
}
else if (actor.eq(constants.WEAR_WIELD) == self && strn_cmp(arg_array[0], "sheath", 3))
{
	if (actor.eq(constants.WEAR_BACK))
	{
		actor.send("You already have something on your back.");
		_block;
		return;
	}
	else
	{
		act("You sling " + self.name + " behind you in one swift motion.", true, actor, null, null, constants.TO_CHAR);
		act("$n slings " + self.name + " behind $m in one swift motion.", true, actor, null, null, constants.TO_ROOM);
		self.moveToChar(actor);
		actor.comm("wear bow");
		_block;
		return;
	}
}
_noblock;

};

