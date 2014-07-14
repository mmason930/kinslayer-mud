var script10626 = function(self, actor, here, args, extra)
{
	var qName = 'Forging Help'
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp && self.vnum == 4598 ) {
     getCharCols(actor);
     actor.send(bld+cyn+"You gather a piece of iron ore as instructed."+nrm);
     waitpulse 5
     actor.updateJournalTask(qName, 1);
}

};

