var script20877 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"yes",1)) {
		setSval(actor,20859,"prepConfirm",1);
		actor.detach(20877);
		actor.comm("prepare "+getSval(actor,20859,"prepType"));
		return;
	}
	else if (strn_cmp(vArgs[0],"no",1)) {
		actor.detach(20877);
		actor.send("You stop preparing.");
		setSval(actor,20859,"prepType",null);
		return;
	}
	else {
		actor.send("Type [Y]es or [N]o.");
		return;
	}
}