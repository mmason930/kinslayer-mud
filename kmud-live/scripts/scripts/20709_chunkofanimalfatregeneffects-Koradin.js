var script20709 = function(self, actor, here, args, extra) {
	//chunk of animal fat regen effect
	//Koradin - June 2010
	var vArgs = getArgList(args);
	if (!vArgs[1] || vArgs[2])
	{
	        _noblock;
	        return;
	}
	var trigger = false;
	for each (var name in self.namelist.split(" "))
	{
	        if (!str_cmp(vArgs[1],name))
	                trigger = true;
	}
	if (trigger == false)
	{
	        _noblock;
	        return;
	}
	if (getSval(actor, 20709, "regen effect") > 0)
	{
	        actor.send("You can still feel the effects from the last one you ate!");
	        _block;
	        return;
	}
	_block;
	self.moveToRoom(getRoom(20899));
	act("You put " + self.name + " in your mouth, chewing it quickly.", true, actor, null, null, constants.TO_CHAR);
	act("$n puts " + self.name + " in $s mouth, chewing it quickly.", true, actor, null, null, constants.TO_ROOM);
	actor.attach(20710);
	waitpulse 1;
	setSval(actor, 20709, "regen effect", 1);
	setSval(actor, 20709, "decay", getDecayProgress(self));
}