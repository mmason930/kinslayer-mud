var script20960 = function(self, actor, here, args, extra)
{
	//BUILD command - global command (for fire and shelter - hp, sps and mv regen)
//Koradin - March 2010
var arg_array = getArgList(args);
if (!strn_cmp(arg_array[0],"build",2))
{
	_noblock;
	return;
}
else
	_block;
if (actor.room.zoneVnum == global.arenaZoneVnum || arrContains(global.arenaPlayers, actor)) {
	actor.send("The BUILD command is disabled inside the Arena.");
	return;
}
if (actor.position != constants.POS_STANDING)
{
	actor.send("You're not in the right position to do that right now!");
	return;
}
if (!arg_array[1] || arg_array[2])
{
	actor.send(" ");
	actor.send("You can currently build: FIRE, BONFIRE, SHELTER, TOWER");
	return;
}
var skill = actor.getSkill(getSkillVnum('Survival'));
var req_items = 0;
var item_array = [];
var fail_score = 0;
var existing_pile = false;
var existing_shelter = false;
var existing_fire = false;
var pile_found = false;
if (skill > 90)
{
	var fire_num_req = 2;
	var bonfire_num_req = 1;
	var shelter_num_req = 3;
	var tower_num_req = 7;
	var starting_timer = 3;
	var timer = (100 - skill) / 5;
}
else if (skill > 70)
{
	var fire_num_req = 3;
	var bonfire_num_req = 2;
	var shelter_num_req = 4;
	var tower_num_req = 8;
	var starting_timer = 4;
	var timer = (100 - skill) / 8;
	fail_score += 40;
}
else
{
	var fire_num_req = 4;
	var bonfire_num_req = 3;
	var shelter_num_req = 5;
	var starting_timer = 5;
	var timer = (100 - skill) / 10;
	fail_score += 60;
}
if (arg_array[1] == "fire")
{
	var building_array = [9022,20971,20972,20974,20975,23000];
	if (actor.race == constants.RACE_HUMAN)
		var end_item = 20971;
	else if (actor.race == constants.RACE_TROLLOC)
		var end_item = 20972;
	var skill_req = 20;
	var item_req = 1800;
	var pile_item = 20973;
	var num_req = fire_num_req;
	var branch_type = "frail";
	var purpose = "to use as tinder...";
	var message1_you = "\nYou dig out a shallow impression in the ground, assembling your tinder.";
	var message1_room = "\n$n digs out a shallow impression in the ground, assembling $s tinder.";
	var message2_you = "\nTaking two of the branches, you rub them together in the hole to create friction...";
	var message2_room = "\n$n grabs two branches and starts furiously rubbing them together to create friction...";
	var message3_you = "\nSmoke starts rising from the hole, so you add more kindling and blow softly...";
	var message3_room = "\nSmoke starts rising, and $n blows softly on the tinder while adding more kindling...";
	var endmsg_you = "\nThe fire catches and roars to life, starting to burn on its own.";
	var endmsg_room = "\n$n smiles as the fire catches and roars to life, starting to burn on its own.";
}
else if (arg_array[1] == "shelter")
{
	timer *= 1.5;
	var building_array = [20978,20977,20979,20980];
	if (actor.race == constants.RACE_HUMAN)
		var end_item = 20977;
	else if (actor.race == constants.RACE_TROLLOC)
		var end_item = 20978;
	var skill_req = 50;
	var item_req = 20970;
	var pile_item = 20976;
	var num_req = shelter_num_req;
	var branch_type = "sturdy";
	var purpose = "to lash them together with...";
	var message1_you = "\nYou clear an area for the shelter and thrust the branches into the ground, creating a frame.";
	var message1_room = "\n$n clears an area for the shelter and thrusts the branches into the ground, creating a frame.";
	var message2_you = "\nAfter lashing some cross pieces on, you begin weaving brush onto the sides of the frame.";
	var message2_room = "\n$n weaves brush onto the frame using cross pieces and light vegetation to secure a protective area...";
	var message3_you = "\nYou cover the shelter with thick vegetation and begin securing the roof against the weather...";
	var message3_room = "\n$n secures the roof of the shelter against the weather and begins finishing up.";
	var endmsg_you = "\nYou test the structure to make sure it will provide protection, then stand back and admire your work.";
	var endmsg_room = "\nAfter a few quick tests, $n stands back and admires the shelter.";
}
else if (arg_array[1] == "tower")
{
	timer *= 3;
	var building_array = [20979,20980];
	if (actor.race == constants.RACE_HUMAN)
	{
		var pile_item = 20977;
		var shelter_type = "shack";
		var end_item = 20979;
	}
	else if (actor.race == constants.RACE_TROLLOC)
	{
		var pile_item = 20978;
		var shelter_type = "hut";
		var end_item = 20980;
	}
	var skill_req = 70;
	var item_req = 20970;
	var num_req = tower_num_req;
	var branch_type = "sturdy";
	var message1_you = "\nYou clear the roof off the shelter and begin building on, securing supports for the tower.";
	var message1_room = "\n$n clears the roof off the shelter and begins building on, securing supports for a tower.";
	var message2_you = "\nAfter testing the supports, you start adding a new floor above for a bird's eye view.";
	var message2_room = "\n$n finishes the supports and begins building a new floor above...";
	var message3_you = "\nYou build a simple trapdoor and ladder, and finish up the guard rail along the top of the tower...";
	var message3_room = "\n$n builds a ladder and trapdoor, then starts providing extra defenses to the tower including guard rail and roof...";
	var endmsg_you = "\nThe tower comes together and looks solid, fit for defending the area.";
	var endmsg_room = "\nThe tower takes shape and looks solid enough to provide ample defense for the area.";
}
else if (arg_array[1] == "bonfire")
{
	var building_array = [20974,20975];
	var skill_req = 30;
	var item_req = 1800;
	if (actor.race == constants.RACE_HUMAN)
	{
		var pile_item = 20971;
		var end_item = 20974;
	}
	else if (actor.race == constants.RACE_TROLLOC)
	{
		var pile_item = 20972;
		var end_item = 20975;
	}
	var num_req = bonfire_num_req;
	var branch_type = "frail";
	var message1_you = "\nYou take a branch and use it to start adjusting the logs on the campfire...";
	var message1_room = "\n$n takes a branch and uses it to start adjusting the logs on the campfire...";
	var message2_you = "\nAfter arranging the base, you begin adding more branches in a teepee around the fire.";
	var message2_room = "\n$n arranges more branches in a teepee around the edge of the fire.";
	var message3_you = "\nThe flames catch the new logs and start stretching higher into the sky...";
	var message3_room = "\nThe flames catch the new logs and start stretching higher into the sky...";
	var endmsg_you = "\nYou smile as the bonfire flames dart wildly into the air and give off more warmth.";
	var endmsg_room = "\n$n smiles as the bonfire flames dart wildly into the air and give off more warmth.";
}
else
{
	actor.send("You can currently build: FIRE, BONFIRE, SHELTER, TOWER");
	actor.send("Build what?");
	return;
}
if (skill < skill_req)
{
	actor.send("Learn more Survival techniques before attempting to build a " + arg_array[1] + "!");
	return;
}
for each (var room_item in actor.room.items)
{
	for each ( var item_num in building_array)
	{
		if (room_item.vnum == item_num)
		{
			actor.send("There's already another " + arg_array[1] + " here!");
			return;
		}
		if (room_item.vnum == 20973 || room_item.vnum == 20976)
			existing_pile = true;
		if (actor.race == constants.RACE_HUMAN)
		{
			if (room_item.vnum == 20977)
				existing_shelter = true;
			if (room_item.vnum == 20971)
				existing_fire = true;
		}
		else if (actor.race == constants.RACE_TROLLOC)
		{
			if (room_item.vnum == 20978)
				existing_shelter = true;
			if (room_item.vnum == 20972)
				existing_fire = true;
		}
		if (room_item.vnum == 991 && arg_array[1] == "fire")
			num_req -= 1;
	}
}
if (existing_pile == false)
{
	var maybe_drop_you = " drop the branches in a pile and";
	var maybe_drop_room = " drops a few " + branch_type + " branches in a pile and";
}
else
{
	var maybe_drop_you = "";
	var maybe_drop_room = "";
}
if (actor.room.sector == constants.SECT_CITY)
{
	actor.send("Why not just go to the inn instead? You're already inside the city limits!");
	return;
}
if (actor.room.sector == constants.SECT_INSIDE)
{
	actor.send("You can't build a " + arg_array[1] + " indoors...");
	return;
}
if (actor.room.sector == constants.SECT_ROAD)
{
	actor.send("You can't build a " + arg_array[1] + " in the middle of the road...");
	return;
}
if (actor.room.sector == constants.SECT_WATER_SWIM || actor.room.sector == constants.SECT_WATER_NOSWIM || actor.room.sector == constants.SECT_UNDERWATER)
{
	actor.send("It's a little too wet to build here...");
	return;
}
if (actor.room.sector == constants.SECT_FLYING)
{
	actor.send("While flying? I don't think so.");
	return;
}
if (actor.room.sector == constants.SECT_MOUNTAIN)
	fail_score -= 10;
else if (actor.room.sector == constants.SECT_HILLS)
	fail_score -= 20;
else if (actor.room.sector == constants.SECT_FOREST)
	fail_score -= 40;
else if (actor.room.sector == constants.SECT_FIELD)
	fail_score -= 30;
for each (var item in actor.inventory)
{
	if (item.vnum == item_req)
	{
		req_items += 1;
		if (req_items <= num_req)
			item_array.push(item);
	}
}
if (arg_array[1] == "fire" || arg_array[1] == "shelter")
{
	if (req_items < num_req && existing_pile == false)
	{
		actor.send("At your skill level, you need " + num_req + " " + branch_type + " branches to build a " + arg_array[1] + "!");
		actor.send("...or use an existing pile of branches left by someone else.");
		return;
	}
	act("You" + maybe_drop_you + " start searching for materials " + purpose, true, actor, null, null, constants.TO_CHAR);
	act("$n" + maybe_drop_room + " starts searching for materials " + purpose, true, actor, null, null, constants.TO_ROOM);
	if (existing_pile == false)
	{
		for each ( var log in item_array)
			log.extract();
		actor.room.loadObj(pile_item);
	}
	actor.startTimer(starting_timer);
	var success = runTimer(actor);
	if (!success)
	{
		act("You give up the " + arg_array[1] + " attempt and leave the branches in a pile.", true, actor, null, null, constants.TO_CHAR);
		act("$n gives up the " + arg_array[1] + " attempt and leaves the branches in a pile.", true, actor, null, null, constants.TO_ROOM);
		return;
	}
	if (fail_score > random(100))
	{
		act("\nYou come back empty handed after your first attempt, so you keep searching for materials...", true, actor, null, null, constants.TO_CHAR);
		act("\n$n sighs and keeps on the search for useful materials...", true, actor, null, null, constants.TO_ROOM);
		actor.startTimer(starting_timer);
		var success2 = runTimer(actor);
		if (!success2)
		{
			act("You give up the " + arg_array[1] + " attempt and leave the branches in a pile.", true, actor, null, null, constants.TO_CHAR);
			act("$n gives up the " + arg_array[1] + " attempt and leaves the branches in a pile.", true, actor, null, null, constants.TO_ROOM);
			return;
		}
	}
	act("\nYou return with materials in hand and begin setting up your " + arg_array[1] + "...", true, actor, null, null, constants.TO_CHAR);
	act("\n$n returns with materials in hand and begins setting up $s " + arg_array[1] + "...", true, actor, null, null, constants.TO_ROOM);
	waitpulse 1;
}
else if (arg_array[1] == "tower")
{
	if (req_items < num_req || existing_shelter == false)
	{
		actor.send("At your skill level, you need " + num_req + " " + branch_type + " branches and an existing shelter to build a " + arg_array[1] + " off of!");
		return;
	}
}
else if (arg_array[1] == "bonfire")
{
	if (req_items < num_req || existing_fire == false)
	{
		actor.send("At your skill level, you need " + num_req + " " + branch_type + " branches and an existing campfire to build a " + arg_array[1] + " off of!");
		return;
	}
}
actor.startTimer(timer);
var success3 = runTimer(actor);
if (!success3)
{
	act("You give up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_CHAR);
	act("$n gives up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_ROOM);
	return;
}
act(message1_you, true, actor, null, null, constants.TO_CHAR);
act(message1_room, true, actor, null, null, constants.TO_ROOM);
waitpulse 1;
actor.startTimer(timer);
var success4 = runTimer(actor);
if (!success4)
{
	act("You give up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_CHAR);
	act("$n gives up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_ROOM);
	return;
}
act(message2_you, true, actor, null, null, constants.TO_CHAR);
act(message2_room, true, actor, null, null, constants.TO_ROOM);
waitpulse 1;
actor.startTimer(timer);
var success5 = runTimer(actor);
if (!success5)
{
	act("You give up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_CHAR);
	act("$n gives up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_ROOM);
	return;
}
act(message3_you, true, actor, null, null, constants.TO_CHAR);
act(message3_room, true, actor, null, null, constants.TO_ROOM);
waitpulse 1;
actor.startTimer(timer);
var success6 = runTimer(actor);
if (!success6)
{
	act("You give up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_CHAR);
	act("$n gives up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_ROOM);
	return;
}
for each (var itm in actor.room.items)
{
	if (itm.vnum == pile_item)
	{
		var pile_found = true;
		var pile = itm;
	}
}
if (pile_found == false)
{
	act("You give up the " + arg_array[1] + " attempt when you realize your materials are no longer here.", true, actor, null, null, constants.TO_CHAR);
	act("$n gives up the " + arg_array[1] + " attempt.", true, actor, null, null, constants.TO_ROOM);
	return;
}
act(endmsg_you, true, actor, null, null, constants.TO_CHAR);
act(endmsg_room, true, actor, null, null, constants.TO_ROOM);
if (arg_array[1] == "tower" || arg_array[1] == "bonfire")
{
	for each ( var log in item_array)
		log.extract();
}
pile.extract();
actor.room.loadObj(end_item);

};

