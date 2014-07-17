var script20800 = function(self, actor, here, args, extra) {
	//This script may turn into the new projectiles script.
	//Koradin - December 2009
	_block;
	var skill = actor.getSkill(getSkillVnum("Projectiles"));
	var projSkill = skill;
	//here.echo("Projectiles skill: "+skill);
	var proj = actor.eq(constants.WEAR_WIELD);
	if (!proj)
	{
		actor.send("You need to wield a throwable weapon first!");
		actor.send(" ");
		actor.send("Weapons that can be thrown include SHORT BLADES, ONE-HANDED SPEARS, and ONE-HANDED AXES.");
		return;
	}
	var proj_type = proj.value(constants.VALUE_WEAPON_TYPE);
	var room = actor.room;
	var knife = constants.WEAPON_SHORT_BLADE;
	var spear = constants.WEAPON_SPEAR;
	var axe = constants.WEAPON_AXE;
	if (actor.getSkill(getSkillVnum("Spears")) > skill && proj_type == spear) {
		skill = actor.getSkill(getSkillVnum("Spears")) - 20 + Math.floor(projSkill/4);
		//here.echo("Throwing spear skill: "+skill);
	}
	else if (actor.getSkill(getSkillVnum("Axes")) > skill && proj_type == axe) {
		skill = actor.getSkill(getSkillVnum("Axes")) - 20 + Math.floor(projSkill/4);
		//here.echo("Throwing axe skill: "+skill);
	}
	else if (actor.getSkill(getSkillVnum("Short Blades")) > skill && proj_type == knife) {
		skill = actor.getSkill(getSkillVnum("Short Blades")) - 20 + Math.floor(projSkill/4);
		//here.echo("Small projectile skill: "+skill);
	}
	//here.echo(skill);
	var crit = actor.getSkill(getSkillVnum("Critical Hit")) / 5;
	if (skill < 1)
		skill = 1;
	else if (skill > 99)
		skill = 99;
	var skill_roll = skill - 20;
	if (skill_roll < 1)
		skill_roll = 1;
	var arg_array = getArgList(args);
	var break_chance = false;
	if (proj.extraFlags(constants.ITEM_TWO_HANDED) && actor.name != "Koradin")
	{
		actor.send("You cannot throw " +proj.name+"!");
		actor.send(" ");
		actor.send("Weapons that can be thrown include SHORT BLADES, ONE-HANDED SPEARS, and ONE-HANDED AXES.");
		return;
	}
	if (proj_type != spear && proj_type != knife && proj_type != axe)
	{
		actor.send("You cannot throw " +proj.name+"!");
		actor.send(" ");
		actor.send("Weapons that can be thrown include SHORT BLADES, ONE-HANDED SPEARS, and ONE-HANDED AXES.");
		return;
	}
	if (arg_array.length < 2)
	{
		if (actor.fighting)
		{
			var timer = (100 - random(0,skill_roll)) / 20;
			if (timer > 5)
				timer = 5;
			var vict = actor.fighting;
		}
		else	
		{
			actor.send("Throw at who?");
			return;
		}
	}
	else
	{
		var vict = actor.getCharRoomVis(arg_array[1]);
			if (!vict)
			{
				actor.send("No one by that name here.");
				return;
			}
			else if (vict == actor)
			{
				actor.send("Why would you throw that at yourself?!");
				return;
			}
		if (actor.fighting)
		{
			var timer = (100 - random(0,skill_roll)) / 20;
			if (actor.fighting != vict)
				timer += random(1,3);
			if (timer > 5)
				timer = 5;
		}
		else
			var timer = (100 - random(skill_roll - 10,skill_roll + 10)) / 20;
	}
	if (proj_type == knife)
	{
		var hitroll = random(-20,50) + skill;
		var multiplier = 1.2;
	}
	else if (proj_type == axe)
	{
		var hitroll = random(-25,50) + skill;
		var multiplier = 1.15;
	}
	else if (proj_type == spear)
	{
		var hitroll = random(-30,50) + skill;
		var multiplier = 1.1;
	}
	var low = proj.value(constants.VALUE_WEAPON_DAM_LOW) * 2;
	var high = proj.value(constants.VALUE_WEAPON_DAM_HIGH) * 2;
	if (Math.floor(proj.vnum / 100) == 26)
	{
		low *= 5;
		high *= 5;
		break_chance = true;
		var cond = parseInt(proj.getPval("throwing"));
		if (cond > 0)
		{
			//actor.send("pval working");
			low += cond;
			if (low > high)
				low = high - 1;
		}
	}
	var dmg = random( low , high );
	//here.echo("Original damage: "+dmg);
	actor.startTimer(timer);
	var success = runTimer(actor);
	if (!success)
		return;
	if (!vict)
	{
		actor.send("They aren't here anymore.");
		return;
	}
	if (vict.room != actor.room)
	{
		actor.send("They aren't here anymore.");
		return;
	}
	act("You throw " + proj.name + " at $N!",true,actor,null,vict,constants.TO_CHAR);
	act("$n throws " + proj.name + " at you!",true,actor,null,vict,constants.TO_VICT);
	act("$n throws " + proj.name + " at $N!",true,actor,null,vict,constants.TO_NOTVICT);
	proj.moveToRoom(getRoom(20897));
	wait 2;
	proj.moveToRoom(room);
	act(capFirstLetter(proj.name) + " flashes through the air...",true,null,proj,null,constants.TO_ROOM);
	proj.moveToRoom(getRoom(20897));
	wait 2;
	proj.moveToRoom(room);
	//room.echo("Hitroll: " +hitroll);
	if (!vict)
	{
		act(capFirstLetter(proj.name) + " hits the ground harmlessly.",true,null,proj,null,constants.TO_ROOM);
		return;
	}
	if (vict.room != room)
	{
		act(capFirstLetter(proj.name) + " hits the ground harmlessly.",true,null,proj,null,constants.TO_ROOM);
		return;
	}
	if (skill < random(-10,100))
	{
		act(capFirstLetter(proj.name) + " narrowly misses you!",true,vict,null,null,constants.TO_CHAR);
		act(capFirstLetter(proj.name) + " narrowly misses $n!",true,vict,null,null,constants.TO_ROOM);
		return;
	}
	if (vict)
	{
		if (vict.isBashed) {
			dmg = Math.floor(dmg / 1.25);
			//here.echo("Damage nerfed to: "+dmg+" because of bash");
		}
	}
	//if (vict.class == constants.CLASS_CHANNELER || vict.class == constants.CLASS_DREADGUARD || vict.class == constants.CLASS_DREADLORD)
	//	hitroll -= random(8,12);
	var dodgeChance = vict.dodge;
	if (dodgeChance > 115)
	{
		dodgeChance = 115;
		var dmgReduction = 1 - ((vict.dodge - dodgeChance) / 50);
		dmg = Math.floor(dmg * dmgReduction);
		//here.echo("Damage is now: "+dmg+" because of DB reduction of: "+((1-dmgReduction)*100)+"%");
	}
	if (vict.shieldBlock == true && random(0,180) < vict.getSkill(getSkillVnum("Shield Parry")))
	{
		act(capFirstLetter(proj.name) + " glances off your raised shield, ricocheting to the ground.",true,vict,null,null,constants.TO_CHAR);
		act(capFirstLetter(proj.name) + " glances off of $n's raised shield, ricocheting to the ground.",true,vict,null,null,constants.TO_ROOM);
		return;
	}
	if (dodgeChance > hitroll)
	{
		act("You swiftly dodge " + proj.name + ", letting it fly past.",true,vict,null,null,constants.TO_CHAR);
		act("$n suddenly dodges to the side, letting " + proj.name + " fly past.",true,vict,null,null,constants.TO_ROOM);
		return;
	}
	var nameArr = proj.name.split(" ");
	nameArr[0] = "Your";
	var name = nameArr.join(" ");
	if (random(0,100) < crit)
	{
		actor.send(actor.green(constants.CL_OFF) + "You scored a critical hit!" + actor.normal(constants.CL_OFF));
		vict.comm("scream");
		dmg = Math.floor(dmg * multiplier);
		//here.echo("Damage is now: "+dmg+" because of CRIT HIT");
	}
	dmg = Math.ceil((dmg * (100 - vict.absorb)) / 100);
	//here.echo("Damange after abs: "+dmg);
	//here.echo(dmg);
	if (dmg < 15)
		var dmg_message = " lightly.";
	else if (dmg < 30)
		var dmg_message = " hard.";
	else if (dmg < 40)
		var dmg_message = " very hard.";
	else if (dmg < 50)
		var dmg_message = " extremely hard.";
	else if (dmg < 65)
		var dmg_message = ", spraying blood everywhere!";
	else if (dmg < 80)
		var dmg_message = " with intense force, spraying blood everywhere!";
	else if (dmg < 100)
		var dmg_message = " with BRUTAL force, spraying blood everywhere!";
	else
		var dmg_message = " with DEADLY force, spraying blood everywhere!";
	act(vict.red(constants.CL_OFF)+capFirstLetter(proj.name) + " hits you"+dmg_message+vict.normal(constants.CL_OFF),true,vict,null,null,constants.TO_CHAR);
	actor.send(actor.green(constants.CL_OFF)+name+ " hits "+vict.name+dmg_message+actor.normal(constants.CL_OFF));
	echoaroundmany([vict,actor],capFirstLetter(proj.name) + " hits "+vict.name+dmg_message);
	if (break_chance == true && random(1,25) == 1)
	{
		room.echo(capFirstLetter(proj.name) + " breaks apart on impact.");
		proj.deletePval("throwing",true);
		proj.extract();
	}
	else
		proj.moveToChar(vict);
	vict.damage(dmg,actor);
	//actor.send(dmg);
	if (vict.vnum != -1)
		vict.comm("kill " + actor.name);
	else if (actor.room == vict.room)
	{
		if (vict.position == constants.POS_STANDING)
			vict.comm("kill " + actor.name);
	}
	
}