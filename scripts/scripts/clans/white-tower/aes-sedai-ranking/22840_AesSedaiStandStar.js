var script22840 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if (vArgs.length >= 2 && !str_cmp(vArgs[1], "star")) {
		_block;
		AesSedaiRank.Util.standStar(self, actor, here, args);
	}
}