var script4523 = function(self, actor, here, args, extra) {
	//Alder
	//Where Ends Meet: Load Nella Make Her Talk
	if ( actor.quest("NELLA_DEAD") != 1 ) {
		var qName = "Where Ends Meet";
		if (Quest.getByName(qName).tasks[0].isInProgress(actor)) {
			self.loadMob(9870);
			actor.qval(qName,2);
			var nella = self.people[0];
			waitpulse 7;
			nella.comm("sa I'm glad you could make it. I hope you didn't tell Zind you were coming here.");
			waitpulse 14;
			nella.comm("sa He has become a bloody thorn in the sides of those I work for, and as such his death is quite a high priority.");
			waitpulse 21;
			nella.comm("sa By working for me and my employers you make the decision to leave Zind behind.");
			waitpulse 5;
			getCharCols(actor,constants.CL_OFF);
			actor.send(bld+cyn+"You now have to choose to pursue quests from Nella or Zind."+nrm);
			actor.updateJournalTask(qName,0);
			return;
		}
		else if ( Quest.getByName(qName).hasCompleted(actor) && !Quest.getByName("Branching Out").hasBegun(actor) ) {
			self.loadMob(9870);
		}
	}
}