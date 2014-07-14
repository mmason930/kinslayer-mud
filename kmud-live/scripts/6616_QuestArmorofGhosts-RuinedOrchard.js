var script6616 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Armor of Ghosts
/** Command Trigger: sit (room 6616) **/
var qName = "Armor of Ghosts";
if ( actor.quest(qName) == 4 ) {
	waitpulse 1;
	getCharCols(actor);
	actor.send(bld+cyn+"Within one of the black puddles all around you suddenly appears a strange reflection which conveys some secret message to you before disappearing as unexpectedly as it came."+nrm);
	actor.updateJournalTask(qName,3);
	actor.qval(qName,5);
}

};

