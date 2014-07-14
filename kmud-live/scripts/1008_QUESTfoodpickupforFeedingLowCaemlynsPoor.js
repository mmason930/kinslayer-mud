var script1008 = function(self, actor, here, args, extra)
{
	var qName = "Feeding Low Caemlyn's Poor";
if ( actor.quest(qName) > 0 && !actor.questTaskProgress(qName)[1].comp ) {
	waitpulse 1;
	self.tell(actor, "Thank the light you're here! Please, take these packages and distribute them to the poor right away!");
	self.loadObj(10000);
	self.loadObj(10000);
	self.loadObj(10000);
	self.comm("give all.package "+actor.name);
	actor.updateJournalTask( qName, 0 );
}

};

