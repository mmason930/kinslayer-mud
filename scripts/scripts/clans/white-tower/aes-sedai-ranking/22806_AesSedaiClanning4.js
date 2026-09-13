var script22806 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if (vArgs.length >= 2 && !str_cmp(vArgs[1], "portal")) {
		if (AesSedaiRank.Util.enterPortal(self, actor, here, args)) {
			_block;
		}
	}
}