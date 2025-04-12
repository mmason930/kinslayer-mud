var script19502 = function(self, actor, here, args, extra) {
	if( here.items.length !== 0 && here.vnum !== 170 )
	{
		let randomIndex = random(0,here.items.length-1);
		let obj = here.items[randomIndex];
		if( obj.canWear(constants.ITEM_WEAR_TAKE) && !isName("corpse", obj.namelist) && self.canSee(obj))
		{
			act("$n gets $p.", false, self, obj, null, constants.TO_ROOM);
			act("You get $p.", false, self, obj, null, constants.TO_CHAR);
			obj.moveToChar(self);
		}
	}
}