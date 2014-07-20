var script22812 = function(self, actor, here, args, extra) {
	if( !actor.isAesSedai() ) {
	                        return;
				}
			if (phraseMatch(args, "Please seal the Hall\\.?$")) {
				self.say("Very well then.");
				wait 4;
				for (var _autoKey in here.people) {
					var person = here.people[_autoKey];
					if(person.affectedBy(constants.AFF_NOQUIT)){
						self.say(person.name + " is too flustered right now.")
					return;
					}}
				for (var _autoKey in here.people) {
					var person = here.people[_autoKey];
					if(person.race == constants.RACE_TROLLOC){
						self.say("There's a flaming shadowspawn in here!");
					return;
					}}
				self.comm("close Halldoor");
				wait 4;
				self.comm("lock Halldoor");
				wait 4;
				self.comm("nod");
				}
			if (phraseMatch(args, "Please seal the Hall to the flame\\.?$")) {
				wait 4;
				self.say("As you wish.");
				wait 4;
				for (var _autoKey in here.people) {
					var person = here.people[_autoKey];
					if(person.affectedBy(constants.AFF_NOQUIT)){
						self.say(person.name + " is too flustered right now.")
					return;
					}}
				for (var _autoKey in here.people) {
					var person = here.people[_autoKey];
					if(person.race == constants.RACE_TROLLOC){
						self.say("There's a flaming shadowspawn in here!");
					return;
					}}
				self.comm("close Halldoor");
				wait 4;
				self.comm("lock Halldoor");
				wait 4;
				self.comm("embrace");
				wait 4;
				self.comm("channel 'circle of silence'");
				wait 20;
				}
			if (phraseMatch(args, "Please unseal the Hall\\.?$")) {
				wait 4;
				self.say("Very well then");
				wait 4;
				self.comm("release");
				wait 4;
				self.comm("unlock Halldoor");
				wait 4;
				self.comm("open Halldoor");
				wait 4;
				self.comm("nod");
				}
}