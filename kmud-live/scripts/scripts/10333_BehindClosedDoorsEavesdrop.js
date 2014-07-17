var script10333 = function(self, actor, here, args, extra) {
	//Alder
		var qName = "Behind Closed Doors";
		if ( actor.quest(qName) == 1 ) {
			_block;
			actor.qval(qName,2);
			getCharCols(actor);
			actor.send("You try to move stealthily to the door in order to eavesdrop on whoever is behind it.\n");
			var sneak = actor.getSkill(getSkillVnum("Sneak"));
			actor.startTimer(3);
			var success = runTimer(actor);
			if ( success && random(1,99) <= sneak ) {
				actor.send(cyn+bld+"\nSucceeded"+nrm);
				actor.exp(100000);
				waitpulse 5;
				actor.send("With your ear to the door, the muffled voices of a man and woman can be heard.");
				waitpulse 16;
				actor.send("Female voice: 'It just seems like a waste of time to me.'");
				waitpulse 14;
				actor.send("Male voice: 'You do not understand these things; they take time.'");
				waitpulse 15;
				actor.send("Male voice: 'It will not be much longer before our plan can be put into motion.'");
				waitpulse 21;
				actor.send("There is a brief pause in the conversation and you hear the sound of glass knocking against wood.");
				waitpulse 24;
				actor.send("Female voice: 'Well you know that he will discover us if we don't act soon.'");
				waitpulse 17;
				actor.send("Female voice: 'And what about that other fellow? The one from Andor.'");
				waitpulse 16;
				actor.send("You struggle to hear as the volume of the voices drops.");
				waitpulse 14;
				actor.send("Male voice: '...about him here Nella. He would kill us if...'");
				waitpulse 17;
				actor.send("Female voice: 'Ambla is our.....operation has encroached much too far.'");
				waitpulse 17;
				actor.send("You hear the sound of chairs scraping against the floor, and then the moving of locks behind the door.");
				waitpulse 24;
				actor.qval(qName,3);
				actor.send("The door opens wide enough for a woman to step out. She sees you but you prevent her from readying her weapon.");
				self.loadMob(9870);
				var nella = self.people[0];
				waitpulse 24;
				actor.send(bld+"The woman yields, knowing you have control of the situation."+nrm);
				waitpulse 12;
				actor.send(cyn+bld+"You demand she tell you who she is and what she was doing here."+nrm);
				waitpulse 14;
				actor.send(bld+"The woman gasps, 'My name is Nella Dierdic.'");
				actor.updateJournalTask(qName,0);
				waitpulse 10;
				nella.comm("say If you know what is good for you, you will not kill me. I can help you.");
				waitpulse 10;
				actor.send(bld+"The woman grins slightly, eyeing you closely."+nrm);
				waitpulse 14;
				actor.send(bld+cyn+"You have obtained adequate information for Zind. Either kill the woman, leave her, or inquire about her 'help'."+nrm);
				nella.hps = 5;
				return;
			}
			else {
				actor.send(red+bld+"\nFailed"+nrm);
				waitpulse 5;
				actor.send("The sound of bolts being moved behind the door reaches you.");
				waitpulse 12;
				actor.send("The door opens wide enough for a woman to step out.");
				waitpulse 13;
				actor.send("The woman says, 'What the hell are you doing out here? Eavesdropping? For this, you die.'");
				waitpulse 18;
				actor.send(red+bld+"The woman brandishes her weapon and rushes you!"+nrm);
				self.loadMob(9870).comm("kill "+actor.name);
				waitpulse 7;
				actor.send(cyn+bld+"If you can defeat the woman you may be able to obtain some information from her."+nrm);
				return;
			}
		}
}