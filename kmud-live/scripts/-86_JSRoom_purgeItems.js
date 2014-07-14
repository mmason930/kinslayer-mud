// Purges all of the objects in a room. If skipChests is true, skips all objects flagged with ITEM_CHEST
JSRoom.prototype.purgeItems = function( skipChests) {
	for each(var obj in this.items) {
		if( !skipChests || !obj.extraFlags(constants.ITEM_CHEST) )
			obj.extract();
	}
}
JSRoom.prototype.purgeMobs = function() {
	for each(var mob in this.people) {
		if (mob.vnum > 0)
			mob.extract();
	}
}
