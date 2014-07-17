var script920 = function(self, actor, here, args, extra) {
	//Alder
	//June 2011
	/** Mashadar Traps People **/
	if ( self.victims == undefined ) {
		self.victims = [];
	}
	if ( actor.level < 100 && actor.vnum != 920 && !arrContains(self.victims,actor) ) {
		self.victims.push(actor);
		actor.send("The tendrils of Mashadar have invaded your being...");
	}
}