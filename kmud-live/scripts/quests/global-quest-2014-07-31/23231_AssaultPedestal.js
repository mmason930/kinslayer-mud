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

	var checks = function(actor, pedestal)
	{
		if(pedestal.isDisabled())
		{
			actor.send("The pedestal is already disabled!");
			return false;
		}

		if(actor.fighting)
		{
			actor.send("You can't assault that while engaged!");
			return false;
		}

		if(actor.position <= constants.POS_FIGHTING)
		{
			actor.send("You must be STANDING to do that!");
			return false;
		}
	};

	if(!checks(actor, pedestal))
	{
		here.echo("Checks failed.");
		return;
	}

	act("$n begins assaulting $P.", false, actor, null, targetObject, constants.TO_ROOM);
	act("You begin assaulting $P.", false, actor, null, targetObject, constants.TO_CHAR);

	while(true)
	{
		actor.startTimer(10);
		var success = runTimer(actor);

		if(!success || !checks(actor, pedestal))
		{
			break;
		}

		pedestal.takeDamage(actor, random(10, 30));
		act("$n delivers a damaging blow to $P!", false, actor, null, targetObject, constants.TO_ROOM);
		act("You deliver a damaging blow to $P!", false, actor, null, targetObject, constants.TO_CHAR);

		if(pedestal.isDisabled())
		{
			act("$p topples over and falls to the ground!", false, null, targetObject, null, constants.TO_ROOM);
			break;
		}
	}
};