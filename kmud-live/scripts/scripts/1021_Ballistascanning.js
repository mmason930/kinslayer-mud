var script1021 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args)
	var izone = self.zoneVnum
	_block;
	if( vArgs[1] == "zone" ){
		actor.send('You scan the sourrounding area for life.')
		actor.startTimer(2.5);
		var success = runTimer(actor);
		if (success){
			for each (var player in getPlayersInZone(izone))
				actor.send(player.name);
			}
	}
}