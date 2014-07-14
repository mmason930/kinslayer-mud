var script14217 = function(self, actor, here, args, extra)
{
	function afterWait(args) {
	var actor = args[0];
	var self = args[1];
	if (actor.eq(constants.WEAR_WIELD)) {
		actor.comm("drop "+self.namelist.split(" ")[0]);
		if (arrContains(actor.inventory,self)) {
			actor.send("You drop "+self.name+".");
			self.moveToRoom(actor.room);
			actor.detach(14215);
		}
		actor.sps = (Math.floor(actor.maxSpell*.6));
		actor.shadowPoints = (Math.floor(actor.maxShadow*.6));
	}
}
setTimeout(1,afterWait,[actor,self]);

};

