var script1009 = function(self, actor, here, args, extra) {
	var qName = "Feeding Low Caemlyn's Poor";
	if ( actor.questTaskProgress(qName)[1].comp && !actor.questTaskProgress(qName)[2].comp ) {
		waitpulse 1;
		self.tell(actor, "Are you from the Tower? We're so hungry.");
	}
		
}