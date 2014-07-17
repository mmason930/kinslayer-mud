var script1050 = function(self, actor, here, args, extra) {
	markEmoticonQuestMobAsKilled(self, actor);
	emoticonQuestCheckIfLastMember(self);
	emoticonQuestReAssessLeadership(self);
	
	if(self.mount)
	{
		act("$n explodes into tiny fragments!", false, self.mount, null, null, constants.TO_ROOM);
		act("You explode into tiny fragments!", false, self.mount, null, null, constants.TO_CHAR);
		self.mount.extract();
	}
}