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
	var unstarted = 0;
	var unfinished = 0;
	var complete = 0;
	var quests = actor.getAvailableQuests(self.vnum);
	for (var i = 0; i < quests.length; i++)
	{
		var quest = quests[i];
		if (quest.hasCompleted(actor))
			complete += 1;
		else if (!quest.hasBegun(actor))
			unstarted += 1;
		else
			unfinished += 1;
	}
	if (complete == quests.length)
		return;
	else if (unfinished > 0)
	{
		var str = unfinished + " quest";
		if (unfinished > 1)
			str += "s";
		actor.send(yellow + bld + capFirstLetter(self.name) + " is waiting for you to complete " + str + "." + nrm);
	}
	if (unstarted > 0)
	{
		var str = unstarted + " new quest";
		if (unstarted > 1)
			str += "s";
		actor.send(yellow + bld + capFirstLetter(self.name) + " has " + str + " available for you!" + nrm);
	}
	actor.send(" ");
	actor.send("Type '" + cyan + "quest" + nrm + "' at the quest master for more information.");
	actor.send(" ");
}