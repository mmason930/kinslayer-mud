var script18635 = function(self, actor, here, args, extra)
{
	//WoR_FaI Pick up Idol
//Rhollor April 2010
var qName = "WoR: Idol Ruin";
if(actor.questTaskProgress(qName)[1].quotient != 1) {
	actor.updateJournalTask(qName, 0);
}

};

