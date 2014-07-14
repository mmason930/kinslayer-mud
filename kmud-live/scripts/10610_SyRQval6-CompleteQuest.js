var script10610 = function(self, actor, here, args, extra)
{
	var qName = 'Search and Rescue';
var rescued = actor.getCharRoomVis("mara");
if ( actor.quest(qName) > 0 && rescued != undefined && actor == rescued.leader && !actor.questTaskProgress(qName)[6].comp ) {
     waitpulse 7
     getCharCols(actor);
     actor.send(bld+cyn+"Cheers circulate around the base as Mara returns."+nrm);
     actor.send(bld+cyn+"Terrion beams at you and whistles loudly."+nrm);
     waitpulse 14
     actor.send("A sly courier arrives from the north.");
     waitpulse 14
     actor.send("Lord Terrion gives a dimly glowing package to a sly courier.");
     waitpulse 14
     actor.send("A sly courier leaves north.");
     waitpulse 14
     actor.updateJournalTask(qName,5);
rescued.extract()
}

};

