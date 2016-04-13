var script13247 = function(self, actor, here, args, extra) {
	//Receive script for Quest Masters, makes them give the item back.
	//Koradin - January 2010
	waitpulse 1;
	self.comm("give all " + actor.name)
	if (getSval(actor, 13247, "gave too many items") > 0)
	{
		_block;
		return;
	}
	actor.send("If you are attempting to complete a quest, use the command '" + actor.cyan(constants.CL_OFF) + "quest complete <QuestNumber>" + actor.normal(constants.CL_OFF) + "'");
	setSval(actor, 13247, "gave too many items", 1)
	waitpulse 1;
	setSval(actor, 13247, "gave too many items", 0)
}