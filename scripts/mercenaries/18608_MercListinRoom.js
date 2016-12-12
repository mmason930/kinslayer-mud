var script18608 = function(self, actor, here, args, extra) {
	//Rhollor January 2010
	//Adapted from Alder's original Journal paging system
	_block;

	actor.attach(18607);
	here.echoaround(actor, capFirstLetter(actor.name) + " begins the mercenary selection.");	
	actor.viewMercList("main menu");
};
