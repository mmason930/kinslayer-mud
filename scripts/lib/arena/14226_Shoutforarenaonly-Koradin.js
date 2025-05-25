var script14226 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (strn_cmp(vArgs[0],"shout",3) && arrContains(global.arenaPlayers, actor)) {
		actor.send("Use the "+mag+"achat"+nrm+" feature to communicate instead!");
		_block;
		return;
	}
	else {
		_noblock;
		return;
	}
}