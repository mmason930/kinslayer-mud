var script1054 = function(self, actor, here, args, extra) {
	if(getHour() == 10 && getHour() - emoticonQuestHourOfLastTeleportion > 1)
		emoticonQuestRandomTeleportation(self);
	
}