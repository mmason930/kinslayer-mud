var script20802 = function(self, actor, here, args, extra) {
	//For use with bows.  This script is for the "shoot" command to be used while unengaged.
	//Koradin - Converted from DG in December 2009
	if (actor.position != constants.POS_STANDING)
	{
		if (actor.position == constants.POS_FIGHTING)
			actor.send("You're already fighting!");
		else
			actor.send("You're not in the right position for that!");
		_block;
		return;
	}
	//THE FOLLOWING (UP THROUGH LINE 40) CHECKS TO MAKE SURE THEY'RE WIELDING THE BOW INSTEAD OF
	//CARRYING OR WEARING IT ON THEIR BACK.
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
	//Temporary measure to make up for removal of Multishot. This will be done more elegantly at some point.
	hitdam *= 1.5;
	getCharCols(actor,constants.CL_OFF);
	var arg_array = getArgList(args);
	var bow_skill = actor.getSkill(getSkillVnum("Bow"));
	var archery_practice = random(bow_skill - 25,bow_skill + 1);
	if (archery_practice < 0)
	{
		archery_practice = 0;
		var missprac = 1;
	}
	else if (archery_practice < 15)
	{
		archery_practice = 0;
		var missprac = 2;
	}
	else if (archery_practice < 40)
		var hitprac = "the fourth ring.";
	else if (archery_practice < 65)
		var hitprac = "the third ring.";
	else if (archery_practice < 80)
		var hitprac = "the second ring.";
	else if (archery_practice < 90)
		var hitprac = "the first ring.";
	else if (archery_practice < 96)
		var hitprac = "the outer bullseye!";
	else if (archery_practice < 101)
		var hitprac = "the inner bullseye!";
	else if (archery_practice > 100)
	{
		archery_practice = 100;
		var hitprac = "the inner bullseye!";
	}
	var bow_skill_for_dodge = (bow_skill - 50);
	var crit_skill = (actor.getSkill(getSkillVnum("Critical Hit")) / 4);
	//var multi_skill = (actor.getSkill(getSkillVnum("Multishot")) / 5);
	var room = actor.room;
	var arrow_effects = [];
	var dmg_message = [];
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
				hitdam *= 1.1;
				bow_skill_for_dodge += 25;
				crit_skill += 10;
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
	//THE FOLLOWING SETS A FUNCTION TO CALL FOR WHEN THE ARROW IS FINISHED FLYING.
	function arrowFinish()
	{
		var bow_roll = random(1,105);
		var dodge_roll = random(1,100);
		if (!target)
		{
			room.echo(capFirstLetter(arrow) + enter_message + " hits the ground harmlessly.");
			return;
		}
		else if (target.room != room)
		{
			room.echo(capFirstLetter(arrow) + enter_message + " hits the ground harmlessly.");
			return;
		}
		if (bow_skill < bow_roll)
		{
			var mistaken_target = getRandomPersonInRoom(room);
			if (mistaken_target == actor || mistaken_target == target || !actor.canSee(mistaken_target))
			{
				act(capFirstLetter(arrow) + enter_message + " narrowly misses you!", true, target, null, actor, constants.TO_CHAR);
				act(capFirstLetter(arrow) + enter_message + " narrowly misses $n!", true, target, null, actor, constants.TO_ROOM);
				return;
			}
			if (mistaken_target.shieldBlock && random(1,120) < mistaken.target.eq(constants.WEAR_SHIELD).parry)
			{
				act(capFirstLetter(arrow) + " meant for $N flies past and bounces off your raised shield!", true, mistaken_target, null, target, constants.TO_CHAR);
				act(capFirstLetter(arrow) + " meant for $N flies past and bounces off of $n's raised shield!", true, mistaken_target, null, target, constants.TO_NOTVICT);
				act(capFirstLetter(arrow) + " meant for you flies past and bounces off of $n's raised shield!", true, mistaken_target, null, target, constants.TO_VICT);
				return;
			}
			var mistaken_target_abs = (mistaken_target.absorb + (mistaken_target.absorb / 3));
			var dmg = Math.ceil((hitdam * (100 - mistaken_target_abs)) / 100);
			if (crit_skill > random(1,100))
			{
				dmg *= 2;
				//mistaken_target.comm("scream");
				actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
				if (actor.questTaskProgress('It Pays To Be Critical')[1].quotient < 1)
					actor.updateJournalTask('It Pays To Be Critical',0);
			}
			for(i = 0; i < arrow_effects.length; i++)
			{
				target = mistaken_target;
				arrow_effects[i]();
			}
			damageMessage(dmg);
			act(mistaken_target.red(constants.CL_SPARSE) + capFirstLetter(arrow) + " meant for $N flies past and hits you instead!" + mistaken_target.normal(constants.CL_SPARSE), true, mistaken_target, null, target, constants.TO_CHAR);
			act(capFirstLetter(arrow) + " meant for $N flies past and hits $n instead!", true, mistaken_target, null, target, constants.TO_NOTVICT);
			act(capFirstLetter(arrow) + " meant for you flies past and hits $n instead!", true, mistaken_target, null, target, constants.TO_VICT);
			mistaken_target.damage(dmg, actor);
			dmg_message = [];
			return;
		}
		else
		{
			var target_abs = (target.absorb + (target.absorb / 3));
			var dmg = Math.ceil((hitdam * (100 - target_abs)) / 100);
			var target_dodge = (target.dodge - random((bow_skill_for_dodge - 10), (bow_skill_for_dodge + 10)));
			if (target.shieldBlock)
			{
				if (random(1,120) < target.eq(constants.WEAR_SHIELD).parry)
				{
					target.send(capFirstLetter(arrow) + enter_message + " glances off your raised shield, ricocheting to the ground harmlessly.");
					act(capFirstLetter(arrow) + enter_message + " glances off of $n's raised shield, ricocheting to the ground.", true, target, null, null, constants.TO_ROOM);
					return;
				}
				else
				{
					if (crit_skill > random(1,100))
					{
						dmg *= 2;
						//target.comm("scream");
						actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
						if (actor.questTaskProgress('It Pays To Be Critical')[1].quotient < 1)
							actor.updateJournalTask('It Pays To Be Critical',0);
					}
					for(i = 0; i < arrow_effects.length; i++)
						arrow_effects[i]();
					damageMessage(dmg);
					target.send(target.red(constants.CL_SPARSE) + "Your raised shield fails to block " + arrow + flight_message + ", and it hits you" + dmg_message + target.normal(constants.CL_SPARSE));
					if (actor.room == target.room)
						act(actor.green(constants.CL_SPARSE) + "$n's shield fails to block " + arrow + flight_message + ", which hits $m" + dmg_message + actor.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_VICT);
					act("$n's shield fails to block " + arrow + flight_message + ", which hits $m" + dmg_message, true, target, null, actor, constants.TO_NOTVICT);				
					target.damage(dmg, actor);
					dmg_message = [];
					return;
				}
			}
			if (target_dodge > dodge_roll && random(1,200) != 1)
			{
				act(dodge_message, true, target, null, actor, constants.TO_CHAR);
				act("$n suddenly dodges to the side, letting " + arrow + " fly past.", true, target, null, null, constants.TO_ROOM);
				return;
			}
			if (crit_skill > random(1,100))
			{
				dmg *= 2;
				//target.comm("scream");
				actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
				if (actor.questTaskProgress('It Pays To Be Critical')[1].quotient < 1)
					actor.updateJournalTask('It Pays To Be Critical',0);
			}
			for(i = 0; i < arrow_effects.length; i++)
				arrow_effects[i]();
			damageMessage(dmg);
			act(target.red(constants.CL_SPARSE) + capFirstLetter(arrow) + enter_message + " hits you" + dmg_message + target.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_CHAR);
			if (actor.room == target.room)
				act(actor.green(constants.CL_SPARSE) + capFirstLetter(arrow) + enter_message + " hits $n" + dmg_message + actor.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_VICT);
			act(capFirstLetter(arrow) + enter_message + " hits $n" + dmg_message, true, target, null, actor, constants.TO_NOTVICT);		
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
	if (bow_skill < 1)
	{
		actor.send("You have no idea how to use " + self.name + ".");
		// if (arrContains(actor.getIncompleteQuests(),actor.getQuestByName('Archery Practice')))
			// actor.questSend("Head back to the ranger trainer and type 'practice bow' to start practicing 'Bow'.");
		return;
	}
	if (arg_array.length < 2 || arg_array.length > 3)
	{
		actor.send("Shoot who what where?");
		return;
	}
	if (room.isFlagged(constants.ROOM_PEACEFUL))
	{
		actor.send("This room just has such a peaceful, easy feeling...");
		return;
	}
	//THE FOLLOWING CONTAINS SHOOTING A SAME-ROOM TARGET. FOR EXAMPLE: 'SHOOT GALNOR'
	else if (arg_array.length == 2)
	{
		var target = getCharInListVis(actor, arg_array[1], room.people);
		if (!target)
		{
			var target = getObjInListVis(actor, arg_array[1], room.items);
			if (target)
			{
				if (target.vnum != 13387)
				{
					actor.send("You can't shoot " + target.name + "...");
					return;
				}
				var shoot_timer = ((111 - bow_skill) / 10);
				act("You square up and steady your bow, facing the target...", true, actor, null, actor, constants.TO_CHAR);
				act("$n squares up and steadies $s bow, facing " + target.name + "...", true, actor, null, actor, constants.TO_ROOM);
				actor.start_timer(shoot_timer);
				var success = run_timer(actor);
				if (!success)
					return;
				if (!target)
				{
					actor.send("It doesn't seem to be here anymore...");
					return;
				}
				if (target.room != room)
				{
					actor.send("It doesn't seem to be here anymore...");
					return;
				}
				if (actor.fighting)
				{
					actor.send("You're already fighting!");
					return;
				}
				act("\nYou fire an arrow at " + target.name + "!", true, actor, null, actor, constants.TO_CHAR);
				act("\n$n fires an arrow at " + target.name + "!", true, actor, null, actor, constants.TO_ROOM);
				wait 2;
				room.echo("An arrow flies through the air...");
				wait 2;
				if (!missprac)
				{
					act("Your arrow makes a loud THWACK as it hits "+hitprac, true, actor, null, actor, constants.TO_CHAR);
					actor.send("You scored "+archery_practice+"!");
					if (actor.questTaskProgress('Archery Practice')[1].quotient < 1)
						actor.updateJournalTask('Archery Practice',0);
					act("$n's arrow makes a loud THWACK as it hits "+hitprac, true, actor, null, actor, constants.TO_ROOM);
					act("$n scores "+archery_practice+"!", true, actor, null, actor, constants.TO_ROOM);
					return;
				}
				else if (missprac == 1)
				{
					act("Your arrow misses the target completely!", true, actor, null, actor, constants.TO_CHAR);
					actor.send("You scored "+red+"0"+nrm+"!");
					act("$n's arrow misses the target completely!", true, actor, null, actor, constants.TO_ROOM);
					act("$n scores "+red+"0"+nrm+"!", true, actor, null, actor, constants.TO_ROOM);
					return;
				}
				else if (missprac == 2)
				{
					act("Your arrow hits the wooden supports of the target, out of the scoring zone.", true, actor, null, actor, constants.TO_CHAR);
					actor.send("You scored "+red+"0"+nrm+"!");
					act("$n's arrow hits the wooden supports of the target, out of the scoring zone.", true, actor, null, actor, constants.TO_ROOM);
					act("$n scores "+red+"0"+nrm+"!", true, actor, null, actor, constants.TO_ROOM);
					return;
				}
			}
			else
			{
				actor.send(capFirstLetter(arg_array[1]) + " doesn't seem to be here.");
				return;
			}
		}
		if (target == actor)
		{
			actor.send("Why would you shoot yourself?!");
			return;
		}
		var cur_hp = actor.hps;
		var shoot_timer = ((111 - bow_skill) / 10);
		actor.startTimer(shoot_timer);
		var success = runTimer(actor);
		if (!success)
			return;
		if (actor.hps < cur_hp)
		{
			actor.send("You can't maintain your concentration after taking a hit!");
			return;
		}
		if (!target)
		{
			actor.send("They aren't here anymore.");
			return;
		}
		if (target.room != room)
		{
			actor.send("They aren't here anymore.");
			return;
		}
		// if (actor.fighting)
		// {
			// actor.send("You're already fighting!");
			// return;
		// }
		var dodge_message = "You swiftly dodge " + arrow + " with your name on it.";
		var enter_message = "";
		var flight_message = "";
		/*if (multi_skill > random(1,100))
		{
			if (quiver)
			{
				if (quiver.getPval("arrows") > 0)
				{
					if (quiver.getPval("arrows") < 3)
					{
						arrow = "an arrow";
						arrows = "arrows";
						arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
						arrows_fly_message = ("The " + arrows + " fly through the air...");
					}
					else
						quiver.setPval("arrows", (quiver.getPval("arrows") - 3), true);
				}
				dodge_message = "You swiftly dodge " + arrow + " with your name on it.";
			}
			actor.send("\nYou nock three " + arrows + ", take aim and fire them into the air in rapid succession!");
			if (actor.questTaskProgress('The More The Merrier')[1].quotient < 1)
					actor.updateJournalTask('The More The Merrier',0);
			act("\n$n takes aim and fires three " + arrows + " from " + self.name + " in rapid succession!", true, actor, null, target, constants.TO_ROOM);
			wait 2;
			//room.echo(arrows_fly_message + "...")
			wait 2;
			arrowFinish();
			waitpulse 5;
			arrowFinish();
			waitpulse 5;
			arrowFinish();
		}*/
		//else
		//{
			if (quiver)
			{
				if (quiver.getPval("arrows") > 0)
					quiver.setPval("arrows", (quiver.getPval("arrows") - 1), true);
			}
			actor.send("\nYou nock " + arrow + ", take aim and fire it into the air!");
			act("\n$n takes aim and fires " + arrow + " from " + self.name + "!", true, actor, null, target, constants.TO_ROOM);
			wait 2;
			//room.echo(arrow_fly_message + "...")
			wait 2;
			arrowFinish();
		//}
	}
	//THE FOLLOWING CONTAINTS SHOOTING A CROSS-ROOM TARGET. FOR EXAMPLE: 'SHOOT GALNOR EAST'
	else if (arg_array.length == 3)
	{
		for (i = 0; i < 6; i++)
		{
			if (strn_cmp(arg_array[2], dirToText(i), 1))
				var dir = i;
		}
		if (!dir && dir != 0)
		{
			actor.send("The direction '" + arg_array[2] + "' does not exist.");
			actor.send(actor.cyan(constants.CL_SPARSE) + "Directions include NORTH, SOUTH, EAST, WEST, UP, and DOWN." + actor.normal(constants.CL_SPARSE));
			return;
		}
		if (!room.direction(dir) || room.doorHidden(dir) > actor.getSkill(getSkillVnum("Search")))
		{
			actor.send("There doesn't seem to be a '" + dirToText(dir) + "' exit from here.");
			return;
		}
		else if (room.doorIsClosed(dir))
		{
			actor.send("But you can't see through the " + room.doorName(dir) + "!");
			return;
		}
		var rev_dir = revDir(dir);
		var rev_dir_text = dirToText(rev_dir);
		var dir_room = room.direction(dir);
		var dir_text = dirToText(dir);
		if (rev_dir_text == "up")
			var where = "above";
		else if (rev_dir_text == "down")
			var where = "below";
		else
			var where = "the " + rev_dir_text;
		var target = getCharInListVis(actor, arg_array[1], dir_room.people);
		if (!target)
		{
			actor.send("Sorry, '" + arg_array[1] + "' doesn't seem to be " + dir_text + " of here.");
			return;
		}
		if (target.mobFlagged(constants.MOB_SMOB))
		{
			actor.send(target.name + " would probably come over here and tear you to pieces if you did that!");
			return;
		}
		else if (target.leader)
		{
			if (target.leader.mobFlagged(constants.MOB_SMOB))
			{
				actor.send(target.leader.name + " would probably come over here and tear you to pieces if you did that!");
				return;
			}
		}
		act("You raise " + self.name + " and nock " + arrow + ", studying the area " + dir_text + ".", true, actor, null, null, constants.TO_CHAR);
		act("$n raises " + self.name + " and nocks " + arrow + ", studying the area " + dir_text + ".", true, actor, null, null, constants.TO_ROOM);
		var shoot_timer = ((107 - bow_skill) / 3);
		if (shoot_timer > 10)
			shoot_timer = 10;
		actor.startTimer(shoot_timer);
		var success = runTimer(actor);
		if (!success)
			return;
		if (!target)
		{
			actor.send("You notice they aren't there anymore, so you lower the bow.");
			return;
		}
		if (target.room != dir_room)
		{
			actor.send("You notice they aren't there anymore, so you lower the bow.");
			return;
		}
		if (room.doorIsClosed(dir) || room.doorIsLocked(dir))
		{
			actor.send("You see the " + room.doorName(dir) + " is now closed, so you lower the bow.");
			return;
		}
		if (quiver)
		{
			if (quiver.getPval("arrows") > 0)
				quiver.setPval("arrows", (quiver.getPval("arrows") - 1), true);
		}
		actor.send("\nYou nock " + arrow + ", take aim and fire it " + dir_text + "!");
		act("\n$n takes aim and fires " + arrow + " from " + self.name + "!", true, actor, null, target, constants.TO_ROOM);
		wait 2;
		if (room.doorIsClosed(dir) || room.doorIsLocked(dir))
		{
			room.echo(capFirstLetter(arrow) + " hits the " + room.doorName(dir) + " with a loud THWACK as it swings back and forth.");
			return;
		}
		//else
			//room.echo(arrow_fly_message + ", quickly disappearing " + dir_text + ".")
		wait 2;
		room = dir_room;
		var dodge_message = arrow + " flies in from " + where + " with your name on it, but you dodge it expertly.";
		var enter_message = " flies in from " + where + " and";
		var flight_message = " as it flies in from " + where;
		arrowFinish();
	}
}