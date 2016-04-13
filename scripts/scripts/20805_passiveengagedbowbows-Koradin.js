var script20805 = function(self, actor, here, args, extra) {
	//PASSIVE SHOOTING SCRIPT FOR BOW USERS WHILE ENGAGED.
	//KORADIN - DECEMBER 2009
	wait random(1,5);
	_block;
	actor = self.wornBy;
	if (!actor)
		return;
	if (getSval(actor, 20807, "bash stack") == 1)
		return;
	if (actor.isBashed)
		return;
	if (actor.position != constants.POS_FIGHTING)
		return;
	if (actor.eq(constants.WEAR_BACK) == self)
		return;
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
	//Temporary measure to make up for removal of Multishot. This will be done more elegantly at some point.
	hitdam *= 1.5;
	var crit_skill = (actor.getSkill(getSkillVnum("Critical Hit")) / 4);
	var on_back = false;
	var bow_skill = actor.getSkill(getSkillVnum("Bow"));
	var bow_skill_for_dodge = (bow_skill - 50);
	var dodge_roll = 0;
	if (actor.eq(constants.WEAR_BACK) == self)
	{
		hitdam /= 7;
		crit_skill /= 2;
		bow_skill_for_dodge -= 15;
	}
	var target = actor.fighting;
	//var multi_skill = (actor.getSkill(getSkillVnum("Multishot")) / 4);
	var room = actor.room;
	var arrow_effects = [];
	var dmg_message = [];
	if (bow_skill < random(1,100))
		return;
	//THE FOLLOWING CHECKS TO SEE IF THE ACTOR IS WEARING A QUIVER, AND IF SO, MODIFIES THE ARROWS MESSAGES, AND EFFECTS.
	function getQuiver( actor )
	{
		var back_item = actor.eq(constants.WEAR_BACK);
		if (back_item)
		{
			for (var _autoKey in back_item.namelist.split(" ")) {
				var word = back_item.namelist.split(" ")[_autoKey];
				if (word == "quiverforbows")
					return back_item;
			}
		}
		return null;
	}
	var quiver = getQuiver( actor );
	if (!quiver)
	{
				var arrow = "an arrow";
				var arrows = "arrows";
				var arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
				var arrows_fly_message = ("The " + arrows + " fly through the air...");
	}
	else
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
	//THE FOLLOWING SETS A FUNCTION TO CALL FOR WHEN THE ARROW IS FINISHED FLYING.
	function arrowFinish()
	{
		var bow_roll = random(1,105);
		dodge_roll += random(1,100);
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
			if (mistaken_target.isBashed)
			{
				dmg *= 2;
				dmg /= 3;
			}
			if (crit_skill > random(1,100))
			{
				dmg *= 2;
				mistaken_target.comm("scream");
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
			if (target.isBashed)
			{
				dmg *= 2;
				dmg /= 3;
			}
			var target_dodge = (target.dodge - random((bow_skill_for_dodge - 10), (bow_skill_for_dodge + 10)));
			if (target.vnum != -1)
				target_dodge -= 20;
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
						target.comm("scream");
						actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
						if (actor.questTaskProgress('It Pays To Be Critical')[1].quotient < 1)
							actor.updateJournalTask('It Pays To Be Critical',0);
					}
					for(i = 0; i < arrow_effects.length; i++)
						arrow_effects[i]();
					damageMessage(dmg);
					target.send(target.red(constants.CL_SPARSE) + "Your raised shield fails to block " + arrow + flight_message + ", and it hits you" + dmg_message + target.normal(constants.CL_SPARSE));
					if (actor.room == room)
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
				target.comm("scream");
				actor.send(actor.green(constants.CL_SPARSE) + "You scored a critical hit!" + actor.normal(constants.CL_SPARSE));
				if (actor.questTaskProgress('It Pays To Be Critical')[1].quotient < 1)
					actor.updateJournalTask('It Pays To Be Critical',0);
			}
			for(i = 0; i < arrow_effects.length; i++)
				arrow_effects[i]();
			damageMessage(dmg);
			act(target.red(constants.CL_SPARSE) + capFirstLetter(arrow) + enter_message + " hits you" + dmg_message + target.normal(constants.CL_SPARSE), true, target, null, actor, constants.TO_CHAR);
			if (actor.room == room)
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
		actor.send("You step back from the fight long enough to fire a volley of " + arrows + " into the air!");
		if (actor.questTaskProgress('The More The Merrier')[1].quotient < 1)
			actor.updateJournalTask('The More The Merrier',0);
		act("$n steps back from the fight long enough to fire a volley of " + arrows + " into the air!", true, actor, null, target, constants.TO_ROOM);
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
		actor.send("You step back from the fight long enough to fire " + arrow + " into the air!");
		act("$n steps back from the fight long enough to fire " + arrow + " into the air!", true, actor, null, target, constants.TO_ROOM);
		wait 2;
		//room.echo(arrow_fly_message + "...")
		wait 2;
		arrowFinish();
	//}
	var random_second_shot = random(1,2);
	if (random_second_shot == 1)
	{
		wait random(1,5);
		var arrow_effects = [];
		var quiver = getQuiver( actor );
	if (!quiver)
	{
				var arrow = "an arrow";
				var arrows = "arrows";
				var arrow_fly_message = (capFirstLetter(arrow) + " flies through the air");
				var arrows_fly_message = ("The " + arrows + " fly through the air...");
	}
	else
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
	}
		if (!actor)
			return;
		if (getSval(actor, 20807, "bash stack") == 1)
			return;
		if (actor.isBashed)
			return;
		if (actor.position != constants.POS_FIGHTING)
			return;
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
			actor.send("You step back from the fight long enough to fire a volley of " + arrows + " into the air!");
			act("$n steps back from the fight long enough to fire a volley of " + arrows + " into the air!", true, actor, null, target, constants.TO_ROOM);
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
			actor.send("You step back from the fight long enough to fire " + arrow + " into the air!");
			act("$n steps back from the fight long enough to fire " + arrow + " into the air!", true, actor, null, target, constants.TO_ROOM);
			wait 2;
			//room.echo(arrow_fly_message + "...")
			wait 2;
			arrowFinish();
		//}
	}
	
}