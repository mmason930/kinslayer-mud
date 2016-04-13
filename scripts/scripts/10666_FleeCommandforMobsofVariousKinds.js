var script10666 = function(self, actor, here, args, extra) {
	//Jenkin
	//October 2010
	//Quest: The Cover of Darkness
	var qName = 'Eternal Servitude'
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp ) {
	   self.comm("flee");
	   }
}