var script20813 = function(self, actor, here, args, extra) {
	/* Fogel, January 2011 - The "shoot" command for bows. See script 20802 for version 1 */
	// Initialize variables and constants
	var argArray = getArgList(args);
	var weaponSkill = actor.getSkill(getSkillVnum("Bow"));
	var weapon = actor.eq(constants.WEAR_WIELD);
	var ch = actor;
	var vict = 0;
	var room = 0;
	var dmg = 0;
	var shotStage = 0;
	var dir = -1;
	var canBeStunned = true;
	var damMsg = "";
	var dirMsg = "";
	var arrow = "an arrow";
	var arrowEffects = [];
	//Check for quiver and update arrow/effects if necessary.
	var backItem = actor.eq(constants.WEAR_BACK);
	if (backItem)
	{
		for (var _autoKey in backItem.namelist.split(" ")) {
			var word = backItem.namelist.split(" ")[_autoKey];
			if (word == "quiverforbows")
				var quiver = backItem;
		}
	}
	if (quiver)
	{
		if (quiver.getPval("arrows") > 0)
		{
			if (quiver.vnum == 1720)// a studded leather quiver
			{
				arrow = "a diamond-tipped arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " reflects blinding light as it flies through the air");
				//arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_SEVERE_WOUND)) {act(arrow + " buries itself in your flesh, deeply wounding you!", true, vict, null, null, constants.TO_CHAR);} } );
				//arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_SEVERE_WOUND)) {act("$n grunts in pain as " + arrow + " buries itself inside $m!", true, vict, null, null, constants.TO_ROOM);} } );
				//arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_SEVERE_WOUND)) {vict.affect(constants.AFF_SEVERE_WOUND, 0, 0);} } );
			}
			else if (quiver.vnum == 1721)// a well-stitched cloth quiver
			{
				arrow = "a streamlined arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " flies silently with precision and speed");
			}
			else if (quiver.vnum == 1722)// a quiver of scorched leather
			{
				arrow = "a burning arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " leaves a trail of smoke as it streaks through the air");
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BURN)) {vict.burnedBy = actor;} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BURN)) {act("Flames erupt upon you as " + arrow + " finds its mark!", true, vict, null, null, constants.TO_CHAR);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BURN)) {act("$n erupts in flames as " + arrow + " hits $m!", true, vict, null, null, constants.TO_ROOM);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BURN)) {vict.affect(constants.AFF_BURN, 1, 0);} } );
			}
			else if (quiver.vnum == 1723)// a thick fur-lined quiver
			{
				arrow = "an icy steel arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " flashes crystal blue light as it flies through the air");
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_CHILL)) {act("You shiver in frosty pain as " + arrow + " shatters against your skin!", true, vict, null, null, constants.TO_CHAR);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_CHILL)) {act("$n shivers uncontrollably as " + arrow + " hits $m!", true, vict, null, null, constants.TO_ROOM);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_CHILL)) {vict.affect(constants.AFF_CHILL, 1, 0);} } );
			}
			else if (quiver.vnum == 1724)// a wooden quiver with steel buckles
			{
				arrow = "an explosive arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " blasts through the air, creating a fireworks display");
				arrowEffects.push( function() {act("You are knocked back as " + arrow + " blows up in your face!", true, vict, null, null, constants.TO_CHAR);} );
				arrowEffects.push( function() {act("$n is knocked back as " + arrow + " blows up in $s face!", true, vict, null, null, constants.TO_ROOM);} );
				arrowEffects.push( function() {vict.lag(5);} );
				var effect_roll = random(1,3);
				if (effect_roll == 1)
				{
					arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DEAF)) {act("The world goes silent around you.", true, vict, null, null, constants.TO_CHAR);} } );
					arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DEAF)) {vict.affect(constants.AFF_DEAF, 1, 0);} } );
				}
				else if (effect_roll == 2)
				{
					arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BLIND)) {act("The world blurs as your sight is ripped away.", true, vict, null, null, constants.TO_CHAR);} } );
					arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_BLIND)) {vict.affect(constants.AFF_BLIND, 1, 0);} } );		
				}
			}
			else if (quiver.vnum == 1725)// a sickly green quiver
			{
				arrow = "an arrow dipped in venom";
				var arrow_fly_message = (capFirstLetter(arrow) + " hisses through the air in a cloud of poison");
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_POISON)) {act("You feel the venom seep into your bones!", true, vict, null, null, constants.TO_CHAR);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_POISON)) {act("The color drains from $n's face as " + arrow + " hits $m!", true, vict, null, null, constants.TO_ROOM);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_POISON)) {vict.affect(constants.AFF_POISON, 1, 0);} } );
			}
			else if (quiver.vnum == 1726)// a rotting black quiver
			{
				arrow = "a decaying arrow";
				var arrow_fly_message = (capFirstLetter(arrow) + " drips disease as it coasts through the air");
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DECAY)) {vict.decayedBy = actor;} } );		
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DECAY)) {act("You nearly throw up as " + arrow + " contaminates you!", true, vict, null, null, constants.TO_CHAR);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DECAY)) {act("$n starts to feel sick as " + arrow + " hits $m!", true, vict, null, null, constants.TO_ROOM);} } );
				arrowEffects.push( function() {if (!vict.affectedBy(constants.AFF_DECAY)) {vict.affect(constants.AFF_DECAY, 0, 0);} } );
			}
		}
	}
	var shotStageTimers = [ 0.5, 0.4, 0.3, 0.3 ]; // edited by Koradin 7/11/17 to lower stage timers to make bows a viable main weapon class
	var shotStageCharMsgs = [ "You nock " + arrow + " and begin to study the area.",
							  "\nYou start to draw " + arrow + " back.",
							  "\nYou draw " + arrow + " back further, further increasing your shot's power.",
							  "\nYou draw " + arrow + " back as far as you can, hoping to hit for massive damage." ];
	var shotStageVictMsgs = [ "$n nocks " + arrow + " and begins to study the area.",
							  "$n begins to stare at you intently, a look of concentration spreading across $s face!",
							  "$n starts to draw " + arrow + " further back, $s muscles rippling powerfully!",
							  "$n shifts $s aim slightly, studying your body carefully!" ];
	var shotStageRoomMsgs = [ "$n nocks " + arrow + " and begins to study the area.",
							  "$n begins to stare intently, a look of concentration spreading across $s face.",
							  "$n starts to draw " + arrow + " further back, $s muscles rippling powerfully.",
							  "$n shifts $s aim slightly, looking for a weak point in $s victim's armor." ];
	_block;
	// Initial failure conditions
	if( !weaponSkill ) { 
		// The "Bows" skill isn't practiced.
		ch.send("You have no idea how.");
		return;
	}
	if( !weapon || weapon.weaponType != constants.WEAPON_BOW ) { 
		// Not wielding a Bow
		ch.send("You need to be wielding a bow to do that.");
		return;
	}
	// Get vict and vict room
	if( argArray.length == 2 ) {
		// Shooting in the same room.
		vict = ch.getCharRoomVis(argArray[1]);
		room = here;
	}
	else if( argArray.length == 3 ) {
		// Trying to shoot to an adjacent room.
		var arg3 = argArray[2].toLowerCase();
		switch(arg3){
			case "w" || "we" || "wes": arg3 = "west"; break;
			case "n" || "no" || "nor" || "nort": arg3 = "north"; break;
			case "e" || "ea" || "eas": arg3 = "east"; break;
			case "s" || "so" || "sou" || "sout": arg3 = "south"; break;
		}
		dir = dirFromText(arg3);
		room = here.direction(dir);
		if( !room ) {
			ch.send("You can't shoot " + argArray[2] + ".");
			return;
		}
		vict = getCharInListVis(ch, argArray[1], room.people);
	}
	else if( argArray.length == 1 && ch.fighting ) {
		vict = ch.fighting;
		room = here;
	}
	else {
		ch.send("Shoot who where?");
		return;
	}
	// More failure conditions.
	if( !vict ) {
		// Victim doesn't exist.
		ch.send(capFirstLetter(argArray[1]) + " doesn't seem to be here.");
		return;
	}
	if( room.isFlagged(constants.ROOM_PEACEFUL) ) {
		// Trying to shoot in a peace room.
		ch.send("This room just has such a peaceful, easy feeling...");
		return;
	}
	if( ch.fighting && here != room ) {
		// Can't shoot to an adjacent room while in combat.
		ch.send("You are too busy to aim properly!");
		return;
	}
	if( ch == vict ) {
		// We don't allow suicide.
		ch.send("Why would you shoot yourself?!");
		return;
	}
	if( here.doorIsClosed(dir) || here.doorIsLocked(dir) ) {
		// Can't shoot through a door.
		actor.send("But you can't see through the " + here.doorName(dir) + "!");
		return;
	}
	if ( vict.mobFlagged(constants.MOB_SMOB) && here != room) {
		// We can't make SMOBing too easy!
		actor.send(vict.name + " would probably come over here and tear you to pieces if you did that!");
		return;
	}
	// Initial conditions have been set, let's start shooting!
	// Run a timer multiple times, powering up the shot each time a timer completes.
	while( shotStage < shotStageTimers.length ) {
		// The timer and messages for each shotStage are set in arrays that are initialized at the top of the script.
		var timer = shotStageTimers[shotStage];
		/** begin edit out by Koradin 7/11/17: timer was too long to make bows viable as a main weapon class and messages were spammy 
		if( here != room && arrow != "a streamlined arrow") {
			timer *= 2.0;
		}
		act( shotStageCharMsgs[shotStage], true, ch, weapon, vict, constants.TO_CHAR );
		if( shotStage < 1 )
			act( shotStageRoomMsgs[shotStage], true, ch, weapon, vict, constants.TO_NOTVICT );
		if( here == room && here == vict.room) {
			act( shotStageVictMsgs[shotStage], true, ch, weapon, vict, constants.TO_VICT );
		}
		end Koradin edit 7/11/17 **/
		// Start the timer.
		ch.startTimer(timer);
		var success = runTimer(ch);
		if( success && !ch.isBashed ) {
			//if( vict.room == room ) {
				// Victim hasn't moved, increase the shot power.
				shotStage++;
			//}
			//else {
				// Victim moved, stop aiming.
				//act( "Your victim seems to have moved.", true, ch, weapon, vict, constants.TO_CHAR );
				//act( "$n lowers $p, $s target no longer in $s sights.", true, ch, weapon, vict, constants.TO_ROOM );
				//waitpulse 1;
				//return;
			//}		
		}
		else {
			if( ch.isBashed ) {
				return;
			}
			// Timer cancelled, time to shoot.
			break;
		}
	}
	// Prevent people from instantly shooting arrows.
	if( shotStage < 1 ) {
		//act( "$n lowers $p.", true, ch, weapon, vict, constants.TO_ROOM );
		return;
	}
	if( vict.room != room ) {
		// Victim moved, stop aiming.
		act( "Your victim seems to have moved.", true, ch, weapon, vict, constants.TO_CHAR );
		//act( "$n lowers $p, $s target no longer in $s sights.", true, ch, weapon, vict, constants.TO_ROOM );
		waitpulse 1;
		return;
	}
	// Still can't shoot through doors.
	if( here.doorIsClosed(dir) || here.doorIsLocked(dir) ) {
		actor.send("You see the " + here.doorName(dir) + " is now closed, so you lower the bow.");
		return;
	}
	// Shoot the arrow!
	if (quiver)
	{
		if (quiver.getPval("arrows") > 0)
			quiver.setPval("arrows", (quiver.getPval("arrows") - 1), true);
	}
	act( "You take aim and fire " + arrow + " into the air!", true, ch, weapon, vict, constants.TO_CHAR );
	act( "\n$n takes aim and fires " + arrow + " from $p!", true, ch, weapon, vict, constants.TO_NOTVICT );
	if( here == room ) {
		act( "\n$n takes aim and fires " + arrow + " at you from $p!", true, ch, weapon, vict, constants.TO_VICT );
	}
	wait 4;
	// Victim moved, the arrow fails.
	if( vict.room != room ) {
		room.echo(capFirstLetter(arrow) + " hits the ground harmlessly.");
		return;
	}
	var off = arrowOffenseRoll(ch, shotStage, arrow);
	var def = arrowDefenseRoll(vict, arrow);
	if( off > def ) {
		// Arrow hits, calculate damage and messages
		dmg = calculateDamage( weapon, vict.absorb, shotStage, arrow );
		damMsg = damageMessage(dmg);
		dirMsg = directionMessage( revDir(dir) );
		// Send messages
		act(ch.green(constants.CL_SPARSE) + capFirstLetter(arrow) + " hits $N" + damMsg + ch.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_CHAR);
		if( dir )
			act(vict.red(constants.CL_SPARSE) + capFirstLetter(arrow) + dirMsg + " hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
		else
			act(vict.red(constants.CL_SPARSE) + capFirstLetter(arrow) + " hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
		act(capFirstLetter(arrow) + dirMsg + " hits $n" + damMsg, true, vict, weapon, ch, constants.TO_NOTVICT);
		// Apply damage and effects
		for(i = 0; i < arrowEffects.length; i++)
			arrowEffects[i]();
		vict.damage(dmg, ch);
		var p = ch.faceoff(vict);
		if( !ch.fighting && p ) {
			ch.stopFighting();
			ch.setFighting(p);
		}
	}
	else {
		// Arrow misses
		if (vict.shieldBlock) {
			act(capFirstLetter(arrow) + " glances off your raised shield, ricocheting to the ground.", true, vict, weapon, ch, constants.TO_CHAR);
			act(capFirstLetter(arrow) + " glances off $n's raised shield, ricocheting to the ground.", true, vict, weapon, ch, constants.TO_ROOM);
		}
		else {
			act("You dodge to the side, letting " + arrow + " fly past.", true, vict, weapon, ch, constants.TO_CHAR);
			act("$n suddenly dodges to the side, letting " + arrow + " fly past.", true, vict, weapon, ch, constants.TO_ROOM);
		}
		return;
	}
	/**** Functions used during shooting ****/
	// Calculate shot damage. For now it's going to be based on weapon damage, weapon weight and how charged the shot is.
	function calculateDamage(weapon, abs, stage, arrow) {
		var hitdam = random(weapon.value(constants.VALUE_WEAPON_DAM_LOW), weapon.value(constants.VALUE_WEAPON_DAM_HIGH));
		if( stage > 1 ) {
			hitdam += weapon.weight;
		}
		if (stage > 2 ) {
			hitdam *= 1.3;
		}
		if( stage > 3 ) {
			hitdam *= 1.3;
		}
		if (arrow == "a diamond-tipped arrow")
			hitdam *= 1.1;
		hitdam *= (100 - abs) / 100.0;
		return hitdam;
	}
	// Return a message string for the amount of damage the arrow does.
	function damageMessage(dmg) {
		if (dmg < 5)
			return " lightly.";
		else if (dmg < 10)
			return ".";
		else if (dmg < 15)
			return " hard.";
		else if (dmg < 20)
			return " very hard.";
		else if (dmg < 25)
			return " extremely hard.";
		else if (dmg < 30)
			return ", spraying blood everywhere!";
		else if (dmg < 40)
			return " with intense force, spraying blood everywhere!";
		else if (dmg < 50)
			return " with BRUTAL force, spraying blood everywhere!";
		else
			return " with DEADLY force, spraying blood everywhere!";
	}
	// Return a message string for the direction the arrow came from.
	function directionMessage(dir) {
		var dirString = " from ";
		if (dir == constants.UP)
			dirString += "above ";
		else if(dir == constants.DOWN)
			dirString += "below ";
		else if(dir != -1)
			dirString += "the " + dirToText(dir);
		else
			return "";
		return dirString;
	}
	// Calculate the offensive roll for the arrow.
	function arrowOffenseRoll(ch, shotStage, arrow) {
		var bowSkill = ch.getSkill(getSkillVnum("Bow")) / 100.0;
		var ob = ch.offense / 3.0;
		var dex = ch.dexterity * 2.0;
		var str = ch.strength;
		var roll = random(-50, 50);
		var off = bowSkill * (ob + dex + str + roll + (shotStage) * 10);
		if (arrow == "a streamlined arrow")	
			off += 20;
		else if (arrow == "a decaying arrow")
			off -= 10;
		return off;
	}
	// Calculate the defensive roll for the arrow.
	function arrowDefenseRoll(vict, arrow) {
		var dodge = vict.dodge;
		var abs = vict.absorb * 1.5;
		var pb = vict.pb * 2 / 3;
		var def = Math.ceil(dodge, Math.ceil(abs, pb));
		if (vict.shieldBlock)
			def += 20;
		return def;
	}
	// Make a character stand up after being stunned by an arrow.
	function bashStand(vict) {
		vict.isBashed = false;
		vict.comm("stand");
	}
	
}