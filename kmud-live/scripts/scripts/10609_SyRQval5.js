var script10609 = function(self, actor, here, args, extra) {
	var qName = "Search and Rescue";
	if ( actor.quest(qName) > 0 && self.room.vnum == 20862 ) {
	        getCharCols(actor);
	        actor.send(bld+cyn+"As the leader falls, you notice a key ring on his belt. That must contain the key to open the door out."+nrm);
	        actor.updateJournalTask(qName,4);
	}
}