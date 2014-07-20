var script1400 = function(self, actor, here, args, extra) {
	var playerList = getConnectedPlayers();
	var humans = 0;
	var trollocs = 0;
	
	for (var _autoKey in playerList) {
	
		var person = playerList[_autoKey];
		if( person.level >= 100 )
			continue;
		else if( person.race == constants.RACE_HUMAN )
			humans++;
		else if( person.race == constants.RACE_TROLLOC )
			trollocs++;
	}
	
	here.echo( "Humans: " + humans );
	here.echo( "Trollocs: " + trollocs );
}