var script20810 = function(self, actor, here, args, extra) {
	//Barrage skill for use with bows.
	//Koradin - December 2009
	if (actor.position != constants.POS_STANDING)
	{
		if (actor.position == constants.POS_FIGHTING)
			actor.send("You're already fighting!");
		else
			actor.send("You're not in the right position for that!");
		_block;
		return;
	}
	//THE FOLLOWING CHECKS TO SEE IF THE BOW IS ON BACK OR IN INVENTORY.
	var inv_items = actor.inventory;
	for (var _autoKey in inv_items) {
		var item = inv_items[_autoKey];
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
	//ADD MORE BOWS HERE.
	var arrow_effects = [];
	var crit_skill = (actor.getSkill(getSkillVnum("Critical Hit")) / 4);
	var dodge_roll = random(1,100);
	var back_item = actor.eq(constants.WEAR_BACK);
	//THE FOLLOWING CHECKS TO SEE IF THE ACTOR IS WEARING A QUIVER, AND IF SO, MODIFIES THE ARROWS MESSAGES, AND EFFECTS.
	if (back_item)
	{
		for (var _autoKey in back_item.namelist.split(" ")) {
			var word = back_item.namelist.split(" ")[_autoKey];
			if (word == "quiverforbows")
				var quiver = back_item;
		}
	}
	if (quiver)
	{
		if (quiver.getPval("arrows") > 0)
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
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_SEVERE_WOUND)) {target.affect(constants.AFF_SEVERE_WOUND, 0, 0);} } );
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
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BURN)) {target.affect(constants.AFF_BURN, 1, 0);} } );
			}
			else if (quiver.vnum == 1723)// a thick fur-lined quiver
			{
				var arrow = "an icy steel arrow";
				var arrows = "icy steel arrows";
				var arrow_fly_message = (capFirstLetter(arrow) + " flashes crystal blue light as it flies through the air");
				var arrows_fly_message = ("The " + arrows + " flash crystal blue light as they fly through the air");		
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {act("You shiver in frosty pain as " + arrow + " shatters against your skin!", true, target, null, null, constants.TO_CHAR);} } );
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {act("$n shivers uncontrollably as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_CHILL)) {target.affect(constants.AFF_CHILL, 1, 0);} } );
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
				var effect_roll = random(1,3);
				if (effect_roll == 1)
				{
					arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DEAF)) {act("The world goes silent around you.", true, target, null, null, constants.TO_CHAR);} } );
					arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DEAF)) {target.affect(constants.AFF_DEAF, 1, 0);} } );
				}
				else if (effect_roll == 2)
				{
					arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BLIND)) {act("The world blurs as your sight is ripped away.", true, target, null, null, constants.TO_CHAR);} } );
					arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_BLIND)) {target.affect(constants.AFF_BLIND, 1, 0);} } );		
				}
			}
			else if (quiver.vnum == 1725)// a sickly green quiver
			{
				var arrow = "an arrow dipped in venom";
				var arrows = "arrows dipped in venom";
				var arrow_fly_message = (capFirstLetter(arrow) + " hisses through the air in a cloud of poison");
				var arrows_fly_message = ("The " + arrows + " hiss through the air in a cloud of poison");		
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {act("You feel the venom seep into your bones!", true, target, null, null, constants.TO_CHAR);} } );
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {act("The color drains from $n's face as " + arrow + " hits $m!", true, target, null, null, constants.TO_ROOM);} } );
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_POISON)) {target.affect(constants.AFF_POISON, 1, 0);} } );
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
				arrow_effects.push( function() {if (!target.affectedBy(constants.AFF_DECAY)) {target.affect(constants.AFF_DECAY, 1, 0);} } );
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
	}
	else
		{
			var arrow = "an arrow";
			var arrows = "arrows";
			var arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
			var arrows_fly_message = ("The " + arrows + " fly through the air...");
		}
	var arg_array = getArgList(args);
	if (!strn_cmp(arg_array[0], "barrage", 3))
	{
		_noblock;
		return;
	}
	_block;
	var bow_skill = actor.getSkill(getSkillVnum("Bow"));
	var barrage_skill = actor.getSkill(getSkillVnum("Barrage"));
	var room = actor.room;
	if (arg_array.length < 2 || arg_array.length > 2)
	{
		actor.send("In which direction would you like to start your barrage?")
		return;
	}
	else
	{
		for (i = 0; i < 6; i++)
		{
			if (strn_cmp(arg_array[1], dirToText(i), 1))
				var dir = i;
		}
		if (!dir && dir != 0)
		{
			actor.send("The direction '" + arg_array[1] + "' does not exist.");
			actor.send(actor.cyan(constants.CL_SPARSE) + "Directions include NORTH, SOUTH, EAST, WEST, UP, and DOWN." + actor.normal(constants.CL_SPARSE));
			return;
		}
		var dir_room = room.direction(dir);
		var dir_text = dirToText(dir);
		if (!room.direction(dir) || room.doorHidden(dir) > actor.getSkill(getSkillVnum("Search")))
		{
			actor.send("There doesn't seem to be a '" + dirToText(dir) + "' exit from here.");
			return;
		}
		else if (bow_skill < 1)
		{
			actor.send("You have no idea how to use " + self.name + ", let alone perform a barrage!");
			return;
		}
		else if (barrage_skill < 1)
		{
			actor.send("You have no idea how to perform a barrage.");
			return;
		}
		else if (room.sector == constants.SECT_INSIDE)
		{
			actor.send("You can't properly set up a barrage while inside!");
			return;
		}
		else if ((room.doorIsClosed(dir) || room.doorIsLocked(dir)) && dir_room.sector == constants.SECT_INSIDE)
		{
			actor.send("Why barrage over the " + room.doorName(dir) + " when you'll just hit the roof on the other side?");
			return;
		}
		else if (dir_room.sector == constants.SECT_INSIDE)
		{
			actor.send("Why barrage there when the arrows will just hit the roof?");
			return;
		}
		if (dir_room.direction(dir))
		{
			if (dir_room.direction(dir).sector != constants.SECT_INSIDE)
			{
				var dir_room_2 = dir_room.direction(dir);
				if (dir_room_2.direction(dir))
				{
					if (dir_room_2.direction(dir).sector != constants.SECT_INSIDE)
						var dir_room_3 = dir_room_2.direction(dir);
				}
			}
		}
		var barrage_skill_score = (101 - barrage_skill);
		var barrage_timer = 3.5;
		var total_barrage = barrage_skill;
		room.loadMob(20802);
		var mobby = room.people[0];
		setSval(mobby, 20802, "actor", actor);
		setSval(mobby, 20802, "direction", dir);
		setSval(mobby, 20802, "hitdam", hitdam);
		setSval(mobby, 20802, "arrow", arrow);
		if (dir_room_2)
			setSval(mobby, 20802, "dir_room_2", dir_room_2);
		if (dir_room_3)
			setSval(mobby, 20802, "dir_room_3", dir_room_3);
		act("You start setting up your barrage, lining up extra arrows for easy access...", true, actor, null, null, constants.TO_CHAR);
		act("$n starts lining up arrows along the ground, preparing $s barrage...", true, actor, null, null, constants.TO_ROOM);
		setSval(actor, 20802, "volleying", 1);
		actor.startTimer(4);
		var success = runTimer(actor);
		if (!success || actor.fighting)
		{
			setSval(actor, 20802, "volleying", 0);
			actor.send("\nYou stop the barrage.");
			act("\n$n stops the barrage and lowers " + self.name + ".", true, actor, null, null, constants.TO_ROOM);
			return;
		}
		waitpulse 1;
		var i = 0;
		var count = 0;
		while (i < total_barrage)
		{
			i++;
			count += 1;
			if (count == barrage_skill_score && barrage_timer > 1)
			{
			act(actor.cyan(constants.CL_SPARSE) + "\nYou begin to fire faster, finding a rhythm to the barrage..." + actor.normal(constants.CL_SPARSE), true, actor, null, null, constants.TO_CHAR);
			act("\n$n begins firing a little faster, finding $s rhythm...", true, actor, null, null, constants.TO_ROOM);
				count = 0;
				barrage_timer -= 0.5;
			}
			if (quiver)
			{
				if (quiver.getPval("arrows") < 1)
					arrow = "an arrow";
			}
			act("\nYou fire " + arrow + " high into the air!", true, actor, null, null, constants.TO_CHAR);
			act("\n$n fires " + arrow + " high into the air!", true, actor, null, null, constants.TO_ROOM);
			if (quiver)
			{
				if (quiver.getPval("arrows") > 0)
					quiver.setPval("arrows", (quiver.getPval("arrows") - 1), true);
			}
			mobby.comm("volleyarrows");
			actor.startTimer(barrage_timer);
			var success = runTimer(actor);
			if (!success || actor.fighting)
			{
				setSval(actor, 20802, "volleying", 0);
				actor.send("\nYou stop the barrage.");
				act("\n$n stops the barrage and lowers " + self.name + ".", true, actor, null, null, constants.TO_ROOM);
				return;
			}
			waitpulse 1;
		}
		act("\nYou lower " + self.name + ", completely exhausted from your barrage.", true, actor, null, null, constants.TO_CHAR);
		if (actor.questTaskProgress('Beware the Barrage')[1].quotient < 1)
			actor.updateJournalTask('Beware the Barrage',0);
		act("\n$n lowers " + self.name + ", ending $s barrage.", true, actor, null, null, constants.TO_ROOM);
		setSval(actor, 20802, "volleying", 0);
		return;
	}
}