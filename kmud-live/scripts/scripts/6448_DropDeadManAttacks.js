var script6448 = function(self, actor, here, args, extra) {
		var qName = "Drop Dead";
		if ( extra.direction == constants.WEST ) {
			if ( actor.quest(qName) == 2 ) {
				_block;
				function afterWait(args) {
					var actor = args[0];
					actor.detach(6449);
					if ( actor.quest("Drop Dead") == 3 ) {
						actor.qval("Drop Dead",4);
						getCharCols(actor);
						actor.send(red+"Before you can react the brown-garbed man is on you, stabbing with his dagger!"+nrm);
						actor.damage(100);
						var mob = actor.room.loadMob(9854);
						mob.comm("east");
						mob.moveToRoom(getRoom(501));
						mob.extract();
					}
				}
				actor.qval(qName,3);
				var room = actor.room;
				room.echo("A man dressed in brown dashes out from behind the mausoleum!");
				waitpulse 12;
				getCharCols(actor);
				actor.send(bld+"*"+red+"He's headed straight at you with dagger raised!"+nrm+bld+"*"+nrm);
				waitpulse 9;
				actor.send("You could "+cyn+"stop"+nrm+" him if you react quick enough.");
				actor.attach(6449);
				waitpulse 5;
				setTimeout(7,afterWait,[actor]);
			}
			else if ( actor.quest(qName) == 3 ) {
				_block;
				// actor.qval("Drop Dead",2);
				actor.send("The man is moving too fast for you to escape!");
			}
		}
}