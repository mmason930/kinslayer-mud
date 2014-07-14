var script1031 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Armor of Ghosts
/** Death Trigger: a walking skeleton (room 1031) **/
var qName = "Armor of Ghosts";
if ( actor.quest(qName) == 3 && self.room.vnum == 1031 ) {
	getCharCols(actor);
	actor.send(bld+cyn+"As the skeleton falls to the ground you spot a small roll of parchment inside the rib cage. That must be a piece of Hok'dar's knowledge."+nrm);
	actor.updateJournalTask(qName,2);
	actor.qval(qName,4);
}

};

