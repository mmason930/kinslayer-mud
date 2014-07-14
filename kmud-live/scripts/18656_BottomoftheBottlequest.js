var script18656 = function(self, actor, here, args, extra)
{
	//Bottom of the Bottle
//Rhollor April 2010
var qName = "Bottom of the Bottle";
if(actor.questTaskProgress(qName)[1].quotient != 1) {
        actor.updateJournalTask(qName, 0);
}

};

