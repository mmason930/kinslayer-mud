var script9856 = function(self, actor, here, args, extra) {
		if ( getSval(self,9854,"Actor") == actor ) {
			var vArgs = getArgList(args);
			var cmd = vArgs[0];
			getCharCols(actor,constants.CL_OFF);
			function afterWait(arg) {
				var actor = arg[0];
				actor.updateJournalTask("Just Think...", 0);
				setSval(actor,9854,"TOWER CHECK",undefined);
			}
			if ( strn_cmp(cmd,"shake",2) == true ) {
				actor.qval("ZIND_UNLOCK_SECOND_QUEST", 0);
				waitpulse 3;
				self.comm("say Very well then. I shall leave you now.");
				waitpulse 10;
				actor.send(capFirstLetter(self.name)+" cuts loose a small pouch and disappears over the balcony railing before the clinking of coin is heard as the pouch hits the ground.");
				self.moveToRoom(getRoom(501));
				setTimeout(18,afterWait,[actor,false]);
				self.detach(9855);
				self.extract();
			}
			else if ( strn_cmp(cmd,"nod",3) == true ) {
				actor.qval("ZIND_UNLOCK_SECOND_QUEST", 1);
				waitpulse 3;
				Zind.setNewPassword(actor);
				self.comm("say Excellent. Seek Zind in the Amalize. She's docked in Aringill. Knock on the hatch and the word is "+Zind.printPassword(actor)+". Don't bring anyone with you");
				waitpulse 28;
				actor.send(capFirstLetter(self.name)+" cuts loose a small pouch and disappears over the balcony railing before the clinking of coin is heard as the pouch hits the ground.");
				self.moveToRoom(getRoom(501));
				setTimeout(18,afterWait,[actor,true]);
				self.detach(9855);
				self.extract();
			}
		}
}