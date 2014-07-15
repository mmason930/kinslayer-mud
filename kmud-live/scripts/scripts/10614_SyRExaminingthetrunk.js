var script10614 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var targetStr = vArgs[1];
	if ( actor.quest('Search and Rescue') > 0 && targetStr == "trunk" && !actor.questTaskProgress('Search and Rescue')[4].comp ) {
	     _block;
	     getCharCols(actor);
	     actor.send(bld+cyn+"A hole in the earth is visible inside - perhaps you could enter it."+nrm);
	}
}