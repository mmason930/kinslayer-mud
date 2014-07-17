var script169 = function(self, actor, here, args, extra) {
	var rand = random(1,3);
	if( rand == 1 )
		self.comm("yell Amazing sales at unbeatable prices! Come visit the Andoran Bazaar to see what we have to offer!");
	else if( rand == 2 )
		self.comm("Peddlers and visitors of Caemlyn, come to the Andoran Bazaar to satisfy all of your merchant needs!");
	else if( rand == 3 )
		self.say("Step right in! The Andoran Bazaar awaits just to the west!");
}