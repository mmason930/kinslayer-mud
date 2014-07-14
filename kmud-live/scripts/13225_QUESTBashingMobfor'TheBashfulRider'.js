var script13225 = function(self, actor, here, args, extra)
{
	if ( actor.quest('The Bashful Rider') < 1 ) {
	return;
}
if ( self.isBashed == true ) {
	waitpulse 1;
	actor.updateJournalTask('The Bashful Rider',0);
}

};

