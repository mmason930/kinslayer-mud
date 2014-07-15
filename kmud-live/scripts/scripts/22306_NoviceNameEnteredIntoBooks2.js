var script22306 = function(self, actor, here, args, extra) {
	if( !actor.clanCheck( [16,17,18,19,20,21,22,23] ) && actor.quest( "novice_proposition" ) == 1 )
	{
		wait 2;
		var target = actor;
		if( target ) {
			self.say("Very well, Child.");
			wait 1;
			if( target && target.room == self.room ) {
				self.comm("emote opens a large leather bound volume on her desk.");
			}
			wait 1;
			if( target && target.room == self.room ) {
				self.comm("scribbles a line in the large book and blots it with sand.");
				actor.clan(16);
			}
			wait 2;
			if( target && target.room == self.room ) {
				var dress = self.loadObj( 22305 );
				self.comm( "give dress " + target.name );
			}
			wait 1;
			if( target && target.room == self.room ) {
				self.say("Put this on and do not let me see you without it!");
			}
			wait 2;
			if( target && target.room == self.room ) {
				self.say("Knock at the landing and you will find Suana Sedai within, now go Child.");
				target.qval( 'novice_proposition', 2 );
			}
			actor.send(cyn + "You may seek assistance in the White Tower forums found at http://kinslayermud.org/forums/viewforum.php?f=24." + nrm);
		}
	}
}