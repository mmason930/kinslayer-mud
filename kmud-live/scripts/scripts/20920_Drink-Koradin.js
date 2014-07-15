var script20920 = function(self, actor, here, args, extra) {
	//Drinking and sipping to check for vial effects
	//Pouring flasks and skins out, filling them from fountains
	//Koradin - May 2011
	var vArgs = getArgList(args);
	if (!vArgs[1]) {
		for each (var item in actor.room.items) {
			if (item.type == constants.ITEM_FOUNTAIN) {
				var container = item;
				break;
			}
		}
	}
	else if (vArgs[2]) {
		if (!str_cmp(vArgs[0],"pour") && !str_cmp(vArgs[2],"out")) {
			var container = getObjInListVis(actor,vArgs[1],actor.inventory);
		}
		else if (strn_cmp(vArgs[0],"fill",2)) {
			var flask = getObjInListVis(actor,vArgs[1],actor.inventory);
			var container = getObjInListVis(actor,vArgs[2],actor.room.items);
			if (!flask) {
				_noblock;
				return;
			}
			if (flask.type != constants.ITEM_DRINKCON && flask.type != constants.ITEM_FOUNTAIN) {
				_noblock;
				return;
			}
		}
		else {
			_noblock;
			return;
		}
	}
	else {
		var container = getObjInListVis(actor,vArgs[1],actor.inventory);
			if (!container)
				var container = getObjInListVis(actor,vArgs[1],actor.room.items);
	}
	if (!container) {
		_noblock;
		return;
	}
	if (container.type != constants.ITEM_DRINKCON && container.type != constants.ITEM_FOUNTAIN) {
		_noblock;
		return;
	}
	else if (container.type == constants.ITEM_DRINKCON) {
		var curDrinks = constants.VALUE_LIQ_CONTAINER_DRINKS;
		var disTime = 21600; //flasks, skins, etc. 
	}
	else if (container.type == constants.ITEM_FOUNTAIN) {
		var curDrinks = constants.VALUE_FOUNTAIN_DRINKS;
		var disTime = 4320; //fountains, wells, streams, etc 
	}
	var affected = container.getPval("vialEffect");
	if (!affected) {
		_noblock;
		return;
	}
	affected = affected.split("~");
	var vialVnum = parseInt(affected[0]);
	var numEffect = parseInt(affected[1]);
	var timeAffected = parseInt(affected[2]);
	var curTime = time();
	if (numEffect < 1 || curTime - timeAffected > disTime || container.value(curDrinks) == 0 || !str_cmp(vArgs[0],"pour")) {
		_noblock;
		container.deletePval("vialEffect",true);
		return;
	}
	if (strn_cmp(vArgs[0],"fill",2) && flask.value(constants.VALUE_LIQ_CONTAINER_DRINKS) < flask.value(constants.VALUE_LIQ_CONTAINER_MAX_DRINKS)) {
		_noblock;
		flask.setPval("vialEffect",vialVnum+"~"+numEffect+"~"+curTime,true);
		numEffect -= 1;
		container.deletePval("vialEffect",true);
		container.setPval("vialEffect,",vialVnum+"~"+1+"~"+timeAffected,true);
		return;
	}
	var color = (actor.cyan(constants.CL_SPARSE) + actor.bold(constants.CL_SPARSE));
	var color2 = (actor.red(constants.CL_SPARSE) + actor.bold(constants.CL_SPARSE));
	var coloroff = actor.normal(constants.CL_SPARSE);
		if (vialVnum < 2710) // a vial filled with a <describer> blue liquid, regenerates HPS
		{
			if (vialVnum == 2700)
				var mvMod = [.005];
			else if (vialVnum == 2701)
				var mvMod = [.01];
			else if (vialVnum == 2702)
				var mvMod = [.015, 0];
			else if (vialVnum == 2703)
				var mvMod = [.02, 0];
			else if (vialVnum == 2704)
				var mvMod = [.025, 1];
			else if (vialVnum == 2705)
				var mvMod = [.03, 1];
			else if (vialVnum == 2706)
				var mvMod = [.035, 2];
			else if (vialVnum == 2707)
				var mvMod = [.04, 2];
			else if (vialVnum == 2708)
				var mvMod = [.045, 3];
			else if (vialVnum == 2709)
				var mvMod = [.05, 3];
			act(color + "You feel the healing liquid start to go to work inside you." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n takes on an added look of vigor.", true, actor, null, null, constants.TO_ROOM);	
			var hp_limit = (actor.maxHit * 0.6);
			var heals = Math.floor(actor.maxHit * mvMod[0]);
			var burst = heals * 2;
			setSval(actor, 2033, "hps", 1);
			if (actor.hps > hp_limit || actor.affectedBy(constants.AFF_HEALING))
			{
				actor.send("You don't feel the effect as greatly as you should...");
				if ((actor.hps+burst) > actor.maxHit)
					actor.hps = actor.maxHit;
				else
					actor.hps += burst;
			}
			else {
				actor.hps += burst;
				if (mvMod[1])
					actor.affect(constants.AFF_HEALING,mvMod[1],0,heals);
			}
		}
		else if (vialVnum < 2720) // a bubbling vial, <describer> to the touch, regenerates MVS
		{
			if (vialVnum == 2710)
				var mvMod = [.01];
			else if (vialVnum == 2711)
				var mvMod = [.02];
			else if (vialVnum == 2712)
				var mvMod = [.03, 0];
			else if (vialVnum == 2713)
				var mvMod = [.04, 0];
			else if (vialVnum == 2714)
				var mvMod = [.05, 1];
			else if (vialVnum == 2715)
				var mvMod = [.06, 1];
			else if (vialVnum == 2716)
				var mvMod = [.07, 2];
			else if (vialVnum == 2717)
				var mvMod = [.08, 2];
			else if (vialVnum == 2718)
				var mvMod = [.10, 3];
			else if (vialVnum == 2719)
				var mvMod = [.12, 3];
			act(color + "The liquid runs down your throat, instantly refreshing you!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n perks up a bit as $e feels more refreshed.", true, actor, null, null, constants.TO_ROOM);	
			var mv_limit = (actor.maxMove * 0.8);
			var heals = Math.floor(actor.maxMove * mvMod[0]);
			var burst = heals * 2;
			if (actor.mvs > mv_limit || actor.affectedBy(constants.AFF_REGENERATION))
			{
				actor.send("You don't feel the effect as greatly as you should...");
				if ((actor.mvs+burst) > actor.maxMove)
					actor.mvs = actor.maxMove;
				else
					actor.mvs += burst;
			}
			else {
				actor.mvs += burst;
				if (mvMod[1])
					actor.affect(constants.AFF_REGENERATION,mvMod[1],0,heals);
			}
		}
		else if (vialVnum < 2725) // a fizzy orange vial, forced sleep + full regen
		{
			if (vialVnum == 2720)
				var mvMod = .80;
			else if (vialVnum == 2721)
				var mvMod = .85;
			else if (vialVnum == 2722)
				var mvMod = .90;
			else if (vialVnum == 2723)
				var mvMod = .95;
			else if (vialVnum == 2724)
				var mvMod = 1;
			act(color + "Exhaustion overcomes you, and you go to sleep." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n collapses into a deep sleep.", true, actor, null, null, constants.TO_ROOM);
			actor.position = constants.POS_SLEEPING;
			actor.attach(2034);
			var wait_time = 720;
			while (wait_time > 0)
			{
				waitpulse 1;
				wait_time -= 1;
				if (!actor) {
					numEffect -= 1;
					container.setPval("vialEffect",vialVnum+"~"+numEffect+"~"+timeAffected,true);
					return;
				}
				else if (actor.position != constants.POS_SLEEPING)
				{
					actor.detach(2034);
					numEffect -= 1;
					container.setPval("vialEffect",vialVnum+"~"+numEffect+"~"+timeAffected,true);
					return;
				}
			}
			actor.detach(2034);
			actor.position = constants.POS_SITTING;
			act(color + "You awaken suddenly, feeling more refreshed than you have in a long time!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n awakens and sits up quickly, feeling refreshed.", true, actor, null, null, constants.TO_ROOM);	
			var hitHeal = Math.floor(actor.maxHit * mvMod);
			var mvHeal = Math.floor(actor.maxMove * mvMod);
			if (actor.hps < hitHeal)
				actor.hps = hitHeal;
			if (actor.mvs < mvHeal)
				actor.mvs = mvHeal;
		}
		else if (vialVnum < 2730)
		{
			if (vialVnum == 2725)
				var mvMod = 1;
			else if (vialVnum == 2726)
				var mvMod = 3;
			else if (vialVnum == 2727)
				var mvMod = 5;
			else if (vialVnum == 2728)
				var mvMod = 7;
			else if (vialVnum == 2729)
				var mvMod = 9;
			if (!actor.affectedBy(constants.AFF_SANCTUARY))
			{
				act(color + "A glow surrounds you, and you start to feel protected!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n starts glowing softly.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_SANCTUARY,mvMod,0,0);
			}
		}
		else if (vialVnum < 2740)
		{
			//another vial
			return;
		}
		else if (vialVnum == 2790) // a red swirling vial, removes BLIND
		{
			// if (actor.affectedBy(constants.AFF_BLIND))
			// {
				// act(color + "The world comes back into view as your eyes are cleansed of the blindness!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n looks around as if taking in the world for the first time.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_BLIND);
			// }
			// else
			// {
				act(color2 + "A burning sensation fills your head, and the world fades from view." + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n blinks, looking around and seeming not to focus on anything.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_BLIND,random(1,3),0,0);
			// }
		}
		else if (vialVnum == 2791) // a dark green vial, removes POISON
		{
			// if (actor.affectedBy(constants.AFF_POISON))
			// {
				// act(color + "The cold sweat disappears, and you feel a lot better!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("The color returns to $n's face.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_POISON);
			// }
			// else
			// {
				act(color2 + "The dark liquid turns your stomach, and you break into a cold sweat..." + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("The blood drains from $n's face.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_POISON,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2792) // a light purple vial, removes INSANITY
		{
			// if (actor.affectedBy(constants.AFF_INSANE))
			// {
				// act(color + "The voices in your head fade away as the liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n breathes a sigh of relief.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_INSANE);
			// }
			// else
			// {
				act(color2 + "You begin to giggle for no reason at all." + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n begins to giggle for no reason at all.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_INSANE,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2793) // a smooth brown vial, removes DECAY
		{
			// if (actor.affectedBy(constants.AFF_DECAY))
			// {
				// act(color + "Your body stops falling apart, the stench fading away." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("The stench from $n's decaying body fades away.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_DECAY);
			// }
			// else
			// {
				act(color2 + "A rancid stench fills your nostrils as your body begins to decay!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n gags at the smell of rotting flesh that suddenly fills the air.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_DECAY,random(0,3),0,0);
			// }
		}
		else if (vialVnum == 2794) // a sky blue vial, removes CHILL
		{
			// if (actor.affectedBy(constants.AFF_CHILL))
			// {
				// act(color + "Your body warms as the liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n stops shivering.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_CHILL);
			// }
			// else
			// {
				act(color2 + "You shiver as the cold liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n starts shivering uncontrollably.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_CHILL,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2795) // a cream colored vial, removes DEAF
		{
			// if (actor.affectedBy(constants.AFF_DEAF))
			// {
				// act(color + "The sounds of the world come rushing back to your ears." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n covers $s ears as if everything just got louder.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_DEAF);
			// }
			// else
			// {
				act(color2 + "The sound of your ears popping is the last thing you hear!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n panics, covering $s ears with $s hands!", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_DEAF,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2796) // a deep amber vial, removes WEAKEN
		{
			// if (actor.affectedBy(constants.AFF_WEAKEN))
			// {
				// act(color + "The atrophy in your muscles fades away..." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n rubs $s muscles, smiling.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_WEAKEN);
			// }
			// else
			// {
				act(color2 + "Your muscles instantly atrophy, all strength fading away!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n gasps and struggles to hold $s body up.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_WEAKEN,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2797) // a fuzzy yellow vial, removes SILENCE
		{
			// if (actor.affectedBy(constants.AFF_SILENCE))
			// {
				// act(color + "The syrup stings your vocal chords, which perk up nicely." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n's throat swells, then settles back down.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_SILENCE);
			// }
			// else
			// {
				act(color2 + "The syrup chokes you, smothering your vocal chords!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n starts coughing, sputtering and working $s mouth.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_SILENCE,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2798) // a smoky charcoal vial, removes CRIPPLE
		{
			// if (actor.affectedBy(constants.AFF_CRIPPLE))
			// {
				// act(color + "Strength returns to your limbs as the liquid rushes down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n stands tall, flexing $s muscles anew.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_CRIPPLE);
			// }
			// else
			// {
				act(color2 + "Your muscles contort, and you nearly lose all control of them!" + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n suddenly hunches over, groaning in pain.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_CRIPPLE,random(5,8),0,0);
			// }
		}
		else if (vialVnum == 2799) // a sky blue vial, removes SHIELD
		{
			// if (actor.affectedBy(constants.AFF_SHIELD))
			// {
				// act(color + "You breathe a sigh of relief as you once again sense the Source." + coloroff, true, actor, null, null, constants.TO_CHAR);
				// act("$n breathes a sigh of relief.", true, actor, null, null, constants.TO_ROOM);	
				// actor.unaffect(constants.AFF_SHIELD);
			// }
			// else
			// {
				if (actor.class == constants.CLASS_CHANNELER || actor.class == constants.CLASS_DREADGUARD || actor.class == constants.CLASS_DREADLORD)
				{
					act(color2 + "A feeling of loss comes over you that you cannot explain." + coloroff, true, actor, null, null, constants.TO_CHAR);
					act("$n takes on a saddened and confused look.", true, actor, null, null, constants.TO_ROOM);	
					actor.affect(constants.AFF_SHIELD,random(1,3),0,0);
				}
			// }
		}
	numEffect -= 1;
	container.setPval("vialEffect",vialVnum+"~"+numEffect+"~"+timeAffected,true);
}