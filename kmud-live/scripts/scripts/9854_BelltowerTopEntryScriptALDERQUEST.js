var script9854 = function(self, actor, here, args, extra) {
		var mob = undefined;
		for (var _autoKey in self.people) {
			var person = self.people[_autoKey];
			if ( person.vnum == 9854 )
				var mob = person;
		}
		if ( mob != undefined && getSval(actor,9854,"TOWER CHECK") != true ) {
			_block;
			actor.send("You hear people talking above and get the feeling your presence would be unwanted.");
		}
		else if ( getSval(actor,9854,"TOWER CHECK") == true ) {
			_noblock;
		}
		else {
			waitpulse 1;
			var playerCount = 0;
			for (var _autoKey in self.people) {
				var person = self.people[_autoKey];
				if ( person.vnum == -1 )
					++playerCount;
			}
			if ( actor.race != constants.RACE_HUMAN || actor.quest("Just Think...") != 1 || actor.affectedBy(constants.AFF_NOQUIT) == true || playerCount > 1 ) {
				if ( playerCount > 1 && actor.quest("Just Think...") > 0 && actor.race == constants.RACE_HUMAN ) {
					actor.send("This has to be the place the riddle was talking about, but it did say to come alone...");
				}
				else if ( playerCount == 1 && actor.quest("Just Think...") > 0 && actor.race == constants.RACE_HUMAN && actor.affectedBy(constants.AFF_NOQUIT) == true ) {
					actor.send("This has to be the place the riddle was talking about, but you should probably come back when your heart rate has slowed to look around.");
				}
					return;
			}
			getCharCols(actor, constants.CL_OFF);
			setSval(actor,9855,"Leave",false);
			waitpulse 3;
			actor.send("This must be the location that the riddle was hiding. You're here, but you don't see anything conspicuous...");
			waitpulse 20;
			actor.send("Suddenly a figure clothed in brown swoops in from outside the balcony!");
			self.loadMob(9854);
			var mob = self.people[0];
			waitpulse 20;
			mob.comm("say And here I thought the riddle would have led you to jump off the tower in an attempt to fly.");
			waitpulse 24;
			actor.send(capFirstLetter(mob.name)+" smiles wryly.");
			waitpulse 14;
			mob.comm("say But it is of no matter, the riddle is meaningless now; merely a device to attract able...volunteers.");
			waitpulse 28;
			mob.comm("say It's up to you whether you wish to continue, though I can't tell you what you will be doing in the future. However I'm sure it won't always be very savory.");
			waitpulse 30;
			mob.comm("say All you must do is nod or shake your head. Decide quickly, however.");
			waitpulse 14;
			setSval(actor,9854,"TOWER CHECK",true);
			setSval(actor,9855,"Leave",true);
			setSval(mob,9854,"Actor",actor);
			setSval(mob,9855,"Wait",0);
			mob.attach(9855);
		}
}