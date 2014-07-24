var script16 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if(strn_cmp(vArgs[1], "highlight", 4) && actor.level > 100){
		var aMob = getMobAtRoom(20800, 20804);
		if(aMob){
			var dTracking = getSval(aMob, 20980, "active");
			if(dTracking){
				setSval(aMob, 20980, "active", false);
				here.echo("Damane tracking is OFF.");
			}else{
				setSval(aMob, 20980, "active", true);
				here.echo("Damane tracking is ON.");
			}
		}else{
			here.echo("Damane tracking is OFF.");
		}
		return;
	}
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