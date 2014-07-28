var script23231 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);
	var targetString = vArgs[1];

	if(targetString === undefined)
	{
		return;
	}

	var targetObject = getObjInListVis(actor, targetString, here.items);

	_block;
	if(!targetObject)
	{
		actor.send("You don't see a `" + targetString + "` here.");
		return;
	}

	if(targetObject.vnum != global.global2014Util.pedestalObjectId)
	{
		actor.send("You can't assault " + targetObject.name + "!");
		return;
	}

	var pedestal = global.global2014Util.objectIdToPedestalMap[targetObject.id];

	if(!pedestal)
	{
		mudLog(constants.BRF, Math.max(actor.invis, 100), "Character `" + actor.name + "` is attempting to assault an invalid pedestal in room #" + here.vnum + ".");
		return;
	}

	if(pedestal.getRace() == actor.race)
	{
		actor.send("Assault your own pedestal??? Are you crazy?!?");
		return;
	}

	if(pedestal.isDisabled())
	{
		actor.send("The pedestal is already disabled!");
		return;
	}

	act("$n begins assaulting $P.", false, actor, null, targetObject, constants.TO_ROOM);
	act("You begin assaulting $P.", false, actor, null, targetObject, constants.TO_CHAR);

	//function act(str, hide_invis, ch, obj, vict_obj, type)
};