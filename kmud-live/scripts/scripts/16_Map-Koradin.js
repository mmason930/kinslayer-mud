var script16 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[1], "legend", 3)) {
		var legToggle = actor.quest("MapLegend");
		if (legToggle == 2) {
			actor.send("Your map legend is now ON.");
			actor.qval("MapLegend",1);
		}
		else {
			actor.send("Your map legend is now OFF.");
			actor.qval("MapLegend",2);
		}
		return;
	}
	var bool = true;
	if (actor.quest("MapLegend") == 2)
		bool = false;
	actor.send(global.helpManager.getWorldMap(actor, bool));
	act("$n takes a look at $s map.",true,actor,null,null,constants.TO_ROOM);
}