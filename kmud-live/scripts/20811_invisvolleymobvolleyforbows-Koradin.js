var script20811 = function(self, actor, here, args, extra)
{
	//This script is on an invisible mob that appears when people are barraging an area using a bow.
//Koradin - December 2009
var room = self.room;
var dodge_roll = random(1,100);
var arrow_effects = [];
var dmg_message = [];
var bow_skill = actor.getSkill(getSkillVnum("Bow"));
var bow_skill_for_dodge = (bow_skill - 50);
var bow_roll = random(1,105);
var barrage_roll = random(1,105);
var barrage_skill = actor.getSkill(getSkillVnum("Barrage"));
var crit_skill = (actor.getSkill(getSkillVnum("Critical Hit")) / 4);
var dir = getSval(self, 20802, "direction");
actor = getSval(self, 20802, "actor");
var hitdam = getSval(self, 20802, "hitdam");
var dir_room = room.direction(dir);
var dir_room_1 = dir_room;
var dir_room_2 = getSval(self, 20802, "dir_room_2");
var dir_room_3 = getSval(self, 20802, "dir_room_3");
if (dir_room_2)
{
	if (dir_room_3)
	{
		var room_roll = random(1,10)
		if (room_roll > 4)
			dir_room = dir_room_1;
		else if (room_roll < 5 && room_roll > 1)
			dir_room = dir_room_2;
		else if (room_roll == 1)
			dir_room = dir_room_3;
	}
	else
	{
		var room_roll = random(1,10)
		if (room_roll > 3)
			dir_room = dir_room_1;
		else
			dir_room = dir_room_2;
	}
}
var back_item = actor.eq(constants.WEAR_BACK);
//THE FOLLOWING CHECKS TO SEE IF THE ACTOR IS WEARING A QUIVER, AND IF SO, MODIFIES THE ARROWS MESSAGES, AND EFFECTS.
if (back_item)
{
	for each(var word in back_item.namelist.split(" "))
	{
		if (word == "quiverforbows")
			var quiver = back_item;
	}
}
if (quiver && getSval(quiver, 20804, "starting arrows") > 0)
{
	if (quiver.vnum == 1720)// a studded leather quiver
	{
		var arrow = "a diamond-tipped arrow";
		var arrows = "diamond-tipped arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " reflects blinding light as it flies through the air");
		var arrows_fly_message = ("The " + arrows + " reflect blinding light as they fly through the air");
		hitdam *= 1.1;
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_SEVERE_WOUND)) {act(arrow + " buries itself in your flesh, deeply wounding you!", true, target, null, null, constants.TO_CHAR);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_SEVERE_WOUND)) {act("$n grunts in pain as " + arrow + " buries itself inside $m!", true, target, null, null, constants.TO_ROOM);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_SEVERE_WOUND)) {target.affect(constants.AFF_SEVERE_WOUND, 3, 0);} } );
	}
	else if (quiver.vnum == 1721)// a well-stitched cloth quiver
	{
		var arrow = "a streamlined arrow";
		var arrows = "streamlined arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " flies silently with precision and speed");
		var arrows_fly_message = ("The " + arrows + " fly silently with precision and speed");
		hitdam *= 1.5;
		dodge_roll += 25;
		crit_skill += 20;
	}
	else if (quiver.vnum == 1722)// a quiver of scorched leather
	{
		var arrow = "a burning arrow";
		var arrows = "burning arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " leaves a trail of smoke as it streaks through the air");
		var arrows_fly_message = ("The " + arrows + " leave trails of smoke as they streak through the air");
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BURN)) {target.burnedBy = actor;} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BURN)) {act("Flames erupt upon you as " + arrow + " finds its mark!", true, target, null, null, constants.TO_CHAR);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BURN)) {act("$n erupts in flames as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BURN)) {target.affect(constants.AFF_BURN, 3, 0);} } );
	}
	else if (quiver.vnum == 1723)// a thick fur-lined quiver
	{
		var arrow = "an icy steel arrow";
		var arrows = "icy steel arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " flashes crystal blue light as it flies through the air");
		var arrows_fly_message = ("The " + arrows + " flash crystal blue light as they fly through the air");		
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {act("You shiver in frosty pain as " + arrow + " shatters against your skin!", true, target, null, null, constants.TO_CHAR);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {act("$n shivers uncontrollably as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {target.affect(constants.AFF_CHILL, 3, 0);} } );
	}
	else if (quiver.vnum == 1724)// a wooden quiver with steel buckles
	{
		var arrow = "an explosive arrow";
		var arrows = "explosive arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " blasts through the air, creating a fireworks display");
		var arrows_fly_message = ("The " + arrows + " blast through the air, creating a fireworks finale");
		arrow_effects.push( function() {act("You are knocked back as " + arrow + " blows up in your face!", true, target, null, null, constants.TO_CHAR);} );
		arrow_effects.push( function() {act("$n is knocked back as " + arrow + " blows up in $s face!", true, target, null, null, constants.TO_ROOM);} );
		arrow_effects.push( function() {target.lag(5);} );
	}
	else if (quiver.vnum == 1725)// a sickly green quiver
	{
		var arrow = "an arrow dipped in venom";
		var arrows = "arrows dipped in venom";
		var arrow_fly_message = (capFirstLetter(arrow) + " hisses through the air in a cloud of poison");
		var arrows_fly_message = ("The " + arrows + " hiss through the air in a cloud of poison");		
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {act("You feel the venom seep into your bones!", true, target, null, null, constants.TO_CHAR);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {act("The color drains from $n's face as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {target.affect(constants.AFF_POISON, 3, 0);} } );
	}
	else if (quiver.vnum == 1726)// a rotting black quiver
	{
		var arrow = "a decaying arrow";
		var arrows = "decaying arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " drips disease as it coasts through the air");
		var arrows_fly_message = ("The " + arrows + " drip disease as they coast through the air");
		dodge_roll -= 10;
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DECAY)) {target.decayedBy = actor;} } );		
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DECAY)) {act("You nearly throw up as " + arrow + " contaminates you!", true, target, null, null, constants.TO_CHAR);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DECAY)) {act("$n starts to feel sick as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
		arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DECAY)) {target.affect(constants.AFF_DECAY, 3, 0);} } );
	}
	else
	{
		var arrow = "an arrow";
		var arrows = "arrows";
		var arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
		var arrows_fly_message = ("The " + arrows + " fly through the air...");
	}
}
else
{
	var arrow = "an arrow";
	var arrows = "arrows";
	var arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
	var arrows_fly_message = ("The " + arrows + " fly through the air...");
}
//THE FOLLOWING SETS A FUNCTION TO CALL FOR WHEN THE ARROW IS FINISHED FLYING.
function arrowFinish()
{
	var people = dir_room.people.length;
	var random_hit_chance = (5 - people);
	if (random_hit_chance < 0)
		random_hit_chance = 0;
	var hit_roll = random(1,10);
	if (dir_room.items)
	{
		if (dir_room.items.length > 0)
		{
			var random_roll = random(0, (dir_room.items.length - 1));
			if (random(1,2) == 1 && !dir_room.items[random_roll].extraFlags(constants.ITEM_INVISIBLE))
				var miss_message = dir_room.items[random_roll].name;
			else
				var miss_message = "the ground harmlessly";
		}
		else
			var miss_message = "the ground harmlessly";
	}
	else
		var miss_message = "the ground harmlessly";
	var target = getRandomPersonInRoom(dir_room);
	if (!target)
	{
		dir_room.echo(capFirstLetter(arrow) + " rains down from the sky and hits " + miss_message + ".");
		return;
	}
	else if (target.room != dir_room)
	{
		dir_room.echo(capFirstLetter(arrow) + " rains down from the sky and hits " + miss_message + ".");
		return;
	}
	if (bow_skill < bow_roll || barrage_skill < barrage_roll || hit_roll < random_hit_chance || !actor.canSee(target))
	{
		dir_room.echo(capFirstLetter(arrow) + " rains down from the sky and hits " + miss_message + ".");
		return;
	}
	else
	{
		if (target.absorb < 10)
			var dmg = Math.ceil((hitdam * (100 - target.absorb)) / 100);
		else if (target.absorb < 20)
			var dmg = Math.ceil((hitdam * (90 - target.absorb)) / 100);
		else
			var dmg = Math.ceil((hitdam * (80 - target.absorb)) / 100);
		var target_dodge = (target.dodge - random((bow_skill_for_dodge - 10), (bow_skill_for_dodge + 10)));
		if (target.shieldBlock)
		{
			if (random(1,120) < target.eq(constants.WEAR_SHIELD).parry)
			{
				target.send(capFirstLetter(arrow) + " rains down from the sky and glances off your raised shield, ricocheting to the ground harmlessly.");
				act(capFirstLetter(arrow) + " rains down from the sky and glances off of $n's raised shield, ricocheting to the ground.", true, target, null, null, constants.TO_ROOM);
				return;
			}
			else
			{
				if (crit_skill > random(1,100))
				{
					dmg *= 2;
					target.comm("scream");
					actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
				}
				for(i = 0; i < arrow_effects.length; i++)
					arrow_effects[i]();
				damageMessage(dmg);
				target.send(target.red(constants.CL_SPARSE) + "Your raised shield fails to block " + arrow + " as it rains down from the sky, and it hits you" + dmg_message + target.normal(constants.CL_SPARSE));
				act("$n's shield fails to block " + arrow + " as it rains down from the sky, which hits $m" + dmg_message, true, target, null, null, constants.TO_ROOM);
				target.damage(dmg, actor);
				dmg_message = [];
				return;
			}
		}
		if (target_dodge > dodge_roll)
		{
			act("You swiftly dodge " + arrow + " as it rains down from the sky.", true, target, null, actor, constants.TO_CHAR);
			act("$n suddenly dodges to the side, letting " + arrow + " hit the ground harmlessly.", true, target, null, null, constants.TO_ROOM);
			return;
		}
		if (crit_skill > random(1,100))
		{
			dmg *= 2;
			target.comm("scream");
			actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
		}
		for(i = 0; i < arrow_effects.length; i++)
			arrow_effects[i]();
		damageMessage(dmg);
		act(target.red(constants.CL_SPARSE) + capFirstLetter(arrow) + " suddenly rains down from the sky, hitting you" + dmg_message + target.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_CHAR);
		act(capFirstLetter(arrow) + " sudden rains down from the sky, hitting $n" + dmg_message, true, target, null, null, constants.TO_ROOM);
		target.damage(dmg, actor);
		dmg_message = [];
		if (target.vnum != -1)
			target.comm("k " + actor.name);
		return;
	}
}
//FUNCTION FINISHED.
//THE FOLLOWING SETS A FUNCTION FOR THE MESSAGE RECEIVED DEPENDING ON DAMAGE.
function damageMessage(dmg)
{
	if (dmg < 5)
		dmg_message.push(" lightly.");
	else if (dmg < 10)
		dmg_message.push(".");
	else if (dmg < 15)
		dmg_message.push(" hard.");
	else if (dmg < 20)
		dmg_message.push(" very hard.");
	else if (dmg < 25)
		dmg_message.push(" extremely hard.");
	else if (dmg < 30)
		dmg_message.push(", spraying blood everywhere!");
	else if (dmg < 40)
		dmg_message.push(" with intense force, spraying blood everywhere!");
	else if (dmg < 50)
		dmg_message.push(" with BRUTAL force, spraying blood everywhere!");
	else
		dmg_message.push(" with DEADLY force, spraying blood everywhere!");
}
//DAMAGE FUNCTION FINISHED.
wait 2;
if (room.doorIsClosed(dir) || room.doorIsLocked(dir))
	room.echo(capFirstLetter(arrow) + " soars high into the air, over the " + room.doorName(dir) + " and out of sight.");
else
	room.echo(capFirstLetter(arrow) + " soars high into the air, disappearing to the " + dirToText(dir) + ".");
wait 2;
if (dir_room == dir_room_3)
{
	dir_room_1.echo(capFirstLetter(arrow) + " soars by high overhead, quickly disappearing " + dirToText(dir) + "...");
	wait 2;
	dir_room_2.echo(capFirstLetter(arrow) + " soars by high overhead, quickly disappearing " + dirToText(dir) + "...");
	wait 2;
	arrowFinish();
	return;
}
else if (dir_room == dir_room_2)
{
	dir_room_1.echo(capFirstLetter(arrow) + " soars by high overhead, quickly disappearing " + dirToText(dir) + "...");
	wait 2;
	arrowFinish();
	return;
}
else if (dir_room == dir_room_1)
{
	arrowFinish();
	return;
}
if (getSval(actor, 20802, "volleying") == 0)
{
	self.extract();
	return;
}

};

