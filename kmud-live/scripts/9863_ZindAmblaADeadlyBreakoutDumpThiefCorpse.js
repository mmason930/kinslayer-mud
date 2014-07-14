var script9863 = function(self, actor, here, args, extra)
{
		var qName = "A Deadly Breakout";
	if ( actor.quest(qName) == 1 && actor.checkQuestItem("the corpse of an imprisoned thief", 1, qName) ) {
		_block;
		actor.send("You lean forward and release the corpse, letting it fall over the edge of the well into the darkness.");
		self.echoaround(actor,actor.name + " leans forward and releases a bloody corpse, letting it fall over the edge of the well into the darkness.");
		actor.remQuestItem("the corpse of an imprisoned thief",-1,qName);
		waitpulse 10;
		actor.updateJournalTask(qName,1);
		actor.qval(qName,2);
	}

};

