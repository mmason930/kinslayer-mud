var script20650 = function(self, actor, here, args, extra) {
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
	var shotStageTimers = [ 2.0, 4.0, 6.0, 8.0 ];
	var shotStageCharMsgs = [ "You nock an arrow and begin to study the area.",
							  "\nYou stare $N down, aiming carefully.",
							  "\nYou start to draw your arrow back further, hoping to knock $N off $S feet.",
							  "\nYou start shifting your aim to $N's weak point, hoping to hit for massive damage." ];
	var shotStageVictMsgs = [ "$n nocks an arrow and begins to study the area.",
							  "$n begins to stare at you intently, a look of concentration spreading across $s face!",
							  "$n starts to draw $s arrow further back, $s muscles rippling powerfully!",
							  "$n shifts $s aim slightly, studying your body carefully!" ];
	var shotStageRoomMsgs = [ "$n nocks an arrow and begins to study the area.",
							  "$n begins to stare intently, a look of concentration spreading across $s face.",
							  "$n starts to draw $s arrow further back, $s muscles rippling powerfully.",
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
	// Get target and target room
	if( argArray.length == 2 && ch.fighting ) {
		// Shooting in the same room, determine the strength
		if( argArray[1] == "light" ) {
			shotStage = 0;
			room = here;
		}
		else if( argArray[1] == "medium" ) {
			shotStage = 1;
			room = here;
		}
		else if( argArray[1] == "heavy" ) {
			shotStage = 2;
			room = here;
		}
		vict = ch.getCharRoomVis(ch.fighting);
		room = here;
	}
	else if( argArray.length == 3 ) {
		// Normal shot, determine the strength
		if( argArray[2] == "light" ) {
			shotStage = 0;
			room = here;
		}
		else if( argArray[2] == "medium" ) {
			shotStage = 1;
			room = here;
		}
		else if( argArray[2] == "heavy" ) {
			shotStage = 2;
			room = here;
		}
		else {
			// Not a normal shot, determine if shooting to another room
			dir = dirFromText(argArray[2]);
			room = here.direction(dir);
			if( !room ) {
				ch.send("You can't shoot " + argArray[2] + ".");
				return;
			}
			shotStage = 1;
		}
		vict = getCharInListVis(ch, argArray[1], room.people);
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
	var timer = shotStageTimers[shotStage];
	if( here != room )
		timer *= 2.0;
	ch.startTimer(timer);
	var success = runTimer(ch);
	if( success ) {
		if( vict.room == room ) {
			// Figure out if the shot lands.
			var off = arrowOffenseRoll(ch, shotStage);
			var def = arrowDefenseRoll(vict);
			if( off > def ) {
				// Arrow hits, calculate damage and messages
				dmg = calculateDamage( weapon, vict.absorb, shotStage );
				damMsg = damageMessage(dmg);
				dirMsg = directionMessage( revDir(dir) );
				// Send messages
				act(ch.green(constants.CL_SPARSE) + "Your arrow hits $N with" + damMsg + ch.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_CHAR);
				if( dir )
					act(vict.red(constants.CL_SPARSE) + "An arrow fired" + dirMsg + " hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
				else
					act(vict.red(constants.CL_SPARSE) + "An arrow fired by $n hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
				act("An arrow fired by $N " + dirMsg + " hits $n" + damMsg, true, vict, weapon, ch, constants.TO_NOTVICT);
				// Apply damage and effects
				vict.damage(dmg, ch);
				var p = ch.faceoff(vict);
				if( !ch.fighting && p ) {
					ch.stopFighting();
					ch.setFighting(p);
				}
			}
			else {
				// Arrow misses
				act("You dodge to the side, letting an arrow fly past.", true, vict, weapon, ch, constants.TO_CHAR);
				act("$n suddenly dodges to the side, letting an arrow fly past.", true, vict, weapon, ch, constants.TO_ROOM);
				return;
			}
		}
		else {
			// Victim moved, can't finish the shot.
			ch.send("Shoot who?");
		}
	}
	// Run a timer multiple times, powering up the shot each time a timer completes.
	/*while( shotStage < shotStageTimers.length ) {
		// The timer and messages for each shotStage are set in arrays that are initialized at the top of the script.
		var timer = shotStageTimers[shotStage];
		if( here != room ) {
			timer *= 2.0;
		}
		act( shotStageCharMsgs[shotStage], true, ch, weapon, vict, constants.TO_CHAR );
		if( shotStage < 1 )
			act( shotStageRoomMsgs[shotStage], true, ch, weapon, vict, constants.TO_NOTVICT );
		if( here == room ) {
			act( shotStageVictMsgs[shotStage], true, ch, weapon, vict, constants.TO_VICT );
		}
		// Attach a script that will delay the user's next command, to prevent awkward interaction with the UI.
		ch.attach(20814);
		// Start the timer.
		ch.startTimer(timer);
		var success = runTimer(ch);
		if( success && !ch.isBashed ) {
			if( vict.room == room ) {
				// Victim hasn't moved, increase the shot power.
				shotStage++;
				// Prevent permastun.
				if( vict.isBashed && canBeStunned ) {
					act("$N is already stunned, you won't be able to knock $M down again.", true, ch, weapon, vict, constants.TO_CHAR );
					canBeStunned = false;
				}			
			}
			else {
				// Victim moved, stop aiming.
				act( "Your victim seems to have moved.", true, ch, weapon, vict, constants.TO_CHAR );
				act( "$n lowers $p, $s target no longer in $s sights.", true, ch, weapon, vict, constants.TO_ROOM );
				waitpulse 1;
				ch.detach(20814);
				return;
			}		
		}
		else {
			if( ch.isBashed ) {
				ch.detach(20814);
				return;
			}
			// Timer cancelled, time to shoot.
			break;
		}
	}
	// Remove the command delaying script. The waitpulse is required for it to actually do anything.
	waitpulse 1;
	ch.detach(20814);
	// Prevent people from instantly shooting arrows.
	if( shotStage < 1 ) {
		act( "$n lowers $p.", true, ch, weapon, vict, constants.TO_ROOM );
		return;
	}
	// Still can't shoot through doors.
	if( room.doorIsClosed(dir) || room.doorIsLocked(dir) ) {
		actor.send("You see the " + room.doorName(dir) + " is now closed, so you lower the bow.");
		return;
	}
	// Shoot the arrow!
	act( "You take aim and fire your arrow into the air!", true, ch, weapon, vict, constants.TO_CHAR );
	act( "\n$n takes aim and fires an arrow from $p!", true, ch, weapon, vict, constants.TO_NOTVICT );
	if( here == room ) {
		act( "\n$n takes aim and fires an arrow at you from $p!", true, ch, weapon, vict, constants.TO_VICT );
	}
	wait 4;
	// Victim moved, the arrow fails.
	if( vict.room != room ) {
		room.echo("An arrow hits the ground harmlessly.");
		return;
	}
	var off = arrowOffenseRoll(ch, shotStage);
	var def = arrowDefenseRoll(vict);
	if( off > def ) {
		// Arrow hits, calculate damage and messages
		dmg = calculateDamage( weapon, vict.absorb, shotStage );
		damMsg = damageMessage(dmg);
		dirMsg = directionMessage( revDir(dir) );
		// Send messages
		act(ch.green(constants.CL_SPARSE) + "Your arrow hits $N" + damMsg + ch.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_CHAR);
		if( dir )
			act(vict.red(constants.CL_SPARSE) + "An arrow" + dirMsg + " hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
		else
			act(vict.red(constants.CL_SPARSE) + "An arrow hits you" + damMsg + vict.normal(constants.CL_SPARSE), true, ch, weapon, vict, constants.TO_VICT);
		act("An arrow" + dirMsg + " hits $n" + damMsg, true, vict, weapon, ch, constants.TO_NOTVICT);
		// Apply damage and effects
		vict.damage(dmg, ch);
		var p = ch.faceoff(vict);
		if( !ch.fighting && p ) {
			ch.stopFighting();
			ch.setFighting(p);
		}
		if( shotStage > 2 && !vict.isBashed && canBeStunned && vict.isValid ) {
			var stunSave = Math.ceil( vict.dodge / 3.0, vict.absorb );
			var stunRoll = random(1, 100);
			if( stunRoll < stunSave && vict.dodge / 3.0 > vict.absorb ) {
				// Dodgers can avoid the stun, but the saving roll is less than an abser's since they have a better chance of avoiding the initial arrow.
				act("$N tumbles from the impact, but manages to land on $S feet.", true, ch, weapon, vict, constants.TO_CHAR);
				act("The impact from the arrow knocks you backwards, but you manage to roll back to your feet.", true, ch, weapon, vict, constants.TO_VICT);
			}
			else if( stunRoll < stunSave ) {
				// It's harder to stun absers with arrows.
				act("$N flinches slightly, but manages to stay upright.", true, ch, weapon, vict, constants.TO_CHAR);
				act("You flinch as the arrow hits you, but you avoid getting knocked over.", true, ch, weapon, vict, constants.TO_VICT);
			}
			else {
				// Bash your victim for a round.
				act("$N is knocked off $S feet and sent sprawling!", true, ch, weapon, vict, constants.TO_CHAR);
				act("The arrow knocks you off your feet, sending you sprawling!", true, ch, weapon, vict, constants.TO_VICT);
				act("$n is knocked off $S feet and sent sprawling!", true, vict, weapon, ch, constants.TO_NOTVICT);
				vict.isBashed = true;
				vict.lag(30);
				setTimeout(30, bashStand, vict);
				vict.pos = constants.POS_SITTING;
			}
		}
	}
	else {
		// Arrow misses
		act("You dodge to the side, letting an arrow fly past.", true, vict, weapon, ch, constants.TO_CHAR);
		act("$n suddenly dodges to the side, letting an arrow fly past.", true, vict, weapon, ch, constants.TO_ROOM);
		return;
	}*/
	/**** Functions used during shooting ****/
	// Calculate shot damage. For now it's going to be based on weapon damage, weapon weight and how charged the shot is.
	function calculateDamage(weapon, abs, stage) {
		var hitdam = random(weapon.value(constants.VALUE_WEAPON_DAM_LOW), weapon.value(constants.VALUE_WEAPON_DAM_HIGH)) + weapon.weight;
		if( stage > 1 ) {
			hitdam += weapon.weight;
		}
		if( stage > 3 ) {
			hitdam *= 1.75;
		}
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
	function arrowOffenseRoll(ch, shotStage) {
		var bowSkill = ch.getSkill(getSkillVnum("Bow")) / 100.0;
		var ob = ch.offense / 3.0;
		var dex = ch.dexterity * 2.0;
		var str = ch.strength;
		var roll = random(-70, 30);
		return bowSkill * (ob + dex + str + roll + (shotStage + 2) * 10);
	}
	// Calculate the defensive roll for the arrow.
	function arrowDefenseRoll(vict) {
		var dodge = vict.dodge;
		var abs = vict.abs * 1.5;
		var pb = vict.pb * 2 / 3;
		
		return Math.ceil(dodge, Math.ceil(abs, pb));
	}
	// Make a character stand up after being stunned by an arrow.
	function bashStand(vict) {
		vict.isBashed = false;
		vict.comm("stand");
	}
}