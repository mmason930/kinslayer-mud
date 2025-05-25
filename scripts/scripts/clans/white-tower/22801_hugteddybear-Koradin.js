var script22801 = function(self, actor, here, args, extra) {
	const vArgs = getArgList(args);
	if (vArgs.length < 2) {
		_noblock;
		return;
	}

	if (!str_cmp(vArgs[1], "teddy") ||
		!str_cmp(vArgs[1], "bear") ||
		!str_cmp(vArgs[1], "enormous")
	) {
		actor.send("You squeeze " + self.name + ", bringing back memories of your childhood.");
		echoaround(actor, actor.name + " squeezes " + self.name + " fondly, remembering " + actor.hisHer() + " childhood.");
		_block;
	}
}