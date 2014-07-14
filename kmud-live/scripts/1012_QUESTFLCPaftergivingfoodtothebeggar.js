var script1012 = function(self, actor, here, args, extra)
{
	var qName = "Feeding Low Caemlyn's Poor";
if ( extra.obj.vnum == 10000 && actor.questTaskProgress(qName)[1].comp && !actor.questTaskProgress(qName)[2].comp ) {
	waitpulse 1;
	self.tell( actor, "Thank the light! I was beginning to think we'd starve. This will keep my family and me fed for weeks.");
	self.moveToRoom(getRoom(501));
	self.extract();
	actor.updateJournalTask( qName, 1 );
}

};

