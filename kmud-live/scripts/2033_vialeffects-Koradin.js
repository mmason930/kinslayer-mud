var script2033 = function(self, actor, here, args, extra)
{
	//This script is attached to vials, and adds effects to the quaffer.
//Koradin - January 2010
var arg_array = getArgList(args);
var color = (actor.cyan(constants.CL_SPARSE) + actor.bold(constants.CL_SPARSE));
var color2 = (actor.red(constants.CL_SPARSE) + actor.bold(constants.CL_SPARSE));
var coloroff = actor.normal(constants.CL_SPARSE);
if (arg_array.length < 2)
{
	_noblock;
	return;
}
else
{
	for each(var word in self.namelist.split(" "))
	{
		if (arg_array[1] == word)
			var vial = 1;
	}
	if (!vial)
	{
		actor.send("What do you want to quaff?");
		_block;
		return;
	}
	_block;
	act("You tilt your head back and quaff " + self.name + ".", true, actor, null, null, constants.TO_CHAR);
	act("$n tilts $s head back and quaffs " + self.name + ".", true, actor, null, null, constants.TO_ROOM);	
	if (self.vnum < 2710) // a vial filled with a <describer> blue liquid, regenerates HPS
	{
		if (self.vnum == 2700)
			var mvMod = [.005];
		else if (self.vnum == 2701)
			var mvMod = [.01];
		else if (self.vnum == 2702)
			var mvMod = [.015, 0];
		else if (self.vnum == 2703)
			var mvMod = [.02, 0];
		else if (self.vnum == 2704)
			var mvMod = [.025, 1];
		else if (self.vnum == 2705)
			var mvMod = [.03, 1];
		else if (self.vnum == 2706)
			var mvMod = [.035, 2];
		else if (self.vnum == 2707)
			var mvMod = [.04, 2];
		else if (self.vnum == 2708)
			var mvMod = [.045, 3];
		else if (self.vnum == 2709)
			var mvMod = [.05, 3];
		if (getSval(actor,2033,"hps") > 0)
		{
			actor.send("Nothing seems to happen...");
			self.extract();
			return;
		}
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
			self.moveToRoom(getRoom(20899));
			wait 10;
			setSval(actor, 2033, "hps", 0);
		}
		else {
			actor.hps += burst;
			if (mvMod[1])
				actor.affect(constants.AFF_HEALING,mvMod[1],0,heals);
			wait 10;
			setSval(actor, 2033, "hps", 0);
		}
	}
	else if (self.vnum < 2720) // a bubbling vial, <describer> to the touch, regenerates MVS
	{
		if (self.vnum == 2710)
			var mvMod = [.01];
		else if (self.vnum == 2711)
			var mvMod = [.02];
		else if (self.vnum == 2712)
			var mvMod = [.03, 0];
		else if (self.vnum == 2713)
			var mvMod = [.04, 0];
		else if (self.vnum == 2714)
			var mvMod = [.05, 1];
		else if (self.vnum == 2715)
			var mvMod = [.06, 1];
		else if (self.vnum == 2716)
			var mvMod = [.07, 2];
		else if (self.vnum == 2717)
			var mvMod = [.08, 2];
		else if (self.vnum == 2718)
			var mvMod = [.10, 3];
		else if (self.vnum == 2719)
			var mvMod = [.12, 3];
		if (getSval(actor,2033,"mvs") > 0)
		{
			actor.send("Nothing seems to happen...");
			self.extract();
			return;
		}
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
			self.moveToRoom(getRoom(20899));
			wait 10;
			setSval(actor, 2033, "mvs", 0);
		}
		else {
			actor.mvs += burst;
			if (mvMod[1])
				actor.affect(constants.AFF_REGENERATION,mvMod[1],0,heals);
			wait 10;
			setSval(actor, 2033, "mvs", 0);
		}
	}
	else if (self.vnum < 2725) // a fizzy orange vial, forced sleep + full regen
	{
		if (self.vnum == 2720)
			var mvMod = .80;
		else if (self.vnum == 2721)
			var mvMod = .85;
		else if (self.vnum == 2722)
			var mvMod = .90;
		else if (self.vnum == 2723)
			var mvMod = .95;
		else if (self.vnum == 2724)
			var mvMod = 1;
		act(color + "The strong mixture makes you a bit groggy..." + coloroff, true, actor, null, null, constants.TO_CHAR);
		act("$n yawns and looks a bit more groggy...", true, actor, null, null, constants.TO_ROOM);	
		self.moveToRoom(getRoom(20800));
		waitpulse 5;
		act(color + "Exhaustion overcomes you, and you go to sleep." + coloroff, true, actor, null, null, constants.TO_CHAR);
		act("$n gives in to the exhaustion and goes to sleep.", true, actor, null, null, constants.TO_ROOM);
		actor.stopFighting();
		actor.position = constants.POS_SLEEPING;
		actor.attach(2034);
		setSval(actor,2034,"sleeping",time()+120);
		var wait_time = 720;
		while (wait_time > 0)
		{
			waitpulse 1;
			wait_time -= 1;
			if (!actor) {
				return;
			}
			else if (actor.position != constants.POS_SLEEPING)
			{
				actor.detach(2034);
				return;
			}
		}
		actor.detach(2034);
		if (actor.position == constants.POS_SLEEPING) {
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
	}
	else if (self.vnum < 2730)
	{
		if (self.vnum == 2725)
			var mvMod = 1;
		else if (self.vnum == 2726)
			var mvMod = 3;
		else if (self.vnum == 2727)
			var mvMod = 5;
		else if (self.vnum == 2728)
			var mvMod = 7;
		else if (self.vnum == 2729)
			var mvMod = 9;
		if (!actor.affectedBy(constants.AFF_SANCTUARY))
		{
			act(color + "A glow surrounds you, and you start to feel protected!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n starts glowing softly.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_SANCTUARY,mvMod,0,0);
		}
		else
			actor.send("Nothing seems to happen.");
	}
	else if (self.vnum < 2740)
	{
		//another vial
		return;
	}
	else if (self.vnum == 2790) // a red swirling vial, removes BLIND
	{
		if (actor.affectedBy(constants.AFF_BLIND))
		{
			act(color + "The world comes back into view as your eyes are cleansed of the blindness!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n looks around as if taking in the world for the first time.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_BLIND);
		}
		else
		{
			act(color2 + "A burning sensation fills your head, and the world fades from view." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n blinks, looking around and seeming not to focus on anything.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_BLIND,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2791) // a dark green vial, removes POISON
	{
		if (actor.affectedBy(constants.AFF_POISON))
		{
			act(color + "The cold sweat disappears, and you feel a lot better!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("The color returns to $n's face.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_POISON);
		}
		else
		{
			act(color2 + "The dark liquid turns your stomach, and you break into a cold sweat..." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("The blood drains from $n's face.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_POISON,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2792) // a light purple vial, removes INSANITY
	{
		if (actor.affectedBy(constants.AFF_INSANE))
		{
			act(color + "The voices in your head fade away as the liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n breathes a sigh of relief.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_INSANE);
		}
		else
		{
			act(color2 + "You begin to giggle for no reason at all." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n begins to giggle for no reason at all.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_INSANE,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2793) // a smooth brown vial, removes DECAY
	{
		if (actor.affectedBy(constants.AFF_DECAY))
		{
			act(color + "Your body stops falling apart, the stench fading away." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("The stench from $n's decaying body fades away.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_DECAY);
		}
		else
		{
			act(color2 + "A rancid stench fills your nostrils as your body begins to decay!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n gags at the smell of rotting flesh that suddenly fills the air.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_DECAY,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2794) // a sky blue vial, removes CHILL
	{
		if (actor.affectedBy(constants.AFF_CHILL))
		{
			act(color + "Your body warms as the liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n stops shivering.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_CHILL);
		}
		else
		{
			act(color2 + "You shiver as the cold liquid runs down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n starts shivering uncontrollably.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_CHILL,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2795) // a cream colored vial, removes DEAF
	{
		if (actor.affectedBy(constants.AFF_DEAF))
		{
			act(color + "The sounds of the world come rushing back to your ears." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n covers $s ears as if everything just got louder.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_DEAF);
		}
		else
		{
			act(color2 + "The sound of your ears popping is the last thing you hear!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n panics, covering $s ears with $s hands!", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_DEAF,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2796) // a deep amber vial, removes WEAKEN
	{
		if (actor.affectedBy(constants.AFF_WEAKEN))
		{
			act(color + "The atrophy in your muscles fades away..." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n rubs $s muscles, smiling.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_WEAKEN);
		}
		else
		{
			act(color2 + "Your muscles instantly atrophy, all strength fading away!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n gasps and struggles to hold $s body up.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_WEAKEN,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2797) // a fuzzy yellow vial, removes SILENCE
	{
		if (actor.affectedBy(constants.AFF_SILENCE))
		{
			act(color + "The syrup stings your vocal chords, which perk up nicely." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n's throat swells, then settles back down.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_SILENCE);
		}
		else
		{
			act(color2 + "The syrup chokes you, smothering your vocal chords!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n starts coughing, sputtering and working $s mouth.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_SILENCE,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2798) // a smoky charcoal vial, removes CRIPPLE
	{
		if (actor.affectedBy(constants.AFF_CRIPPLE))
		{
			act(color + "Strength returns to your limbs as the liquid rushes down your throat." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n stands tall, flexing $s muscles anew.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_CRIPPLE);
		}
		else
		{
			act(color2 + "Your muscles contort, and you nearly lose all control of them!" + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n suddenly hunches over, groaning in pain.", true, actor, null, null, constants.TO_ROOM);	
			actor.affect(constants.AFF_CRIPPLE,random(0,2),0,0);
		}
	}
	else if (self.vnum == 2799) // a sky blue vial, removes SHIELD
	{
		if (actor.affectedBy(constants.AFF_SHIELD))
		{
			act(color + "You breathe a sigh of relief as you once again sense the Source." + coloroff, true, actor, null, null, constants.TO_CHAR);
			act("$n breathes a sigh of relief.", true, actor, null, null, constants.TO_ROOM);	
			actor.unaffect(constants.AFF_SHIELD);
		}
		else
		{
			if (actor.class == constants.CLASS_CHANNELER || actor.class == constants.CLASS_DREADGUARD || actor.class == constants.CLASS_DREADLORD)
			{
				act(color2 + "A feeling of loss comes over you that you cannot explain." + coloroff, true, actor, null, null, constants.TO_CHAR);
				act("$n takes on a saddened and confused look.", true, actor, null, null, constants.TO_ROOM);	
				actor.affect(constants.AFF_SHIELD,random(0,2),0,0);
			}
			else
				actor.send("Nothing seems to happen.");
		}
	}
	self.extract();
}


};

