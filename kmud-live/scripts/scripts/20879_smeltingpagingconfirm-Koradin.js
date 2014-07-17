var script20879 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"yes",1)) {
		setSval(actor,20859,"smeltConfirm",1);
		actor.detach(20879);
		actor.comm("smelt "+getSval(actor,20859,"smeltType"));
		return;
	}
	else if (strn_cmp(vArgs[0],"no",1)) {
		actor.detach(20879);
		actor.send("You stop smelting.");
		setSval(actor,20859,"smeltType",null);
		return;
	}
	else {
		actor.send("Type [Y]es or [N]o.");
		return;
	}
}