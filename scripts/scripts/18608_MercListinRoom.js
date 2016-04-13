var script18608 = function(self, actor, here, args, extra) {
	//Rhollor January 2010
	//Adapted from Alder's original Journal paging system
	_block;
	if(actor.name == 'Aj' || actor.name == 'Barg' || actor.name == 'Hagar' || actor.name == 'Jerez' || actor.name == 'Merchaneer' || actor.name == 'Obould' || actor.name == 'Pitch' || actor.name == 'Tzurani' || actor.name == 'Uhoh')
	{
		actor.send("You have been banned from using the mercenary system.");
		return;
	}
	actor.attach(18607);
	here.echoaround(actor, capFirstLetter(actor.name) + " begins the mercenary selection.");	
	actor.viewMercList("main menu");
	
}