var script20820 = function(self, actor, here, args, extra) {
	//Start global quest
	//Koradin - April 2010
	var a = getArgList(args);
	var asked = false;
	var dontask = false;
	var join = false;
	var quest_complete = false;
	if (getSval(self,20820,"talking") == 3)
		return;
	wait 1;
	if (actor.name != "Koradin")
	{
		self.say("Sorry, I'm just testing things with Koradin at the moment.");
		return;
	}
	if (actor.name == "Koradin" && !str_cmp(a[0],"reset"))
	{
		self.say("Resetting your sval, Koradin.");
		setSval(self,20820,actor.id,0);
		return;
	}
	if (!str_cmp(a[0],"hi") || !str_cmp(a[0],"hello") || !str_cmp(a[0],"hi!") || !str_cmp(a[0],"hello!") || !str_cmp(a[0],"hi.") || !str_cmp(a[0],"hello."))
	{	
		if (actor.quest("artifact_global") == 1)
		{
			self.say("Hello to you, " + actor.name + "! Do you bring what I seek?");
			return;
		}
		if (getSval(self,20820,actor.id) == 6)
		{
			self.comm("say I still wouldn't trust you with a mission like this, " + actor.name + "!");
			self.comm("say Come back another day and I might have cooled off.");
			self.comm("mutter " + actor.name);
			return;
		}
		self.say("Hello " + actor.name + ". Have you come to hear my tale?");
		setSval(self,20820,actor.id,1);
		return;
	}
	for (i = 0; i < a.length; i++)
	{
		if (!str_cmp(a[i],"tale") || !str_cmp(a[i],"story") || !str_cmp(a[i],"tale?") || !str_cmp(a[i],"story?") )
		{
			if (getSval(self,20820,actor.id) == 6)
			{
				self.comm("say I still wouldn't trust you with a mission like this, " + actor.name + "!");
				self.comm("say Come back another day and I might have cooled off.");
				self.comm("mutter " + actor.name);
				return;
			}
			setSval(self,20820,actor.id,1);
			asked = true;
		}
		if (!str_cmp(a[i],"join?") || !str_cmp(a[i],"help?") || !str_cmp(a[i],"join") || !str_cmp(a[i],"help") || !str_cmp(a[i],"will"))
			join = true;
		if (!str_cmp(a[i],"yep") || !str_cmp(a[i],"yes") || !str_cmp(a[i],"yeah") || !str_cmp(a[i],"sure"))
		{
			join = true;
			if (actor.quest("artifact_global") == 1)
				quest_complete = true;
		}
		if (!str_cmp(a[i],"no") || !str_cmp(a[i],"don't") || !str_cmp(a[i],"dont") || !str_cmp(a[i],"not") || !str_cmp(a[i],"can't") || !str_cmp(a[i],"cant") || !str_cmp(a[i],"cannot") || !str_cmp(a[i],"won't") || !str_cmp(a[i],"wont"))
			dontask = true;
	}
	if (dontask == true)
	{
		asked = false;
		join = false;
		quest_complete = false;
	}
	if (actor.quest("artifact_global") == 1)
	{
		if (quest_complete == true)
		{
			self.say("Fantastic, " + actor.name + "! Hand it to me now, and you'll have your reward.");
			return;
		}
		else
		{
			self.say("Well, keep searching! I'm confident they're out there somewhere!");
			return;
		}
	}
	if (getSval(self,20820,actor.id) == 1)
	{
		if (!str_cmp(a[0],"yes") || !str_cmp(a[0],"sure") || !str_cmp(a[0],"yeah") || !str_cmp(a[0],"yep") || asked == true)
		{
			setSval(self,20820,"talking",3);
			self.comm("emote sighs and scratches his leg with the tip of his sword.");
			wait 1;	
			self.comm("sheath sword");
			wait 2;
			self.say("Ah yes, " + actor.name + ", I'll tell you.");
			wait 2;
			self.say("But be warned, it's not a fairytale.");
			wait 2;
			self.comm("emote clears his throat and starts his story...");
			var string = [];
			string.push("say The armies swept over the hills.");
			string.push("say All birds in the sky disappeared.");
			string.push("say No one was coming to save us.");
			string.push("say There were only six of us left.");
			string.push("emote sighs again and wipes a ghostly tear, then continues.");
			string.push("say We had completed our mission.");
			string.push("say In the right hands, it would have made a difference.");
			string.push("emote glances off into the distance.");
			string.push("say We separated it amongst us and split up.");
			string.push("say It was all we could do to preserve the power.");
			string.push("say Now, ages later, I am the last of them.");
			string.push("emote takes on a determined look.");
			string.push("say My mission is simple.");
			string.push("say I must find what has been lost,");
			string.push("say And take what has been broken and repair it.");
			string.push("say Only then will my soul be at peace.");
			string.push("draw sword");
			string.push("say If you want to help me in my mission, just say so.");
			for (i = 0; i < 20; i++)
			{
				if (string[i])
				{	
					wait 5;
					if (actor.room != self.room)
					{
						self.say("Yes, everyone gets bored of hearing my stories...");
						self.comm("emote sighs and stops talking.");
						self.comm("draw sword");
						setSval(self,20820,"talking",0);
						setSval(self,20820,actor.id,0);
						return;
					}	
					self.comm(string[i]);
				}
			}
			setSval(self,20820,"talking",0);
			setSval(self,20820,actor.id,2);
			return;
		}
		else if (!str_cmp(a[0],"no") || !str_cmp(a[0],"nope") || !str_cmp(a[0],"nah") || dontask == true)
		{
			wait 1;
			self.comm("say Then be gone with you! I am looking for a more worthy traveler anyway.");
			self.comm("mutter " + actor.name);
			setSval(self,20820,actor.id,6);
			return;
		}
	}
	else if (getSval(self,20820,actor.id) == 2)
	{
		if (join == true)
		{
			self.comm("say Welcome to you, young " +actor.name+"! I thank you for helping me with this quest.");
			actor.qval("artifact_global",1);
			wait 1;
			self.say("I can tell you the areas in which I last saw the remaining pieces of the artifact I seek, but I'm afraid it's been ages and they may have been moved!");
			wait 1;
			self.say("Just let me know if you need help getting started.");
			self.comm("clap " +actor.name);
			return;
		}
		else
		{
			wait 1;
			self.comm("say Then be gone with you! I am looking for a more worthy traveler anyway.");
			self.comm("mutter " + actor.name);
			setSval(self,20820,actor.id,6);
			return;
		}
	}
}