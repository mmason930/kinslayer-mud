var script10608 = function(self, actor, here, args, extra)
{
	if ( actor.quest('Search and Rescue') > 0 && !actor.questTaskProgress('Search and Rescue')[4].comp ) {
     wait 2
     getCharCols(actor);
     actor.send(bld+cyn+"You hear echoes of a dark ritual within - you have discovered the lair!"+nrm);
     wait 2
     actor.updateJournalTask('Search and Rescue', 3);
}

};

