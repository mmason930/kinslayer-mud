var script10602 = function(self, actor, here, args, extra) {
	_block;
	if ( actor.quest('Search and Rescue') > 0 && !actor.questTaskProgress('Search and Rescue')[3].comp ) {
		wait 2;
		actor.send(self.name + " glances around quickly before motioning you forward.");
		wait 4;
		self.comm("whisper " + actor.name + " Times are dark here, very dark indeed...");
		self.comm("whisper " + actor.name + " Can I trust you?");
		wait 5;
		self.loadObj(10609);
		self.loadObj(10610);
		self.comm("smile " + actor.name);
		self.comm("give rations " + actor.name);
		self.comm("give lantern " + actor.name);
		wait 2;
		self.comm("whisper " + actor.name + " I cannot fathom how you will survive, but I wish you a safe journey nonetheless. Take these to help you.");
		wait 2;
		self.comm("whisper " + actor.name + " Dark strangers set upon Terrion's daughter, bound her up and dragged her off in a sack.");
		wait 2;
		self.comm("whisper " + actor.name + " Faster than the cock crows, they were gone. I heard one of them muttering about where they");
		wait 2;
		self.comm("whisper " + actor.name + " was headed...seek out the burned and blackened grasslands northeast of Terrion's domain.");
		wait 2;
		self.comm("whisper " + actor.name + " Look for a hollow stump there and you shall find the entrance to the thieves' den. Curse them!");
	
		actor.updateJournalTask('Search and Rescue', 2)
	}
}