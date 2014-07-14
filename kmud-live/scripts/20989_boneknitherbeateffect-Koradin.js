var script20989 = function(self, actor, here, args, extra)
{
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
if (getSval(actor, 20988, "healing effect") > 0)
{
	actor.send("You can still feel the effects from the last one you ate!");
	_block;
	return;
}
_block;
self.moveToRoom(getRoom(20899));
act("You put " + self.name + " in your mouth, chewing it quickly.", true, actor, null, null, constants.TO_CHAR);
act("$n puts " + self.name + " in $s mouth, chewing it quickly.", true, actor, null, null, constants.TO_ROOM);
actor.attach(20988);
waitpulse 1;
setSval(actor, 20988, "healing effect", 1);
setSval(actor, 20988, "decay status", getDecayProgress(self));

};

