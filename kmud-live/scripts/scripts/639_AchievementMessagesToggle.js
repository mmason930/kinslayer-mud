var script639 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//Global ACHIEVEMENT MESSAGES Toggle
	if ( actor.vnum != -1 )
		return;
	var args = getArgList(args);
	if ( strn_cmp(args[0],"achievement",3) && strn_cmp(args[1],"messages",3) ) {
		_block;
		var cVal = actor.quest("HIDE_ACH_TITLE_MSG");
		actor.qval("HIDE_ACH_TITLE_MSG",!cVal);
		if ( !cVal ) {
			actor.send("Achievement/title unlock messages OFF.");
		}
		else {
			actor.send("Achievement/title unlock messages ON.");
		}
	}
}