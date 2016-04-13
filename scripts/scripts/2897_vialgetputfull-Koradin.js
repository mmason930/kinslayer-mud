var script2897 = function(self, actor, here, args, extra) {
	if (!arrContains(self.namelist.split(" "), "vialisnowfull"))
	{
		_noblock;
		return;
	}
	var argArray = getArgList(args);
	if (!argArray[1] || !argArray[2] || argArray[3]) {
		_noblock;
		return;
	}
	if (strn_cmp(argArray[0],"put",1))
		var message = "The vial is already full! You can't fit anything else in.";
	else if (strn_cmp(argArray[0],"get",1))
		var message = "You can't reach in without spilling it!  Try pouring it out.";
	var item = getObjInListVis(actor,argArray[2],actor.inventory);
	if (item == self) {
		actor.send(message);
		_block;
		return;
	}
	item = getObjInListVis(actor,argArray[2],actor.room.items);
	if (item == self) {
		actor.send(message);
		_block;
		return;
	}
}