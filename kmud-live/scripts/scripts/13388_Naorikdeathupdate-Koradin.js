var script13388 = function(self, actor, here, args, extra) {
	if (actor.questTaskProgress('Test of the Bowman')[1].quotient < 1)
		actor.updateJournalTask('Test of the Bowman',0);
}