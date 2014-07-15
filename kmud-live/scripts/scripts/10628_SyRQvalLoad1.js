var script10628 = function(self, actor, here, args, extra) {
	var qName = 'Search and Rescue';
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
	     here.loadMob(10602);
	}
}