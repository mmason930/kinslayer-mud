var script13248 = function(self, actor, here, args, extra) {
	//Greet script for Quest Masters
	//Koradin - January 2010
	if (actor.vnum != -1 || actor.quest('QUEST_GREETINGS') == 1)
		return;
	waitpulse 1;
	if ( actor.room != self.room ) {
		return;
	}
	var cyan = actor.cyan(constants.CL_OFF);
	var nrm = actor.normal(constants.CL_OFF);
	var bld = actor.bold(constants.CL_OFF);
	var yellow = actor.yellow(constants.CL_OFF);
	var quest_qvals = [];
	var unstarted = 0;
	var unfinished = 0;
	var complete = 0;
	for (var _autoKey in actor.getAvailableQuests(self.vnum)) {
		var quest = actor.getAvailableQuests(self.vnum)[_autoKey];
		var push = false;
		if ( quest.priorQuests.length > 0 ) {
			var i = 0;
			for (var _autoKey in quest.priorQuests ) {
				var name = quest.priorQuests [_autoKey];
				if ( actor.quest(name) == -1 ) {
					++i;
				}
			}
			if ( i == quest.priorQuests.length ) {
				push = true;
			}
		}
		else {
			push = true;
		}
		if ( push == true ) {
			quest_qvals.push(actor.quest(quest.questName));
		}
	}
	for (var i = 0; i < quest_qvals.length; i++)
	{
		if (quest_qvals[i] == -1)
			complete += 1;
		else if (quest_qvals[i] == 0)
			unstarted += 1;
		else if (quest_qvals[i] > 0)
			unfinished += 1;
	}
	if (complete == quest_qvals.length)
		return;
	else if (unfinished > 0)
	{
		if (unfinished == 1)
			unfinished = "1 quest";
		else if (unfinished > 1)
			unfinished = (unfinished + " quests");
		actor.send(yellow + bld + capFirstLetter(self.name) + " is waiting for you to complete " + unfinished + "." + nrm);
	}
	if (unstarted > 0)
	{
		if (unstarted == 1)
			unstarted = "1 new quest";
		else if (unstarted > 1)
			unstarted = (unstarted + " new quests");
		actor.send(yellow + bld + capFirstLetter(self.name) + " has " + unstarted + " available for you!" + nrm);
	}
	actor.send(" ");
	actor.send("Type '" + cyan + "quest" + nrm + "' at the quest master for more information.");
	actor.send(" ");
}