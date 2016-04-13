var script10639 = function(self, actor, here, args, extra) {
	///Jenkin 
	//September 2010
	//Quest: Shienaran Expedition
	var qName = 'The Shienaran Expedition';
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp ) {
	here.loadMob(18109);
	}
}