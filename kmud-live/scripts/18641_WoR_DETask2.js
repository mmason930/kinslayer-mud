var script18641 = function(self, actor, here, args, extra)
{
	//WoR_DE Task 2
//Rhollor April 2010
var qName = "WoR: A Dead End";
if(actor.quest(qName)) {
	wait 1;
	self.comm("say If I remember correctly there was a lad that was headed to the western keep looking for a hidden location among the Blight trees.");
	wait 2;
	self.comm("say If that means anything to you anyway... I thought he was nuts.");
	actor.updateJournalTask(qName,1);
}

};

