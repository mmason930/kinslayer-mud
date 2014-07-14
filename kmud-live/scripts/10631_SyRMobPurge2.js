var script10631 = function(self, actor, here, args, extra)
{
	var qName = 'Search and Rescue';
if ( actor.questInProgress(qName) ) {
self.moveToRoom(getRoom(501));
self.extract();
}

};

