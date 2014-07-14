var script44 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if (vArgs[1]) {
	if (vArgs[1] < 1) {
		_block;
		actor.send("You have to give at least SOMETHING...");
		return;
	}
	else if (vArgs[1] > 9999) {
		_block;
		actor.send("Way too much! Try giving a different denomination.");
		return;
	}
}

};

