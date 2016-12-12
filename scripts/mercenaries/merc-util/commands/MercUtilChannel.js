var MercUtil = (function(MercUtil) {

	MercUtil.performChannel = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_CHANNELER && myMercObj.className != constants.CLASS_DREADGUARD) {

			target.say("Are you crazy?! I can't channel!");
			return;
		}

		if(!arg2) {
			target.say("What do you want me to weave?");
			return;
		}

		var arg2 = vArgs[1];
		var heal, dmg, msgToActor, msgToRoom, length, cost, victim, combat;
		health = 0;
		dmg = 0;
		cost = 0;
		length = 0;
		msgToActor = "";
		msgToRoom = "";
		victim = "undefined";
		combat = false;
		/** Non-Combat Weaves **/
		switch(arg2) {
			case "healing":
				/*Healing*/
				if(myMercObj.skillsList[myMercObj.skill] != "Healing") {
					target.say("I don't have that talent.");
					return;
				}
				cost = Math.floor(0.85*myMercObj.maxSpell);
				msgToActor = capFirstLetter(target.name) + " makes a small gesture towards you and you feel better.";
				msgToRoom = capFirstLetter(target.name) + " makes a small gesture towards " + actor.name + " and " + actor.heShe() + " looks better.";
				length = 10;
				victim = actor;
				health = 2*myMercObj.wis + myMercObj.intel;
				combat = false;
				break;
		/** Combat Weaves **/
			case "air":
				/*Air Scythe*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(15,19);
				length = 1.5;
				msgToActor = "You suddenly feel intense pain as deep cuts appear on your body.";
				msgToRoom = victim.name + "'s flesh is sliced apart!";
				combat = true;
				cost = 40;
				break;
			case "fireball":
				/*Fireball*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(40,60);
				length = 4;
				msgToActor = "A large fireball blooms into existance and envelops you!";
				msgToRoom = "A large fireball blooms into existance and envelops " + victim.name + ".";
				combat = true;
				cost = 80;
				break;
			case "ice":
				/*Ice Shards*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(26,36);
				length = 3.25;
				msgToActor = "Razor sharp ice shards seem to form from thin air before impaling you.";
				msgToRoom = victim.name + " is struck by razor sharp ice shards!";
				combat = true;
				cost = 50;
				break;
			case "impact":
				/*Impact*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(27,39);
				length = 3.25;
				msgToActor = "A huge boulder rises from the ground and then crashes into you, causing a lot of pain.";
				msgToRoom = "A boulder floats up into the air and then races towards " + victim.name + ", crashing right into " + him_her(victim.sex) + ".";
				combat = true;
				cost = 65;
				break;
			case "create":
				/*Create Fire*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(15,25);
				length = 1.5;
				msgToActor = "There is sudden burst of light as sparks ignite your clothing.";
				msgToRoom = victim.name + " howls in pain as flames suddenly appear on " + him_her(victim.sex) + ".";
				combat = true;
				cost = 40;
				break;
			case "frostbite":
				/*Frostbite*/
				if(target.position == constants.POS_FIGHTING) {
					victim = target.fighting;
				}
				else {
					victim = "undefined";
				}
				dmg = random(14,22);
				length = 1.5;
				msgToActor = "Your limbs start to feel numb as frost accumulates over them.";
				msgToRoom = "Frost begins to grow on " + victim.name + ".";
				combat = true;
				cost = 35;
				break;
		}
		if(msgToActor == "") {
			target.say("What do you want me to weave?");
			return;
		}
		if(target.position != constants.POS_STANDING && combat == false) {
			target.say("I can't form the weaves in this position.");
			return;
		}
		if(target.position == constants.POS_FIGHTING && combat == false) {
			target.say("I can't form those weaves while fighting!");
			return;
		}
		if(actor.position == constants.POS_FIGHTING && combat == false) {
			target.say("I can't get the weaves right with you fighting!");
			return;
		}
		/** Casting **/
		if(!target.isBashed) {
			if(myMercObj.sps < cost) {
				target.say("I don't have the energy to do that.");
				return;
			}
			if(victim == "undefined" || !victim || victim == null) {
				target.say("Who am I doing this to?");
				return;
			}
			/* Echo Start of Weaving */
			here.echo(capFirstLetter(target.name) + " begins to form a weave.");
			target.startTimer(length);
			var success = runTimer(target);
			if(success && victim.room == target.room) {
				if(target.affectedBy(constants.AFF_SHIELD)){
					here.echo(capFirstLetter(target.name) + " looks suprised as " + target.heShe() + " fails to reach the source.");
					return;
				}
				if(combat) {
					if(victim.class == constants.CLASS_FADE || victim.dodge > 100) {
						if(random(0,5) == 2) {
							victim.send("You avoid " + target.name + "'s attempts at channeling!");
							here.echoaround(victim,capFirstLetter(victim.name) + " avoid's " + target.name + "'s attempt at channeling!");
							return;
						}
					}
				}
				else {
					if(target.position != constants.POS_STANDING) {
						target.say("I can't form the weaves in this position.");
						return;
					}
					if(target.position == constants.POS_FIGHTING) {
						target.say("I can't form those weaves while fighting!");
						return;
					}
					if(actor.position == constants.POS_FIGHTING) {
						target.say("I can't get the weaves right with you fighting!");
						return;
					}
				}
				/* Send Messages */
				victim.send(msgToActor);
				var list = [target,victim];
				echoaroundmany(list,msgToRoom);
				/* Do Damage */
				if(dmg) {
					dmg += Math.floor(myMercObj.skillBonus/2);
				}
				victim.damage( dmg, target);
				/* Do Healing */
				if(health) {
					health += 3*myMercObj.skillBonus;
				}
				victim.heal(health);
				/* Drain SPS */
				cost -=  myMercObj.skillBonus;
				target.sps -= cost;
				if(actor.level > 99) {
					here.echo("Cost: " + cost);
					here.echo("Dmg: " + Math.floor(myMercObj.skillBonus/2 + dmg) + ", Healing: " + health);
				}
			}
		}
	};

	return MercUtil;

})(MercUtil || {});