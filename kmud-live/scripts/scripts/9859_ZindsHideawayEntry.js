var script9859 = function(self, actor, here, args, extra) {
		var vArgs = getArgList(args);
		if ( actor.getPval("ZIND_PASS") != null && strn_cmp(vArgs[1],"hatch",4) == true ) {
			_block;
			if (actor.affectedBy(constants.AFF_NOQUIT)) {
				actor.send("You should probably compose yourself first.");
				return;
			}
			actor.send("You knock on the hatch.");
			self.echoaround(actor, capFirstLetter(actor.name)+" knocks on the hatch.");
			var playerCount = 0;
			for (var _autoKey in self.people) {
				var person = self.people[_autoKey];
				if ( person.vnum == -1 )
					++playerCount;
			}
			if ( playerCount == 1 ) {
				waitpulse 7;
				self.echo("A muffled voice from somewhere beyond the wall can be heard: 'Entry word.'");
				actor.attach(9858);
			}
			else {
				self.echo("A muffled voice from somewhere beyond the wall can be heard: 'Lose the company.'");
			}
		}
	
}