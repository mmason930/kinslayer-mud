/**
 * Displays a list of Quests to the actor
 */
function questList(self, actor, quests) {
	//List all the available quests.
	getCharCols(actor, constants.CL_OFF);
	actor.send(red + " Available Quests" + nrm);
	actor.send(bld + strPadding("", "-", 19, "right") + nrm);
	for (var i = 0; i < quests.length; ++i) {
		var qName = quests[i].name;
		var sFinal = cyn + (i + 1) + nrm + ") " + grn + quests[i].name;
		sFinal = strPadding(sFinal, " ", 40, "right");
		actor.send(sFinal + nrm + "  (" + quest.statusString(actor) + ")");
	}
}
