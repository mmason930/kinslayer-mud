var script10663 = function(self, actor, here, args, extra)
{
	//Jenkin
//October 2010
//Quest: The Cover of Darkness
var qName = 'The Cover of Darkness'
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
   actor.send("Your purpose has been achieved.");
   waitpulse 5;
   actor.updateJournalTask(qName, 0);
}

};

