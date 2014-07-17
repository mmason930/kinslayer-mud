var script20930 = function(self, actor, here, args, extra) {
	//CAPTURE global command (for capturing CTF towers)
	//Koradin - March 2010
	var arg_array = getArgList(args);
	if (!strn_cmp(arg_array[0],"capture",3))
	{
		_noblock;
		return;
	}
	else
		_block;
	var go = false;
	if (actor.position != constants.POS_STANDING)
	{
		actor.send("You're not in the right position to do that right now!");
		return;
	}
	if (!arg_array[1] || arg_array[2])
	{
		actor.send("What are you trying to capture?");
		return;
	}
	var inHand = false;
	for each (var thing in actor.inventory) {
		if (thing.vnum > 14199 && thing.vnum < 14205) {//ACTOR HAS FLAG AND IS TRYING TO SCORE
			var target = thing;
			inHand = true;
		}
	}
	if (!target)
		var target = getObjInListVis(actor,arg_array[1],actor.room.items);
	if (!target) {
		actor.send("What are you trying to capture?");
		return;
	}
	/**ARENA CAPTURE THE FLAG MATCH**/
	/** scoring with flag in hand **/
	if (inHand == true)
	{
		if (actor.arenaTeam == constants.ARENA_BLUE) {
			var homeBase = global.arenaCurrentMap.blueHomeBase;
			var mRace = ["Blue",constants.ARENA_BLUE];
			var teamCaps = global.arenaCurrentGame.blueCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_RED) {
			var homeBase = global.arenaCurrentMap.redHomeBase;
			var mRace = ["Red",constants.ARENA_RED];
			var teamCaps = global.arenaCurrentGame.redCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_YELLOW) {
			var homeBase = global.arenaCurrentMap.yellowHomeBase;
			var mRace = ["Yellow",constants.ARENA_YELLOW];
			var teamCaps = global.arenaCurrentGame.yellowCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_GREEN) {
			var homeBase = global.arenaCurrentMap.greenHomeBase;
			var mRace = ["Green",constants.ARENA_GREEN];
			var teamCaps = global.arenaCurrentGame.greenCaptures;
		}
		if (actor.room.vnum != homeBase) {  // Actor is not at his or her home base
			actor.send("You can't score the point unless you're at your home base!");
			return;
		}
		if (actor.fighting) {
			actor.send("You're too busy for that right now!");
			return;
		}
		actor.send("You capture the flag, scoring a point!");
		var scoreMsg = actor.name+" has captured the flag, scoring a point for the "+mRace[0]+" Team!";
		addArenaPoint(actor,"flagCaptures");
		actor.sps = (Math.floor(actor.maxSpell*.6));
		actor.shadowPoints = (Math.floor(actor.maxShadow*.6));
		arenaEcho(scoreMsg);
		var flagCaps = getSval(actor,20930,"flagCaps");
		if (!flagCaps)
			flagCaps = 0;
		setSval(actor,20930,"flagCaps", flagCaps+1);
		if (actor.arenaTeam == constants.ARENA_RED) {
			global.arenaCurrentGame.redCaptures += 1;
			teamCaps = global.arenaCurrentGame.redCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_BLUE) {
			global.arenaCurrentGame.blueCaptures += 1;
			teamCaps = global.arenaCurrentGame.blueCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_YELLOW) {
			global.arenaCurrentGame.yellowCaptures += 1;
			teamCaps = global.arenaCurrentGame.yellowCaptures;
		}
		else if (actor.arenaTeam == constants.ARENA_GREEN) {
			global.arenaCurrentGame.greenCaptures += 1;
			teamCaps = global.arenaCurrentGame.greenCaptures;
		}
		if (teamCaps == global.arenaCurrentGame.limit) { // Goal met, game's over
			var winner = "The "+mRace[0]+" Team";
			arenaEcho(winner+" has won the Match!");
			for each (var play in global.arenaPlayers) {
				if (play.arenaTeam == mRace[1]) 
					addArenaPoint(play, "teamWins");
				else 
					addArenaPoint(play, "teamLosses");
			}
			resetArena();
			return;
		}
		resetFlag(target, actor);
		return;
	}
	/** capturing flag on ground **/
	if (target.vnum > 14199 && target.vnum < 14205) {
		if (target.vnum == 14200) { //Trying to capture the BLUE flag
			var flagRace = constants.ARENA_BLUE;
			var cityRooms = global.arenaCurrentMap.blueCityRooms;
			var mRace = "Blue";
			var teamRets = global.arenaCurrentGame.blueReturns;
		}
		else if (target.vnum == 14201) { //Trying to capture the RED flag
			var flagRace = constants.ARENA_RED;
			var cityRooms = global.arenaCurrentMap.redCityRooms;
			var mRace = "Red";
			var teamRets = global.arenaCurrentGame.redReturns;
		}
		else if (target.vnum == 14203) { //Trying to capture the YELLOW flag
			var flagRace = constants.ARENA_YELLOW;
			var cityRooms = global.arenaCurrentMap.yellowCityRooms;
			var mRace = "Yellow";
			var teamRets = global.arenaCurrentGame.yellowReturns;
		}
		else if (target.vnum == 14204) { //Trying to capture the GREEN flag
			var flagRace = constants.ARENA_GREEN;
			var cityRooms = global.arenaCurrentMap.greenCityRooms;
			var mRace = "Green";
			var teamRets = global.arenaCurrentGame.greenReturns;
		}
		else {
			actor.send("There's no flag here!");
			return;
		}
		if (actor.arenaTeam == flagRace && arrContains(cityRooms,actor.room.vnum)) {
			actor.send("Your flag is already in your city - you can't return it here!");
			return;
		}
		var capRet = "capture";
		if (actor.arenaTeam == flagRace)
			capRet = "return";
		actor.send("You reach for the flag...");
		act("$n reaches for the flag...",true,actor,null,null,constants.TO_ROOM);
		actor.startTimer(2);
		var success = runTimer(actor);
		if (!success || !arrContains(actor.room.items, target)) {
			actor.send("You give up the attempt.");
			act("$n gives up the attempt to "+capRet+" the flag.",true,actor,null,null,constants.TO_ROOM);
			return;
		}
		if (actor.fighting) {
			actor.send("You're too busy for that right now!");
			actor.send("You give up the attempt.");
			act("$n gives up the attempt to "+capRet+" the flag.",true,actor,null,null,constants.TO_ROOM);
			return;
		}
		if (actor.arenaTeam == flagRace) { // ACTOR IS RETURNING FLAG
			actor.send("You return the flag.");
			addArenaPoint(actor, "flagReturns");
			arenaEcho("The "+mRace+" Team's flag has been returned by "+actor.name+".");
			var flagRets = getSval(actor,20930,"flagRets");
			if (!flagRets)
				flagRets = 0;
			setSval(actor,20930,"flagRets",flagRets+1);
			if (actor.arenaTeam == constants.ARENA_RED)
				global.arenaCurrentGame.redReturns += 1;
			else if (actor.arenaTeam == constants.ARENA_BLUE)
				global.arenaCurrentGame.blueReturns += 1;
			else if (actor.arenaTeam == constants.ARENA_YELLOW)
				global.arenaCurrentGame.yellowReturns += 1;
			else if (actor.arenaTeam == constants.ARENA_GREEN)
				global.arenaCurrentGame.greenReturns += 1;
			resetFlag(target);
			return;
		}
		actor.send("\nYou snatch the flag out of the air!");
		act("$n snatches the flag out of the air!",true,actor,null,null,constants.TO_ROOM);
		arenaEcho(capFirstLetter(target.name)+" has been captured!");
		target.moveToChar(actor);
		actor.attach(14215);
		actor.sps = 0;
		actor.shadowPoints = 0;
		if (actor.eq(constants.WEAR_WIELD))
			actor.eq(constants.WEAR_WIELD).moveToChar(actor);
		return;
	}
	/** END ARENA CTF SCRIPT **/
	if (actor.race == constants.RACE_HUMAN)
	{
		var items_to_cap = [20972,20975,20978,20980];
		var items_yours = [20971,20974,20977,20979];
		var race = "Light";
		var vnum = target.vnum;
	}
	else if (actor.race == constants.RACE_TROLLOC)
	{
		var items_to_cap = [20971,20974,20977,20979];
		var items_yours = [20972,20975,20978,20980];
		var race = "Dark";
		var vnum = target.vnum + 1;
	}
	for each (var item in items_to_cap)
	{
		if (target.vnum == item)
			var go = true;
	}
	if (go == false)
	{
		for each (var bad_item in items_yours)
		{
			if (target.vnum == bad_item)
			{
				actor.send("You already own " + target.name + "!");
				return;
			}
		}
		actor.send("You can't capture " + target.name + ".");
		return;
	}
	var skill = actor.getSkill(getSkillVnum('Survival')) / 10;
	var timer = vnum - 20963 - skill;
	if (timer < 1)
		timer = 1;
	var x = 1;
	act("You begin an attempt to capture " + target.name + "!", true, actor, null, null, constants.TO_CHAR);
	act("$n begins an attempt to capture " + target.name + "!", true, actor, null, null, constants.TO_ROOM);
	while (x < 10)
	{
		actor.startTimer(timer)
		var success = runTimer(actor);
		if (!success)
		{
			actor.send("You abandon your attempt to capture " + target.name + ".");
			return;
		}
		if (!target)
		{
			actor.send("Your target isn't here anymore!");
			return;
		}
		act("\n"+capFirstLetter(target.name) + " is being contested by the " + race + "!  Progress: " + (x * 10) + "%", true, null, target, null, constants.TO_ROOM);
		waitpulse 1;
		x += 1;
	}
	actor.startTimer(timer)
	var success2 = runTimer(actor);
	if (!success2)
	{
		actor.send("You abandon your attempt to capture " + target.name + ".");
		return;
	}
	if (!target)
	{
		actor.send("Your target isn't here anymore!");
		return;
	}
	act("\nYou have captured " + target.name + "!", true, actor, null, null, constants.TO_CHAR);
	act("\n"+capFirstLetter(target.name) + " has been captured by the " + race + "!", true, actor, null, null, constants.TO_ROOM);
	if (target.vnum == 20971 || target.vnum == 20972)
	{
		if (target.vnum == 20971)
		{
			var new_target = 20972;
			var flame = "green";
		}
		else if (target.vnum == 20972)
		{
			var new_target = 20971;
			var flame = "yellow";
		}
		var message = "The campfire starts burning a bright " + flame + " color...";
	}
	else if (target.vnum == 20974 || target.vnum == 20975)
	{
		if (target.vnum == 20974)
		{
			var new_target = 20975;
			var flame = "green";
		}
		else if (target.vnum == 20975)
		{
			var new_target = 20974;
			var flame = "yellow";
		}
		var message = "The bonfire starts burning a bright " + flame + " color...";
	}
	else if (target.vnum == 20977 || target.vnum == 20978)
	{
		if (target.vnum == 20977)
		{
			var new_target = 20978;
			var hut_desc = "hut";
		}
		else if (target.vnum == 20978)
		{
			var new_target = 20977;
			var hut_desc = "shack";
		}
		var message = capFirstLetter(target.name) + " takes on more of a " + hut_desc + " appearance...";
	}
	else if (target.vnum == 20979 || target.vnum == 20980)
	{
		if (target.vnum == 20979)
		{
			var new_target = 20980;
			var archer = "A grinning trolloc archer";
		}
		else if (target.vnum == 20980)
		{
			var new_target = 20979;
			var archer = "An archer of the Borderguard";
		}
		var message = archer + " scrambles up to the top of the tower...";
	}
	act("\n"+message, true, null, target, null, constants.TO_ROOM);
	here.loadObj(new_target);
	target.extract();
}