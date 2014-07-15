var script20807 = function(self, actor, here, args, extra) {
	//POWER SHOT ABILITY FOR BOW USERS.
	//KORADIN - DECEMBER 2009
	var inv_items = actor.inventory;
	for each(var item in inv_items)
	{
		if (self == item)
			var in_inv = 1;
	}
	if (actor.eq(constants.WEAR_BACK) == self || in_inv == 1)
	{
		var wielded = actor.eq(constants.WEAR_WIELD);
		if (wielded)
		{
			if (actor.eq(constants.WEAR_WIELD).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW)
				_noblock;
			else
			{
				actor.send("You may want to wield " + self.name + " first!");
				_block;
			}
			return;
		}
		else
		{
			actor.send("You may want to wield " + self.name + " first!");
			_block;
			return;
		}
	}
	_block;
	if (self.vnum == 1709)// a curved shortbow
		var hitdam = random(4,14);
	else if (self.vnum == 1704)// a light wooden bow
		var hitdam = random(5,16);
	else if (self.vnum == 1701)// a gnarled thick wooden bow
		var hitdam = random(6,18);
	else if (self.vnum == 1700)// a stout yew bow
		var hitdam = random(7,20);
	else if (self.vnum == 1702)// a long polished steel bow
		var hitdam = random(8,22);
	else if (self.vnum == 1706)// a sleek golden bow
		var hitdam = random(9,24);
	else if (self.vnum == 1707)// a sturdy sungwood bow (two handed)
		var hitdam = random(10,26);
	else if (self.vnum == 1703)// a blackened silver longbow (two handed)
		var hitdam = random(11,28);
	else if (self.vnum == 1705)// a delicate hunting bow (two handed)
		var hitdam = random(12,30);
	else if (self.vnum == 1708)// an ornate Two Rivers longbow (two handed, unique)
		var hitdam = random(15,35);
	else if (self.vnum == 1346)
		var hitdam = random(14,32);// Obould's master weapon
	var arg_array = getArgList(args);
	if (arg_array[0] == "pshot")
	{
		actor.send("Try " + actor.cyan(constants.CL_SPARSE) + "PSHOOT" + actor.normal(constants.CL_SPARSE) + " instead.");
		return;
	}
	var bow_skill = actor.getSkill(getSkillVnum("Bow"));
	if (bow_skill < 1)
	{
		actor.send("You don't know how to use " + self.name + ", let alone perform a Power Shot!");
		return;
	}
	var pshot_raw_skill = actor.getSkill(getSkillVnum("Power Shot"));
	var pshot_skill = (actor.getSkill(getSkillVnum("Power Shot")) - 50);
	//here.echo("skill = " + pshot_raw_skill);
	//here.echo("power = " + pshot_skill)
	if (pshot_skill < 1)
		pshot_skill = 1;
	//here.echo("power = " + pshot_skill);
	if (pshot_raw_skill <= 0)
	{
		actor.send("You have no idea how to perform a Power Shot!");
		return;
	}
	if ( actor.strength < 17 || (actor.strength == 17 && actor.affectedBy(constants.AFF_STRENGTH)) )
	{
		actor.send("You don't feel strong enough to perform a Power Shot!");
		return;
	}
	var room = actor.room;
	var dodge_roll = random(1, 100);
	//here.echo("dodge = " + dodge_roll);
	if (arg_array.length < 2)
	{
		if (actor.fighting)
			var target = actor.fighting;
		else
		{
			actor.send("Who is supposed to get hit by this powerful shot?");
			return;
		}
	}
	else if (arg_array.length > 2)
	{
		for (i = 0; i < 6; i++)
		{
			if (strn_cmp(arg_array[2], dirToText(i), 1))
				var dir = i;
		}
		if (dir)
		{
			actor.send("Sorry, but the arrow wouldn't be powerful enough once it reaches its destination!");
			return;
		}
		else
		{
			actor.send(capFirstLetter(arg_array[1]) + " may be here, but what is '" + arg_array[2] + "' ??");
			return;
		}
	}
	else
		var target = getCharInListVis(actor, arg_array[1], room.people);
	if (!target)
	{
		actor.send(capFirstLetter(arg_array[1]) + " doesn't seem to be here.");
		return;
	}
	if (target == actor)
	{
		actor.send("Why would you shoot yourself?!");
		return;
	}
	if (getSval(actor, 20807, "bash stack") == 1)
	{
		actor.send("You just shot it! Have patience, it's on its way!");
		return;
	}
	else if (getSval(actor, 20807, "bash stack") == 2 || target.isBashed || getSval(target, 20807, "target stack") > 0)
	{
		actor.send("But your target is already stunned!");
		return;
	}
	actor.startTimer(2.1);
	setSval(actor, 20807, "bash stack", 1);
	var success = runTimer(actor);
	if (!success)
	{
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	if (!target)
	{
		actor.send("They aren't here anymore.");
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	if (target.room != room)
	{
		actor.send("They aren't here anymore.");
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	if (getSval(target, 20807, "target stack") > 0)
	{
		actor.send("But your target is already stunned!");
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	actor.send("\nAfter stretching your bowstring tight, you fire a powerful shot from " + self.name + "!");
	if (actor.fighting)
		act("\n$n steps back from the fight long enough to fire an arrow into the air!", true, actor, null, target, constants.TO_ROOM);
	else
		act("\n$n takes aim and fires an arrow from " + self.name + "!", true, actor, null, target, constants.TO_ROOM);
	wait 1.4;
	//room.echo("An arrow blazes through the air with added speed and power...")
	wait 1.4;
	if (!target)
	{
		room.echo("A large THWACK echoes throughout the area as an arrow hits the ground harmlessly.");
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	if (target.room != room)
	{
		room.echo("A large THWACK echoes throughout the area as an arrow hits the ground harmlessly.");
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	var target_abs = (target.absorb + (target.absorb / 3));
	var dmg = Math.ceil((hitdam * (100 - target_abs)) / 100);
	var target_dodge = (target.dodge - pshot_skill);
	//here.echo("dodge = " + target_dodge);
	if (target_dodge > dodge_roll && getSval(target, 20807, "target stack") < 1)
	{
		act("You swiftly dodge an arrow, letting it hit the ground with a loud THWACK.", true, target, null, actor, constants.TO_CHAR);
		act("$n suddenly dodges to the side, letting an arrow hit the ground with a loud THWACK.", true, target, null, null, constants.TO_ROOM);
		setSval(actor, 20807, "bash stack", 0);
		return;
	}
	if (target.absorb > 19)
	{
		if ((target.absorb - pshot_skill) > random(1,50) || (target.parry - pshot_skill) > random(1,250))
		{
			act("An arrow bounces off your armor, hitting the ground harmlessly.", true, target, null, actor, constants.TO_CHAR);
			act("An arrow bounces off of $n's armor, hitting the ground harmlessly.", true, target, null, null, constants.TO_ROOM);
			setSval(actor, 20807, "bash stack", 0);	
			return;
		}
	}
	if (!target.isBashed && getSval(target, 20807, "target stack") != 1)
	{
		act(target.red(constants.CL_SPARSE) + "An arrow hits you with unbelievable force, knocking you off your feet!" + target.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_CHAR);
		act("An arrow slams into $n with unbelievable force, knocking $m off $s feet!", true, target, null, null, constants.TO_ROOM);
		if (actor.questTaskProgress('Gaining Power')[1].quotient < 1)
			actor.updateJournalTask('Gaining Power',0);
		setSval(actor, 20807, "bash stack", 2);
		setSval(target, 20807, "target stack", 1);
		target.comm("");
		var random_lag = random(29,40);
		target.lag(random_lag);
		target.damage(dmg, actor);
		waitpulse random_lag;
		setSval(actor, 20807, "bash stack", 0);
		setSval(target, 20807, "target stack", 0);
		return;
	}
	else
	{
		act("An arrow hits you with unbelievable force!", true, target, null, actor, constants.TO_CHAR);
		act("An arrow slams into $n with unbelievable force!", true, target, null, null, constants.TO_ROOM);
		if (actor.questTaskProgress('Gaining Power')[1].quotient < 1)
			actor.updateJournalTask('Gaining Power',0);
		setSval(actor, 20807, "bash stack", 0);
		target.comm("");
		target.damage(dmg, actor);
		return;
	}
}