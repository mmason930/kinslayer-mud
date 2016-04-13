var script19502 = function(self, actor, here, args, extra) {
	if( here.items.length != 0 && here.vnum != 170 )
	{
	    var randomIndex = random(0,here.items.length-1);
	    var obj = here.items[randomIndex];
	if( obj.canWear(constants.ITEM_WEAR_TAKE) && !isName("corpse", obj.namelist) && !obj.hidden)
	    {
	        here.echoaround(self, self.name + " gets " + obj.name + ".");
	        self.send("You get " + obj.name + ".");
	        obj.moveToChar(self);
	    }
	}
}