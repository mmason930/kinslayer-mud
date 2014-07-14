var script10629 = function(self, actor, here, args, extra)
{
	var qName = 'Search and Rescue';
var west_room = here.direction(constants.WEST);
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
   west_room.loadMob(10602);
}

};

