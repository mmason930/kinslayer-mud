var script23230 = function(self, actor, here, args, extra) {

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
		actor.send("You can't repair " + targetObject.name + "!");
		return;
	}

	var pedestal = global.global2014Util.objectIdToPedestalMap[targetObject.id];

	if(!pedestal)
	{
		mudLog(constants.BRF, Math.max(actor.invisLevel, 100), "Character `" + actor.name + "` is attempting to repair an invalid pedestal in room #" + here.vnum + ".");
		return;
	}

	if(pedestal.getRace() != actor.race)
	{
		actor.send("Repair an enemy's pedestal??? Are you crazy?!?");
		return;
	}

	if(!pedestal.isDisabled())
	{
		actor.send("The pedestal is not disabled. You cannot repair it yet!");
		return;
	}

	actor.send("You are repairing " + targetObject.name);
};