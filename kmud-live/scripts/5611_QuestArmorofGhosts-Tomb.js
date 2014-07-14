var script5611 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Armor of Ghosts
/** Command Trigger: -read- **/
var qName = "Armor of Ghosts";
if ( actor.quest(qName) == 2 ) {
	_block;
	getCharCols(actor);
	actor.send(bld+cyn+"As you study the inscription on the tomb's wall, you feel imbued with some hidden knowledge."+nrm);
	actor.updateJournalTask(qName,1);
	actor.qval(qName,3);
}

};

