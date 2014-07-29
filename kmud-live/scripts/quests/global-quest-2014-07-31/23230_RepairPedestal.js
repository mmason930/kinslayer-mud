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
		mudLog(constants.BRF, Math.max(actor.invis, 100), "Character `" + actor.name + "` is attempting to repair an invalid pedestal in room #" + here.vnum + ".");
		return;
	}

	if(pedestal.getRace() != actor.race)
	{
		actor.send("Repair an enemy's pedestal??? Are you crazy?!?");
		return;
	}

	var checks = function()
	{
		if(!pedestal.isDisabled())
		{
			actor.send("The pedestal is not disabled. You cannot repair it yet!");
			return false;
		}

		if(actor.fighting)
		{
			actor.send("You can't repair that while engaged!");
			return false;
		}

		if(actor.position <= constants.POS_FIGHTING)
		{
			actor.send("You must be STANDING to do that!");
			return false;
		}

		return true;
	};

	if(!checks())
	{
		return;
	}

	act("$n begins repairing $P.", false, actor, null, targetObject, constants.TO_ROOM);
	act("You begin repairing $P.", false, actor, null, targetObject, constants.TO_CHAR);

	while(true)
	{
		actor.startTimer(10);
		var success = runTimer(actor);

		if(!success || !checks())
		{
			break;
		}

		pedestal.repair(actor, random(3, 7));
		act("$n repairs a portion of $P!", false, actor, null, targetObject, constants.TO_ROOM);
		act("You repair a portion of $P!", false, actor, null, targetObject, constants.TO_CHAR);

		if(!pedestal.isDisabled())
		{
			act("$p is erected back into place, fully repaired!", false, null, targetObject, null, constants.TO_ROOM);
			break;
		}
	}
};