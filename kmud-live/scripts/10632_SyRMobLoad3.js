var script10632 = function(self, actor, here, args, extra)
{
	var qName = 'Search and Rescue';
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp ) {
here.loadMob(10603);
}

};

