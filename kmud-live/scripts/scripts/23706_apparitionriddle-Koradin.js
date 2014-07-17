var script23706 = function(self, actor, here, args, extra) {
	//Reacts to "open the door" speech. If conditions are met, will start the riddle to enter Maerwynn's smob.
	//Koradin - December 2009
	if (getSval(actor, 23705, "maerdoor_sval") == 0)
	{
	_block;
	return;
	}
	else
	{
		if (getSval(self, 23706, "riddle_sval") == 2)
		{
		waitpulse 2;
		self.say("Not before you answer my riddle!");
		return;
		}
		else if (getSval(self, 23706, "riddle_sval") == 1)
		return;
	var arg_array = getArgList(args);
		if (arg_array.length < 3)
		return;
		for(var i = 0; i < arg_array.length; i++)
		{
			if (arg_array[i] == "please")
			var nicely = 1
		}
		if (nicely != 1)
		{
		waitpulse 2;
		self.say("Ask nicely!");
		waitpulse 2;
		self.comm("cack");
		return;
		}
		if (getSval(self, 23706, "riddle_sval") == 1)
		return;
		else
		{
		setSval(self, 23706, "riddle_sval", 1);
		waitpulse 1;
		self.comm("emote arches a ghostly eyebrow.");
		wait 2;
		self.say("I don't take orders from fleshlings!");
		wait 2;
		self.comm("stroke");
		self.say("However...");
		wait 4;
		self.say("If you can prove your worth I may let you pass!");
		wait 2;
		self.say("Solve my riddle and I'll let you through.");
		wait 2;
		self.say("But pay attention! I'll only say this once!");
		wait 3;
		self.comm("emote conjures up a ghostly scroll and begins to recite:");
		wait 2;
			if (random(1,2) == 1)
			{
			self.say("Beautiful beyond compare,");
			wait 1;
			self.say("Abilities thought very rare,");
			wait 1;
			self.say("Thinks first, acts after,");
			wait 1;
			self.say("Ends life, kills laughter,");
			wait 1;
			self.say("Who might this woman be,");
			wait 1;
			self.say("That my mistress will not see?");
			}
			else
			{
			self.say("Beauty, power, battles of Ages,");
			wait 1;
			self.say("Greed, immortal, friend of mages,");
			wait 1;
			self.say("Not merely friend, but enemy,");
			wait 1;
			self.say("Tricks to last infinity,");
			wait 1;
			self.say("Nubile and young, old as time,");
			wait 1;
			self.say("Name her now to solve this rhyme!");
			}
		setSval(self, 23706, "riddle_sval", 2);
		}
	}
}