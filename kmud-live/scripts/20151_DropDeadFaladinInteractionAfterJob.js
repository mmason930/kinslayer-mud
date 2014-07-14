var script20151 = function(self, actor, here, args, extra)
{
		var qName = "Drop Dead";
	var qval = actor.quest(qName);
	if ( qval > 1 && qval != 20 ) {
		getCharCols(actor,constants.CL_OFF);
		waitpulse 5;
		actor.send(cyn+bld+"You inform Lord Faladin of the attacker you faced."+nrm);
		waitpulse 8;
		self.comm("say Attacked?! Did you kill the man?");
		if ( qval < 10 ) {
			waitpulse 5;
			actor.send(bld+"Respond:"+nrm+cyn+"\n'No'\n'Yes' (Lie: int/wis roll)"+nrm);
			actor.qval(qName,5);
			return;
		}
		else if ( qval == 10 ) {
			actor.qval(qName,20);
			waitpulse 10;
			actor.send(cyn+bld+"You explain to Faladin how you killed the man."+nrm);
			waitpulse 10;
			self.comm("sa Well at least he didn't get away; no doubt he was sent by someone who knew about the dead drop.");
			waitpulse 21;
			self.comm("sa As promised, your bonus. Plus something extra for your work in foiling the attacker.");
			actor.loadObj(9860);
			actor.gold += 2000;
//			self.comm("mgold "+actor.name+" 2000");
			actor.send(capFirstLetter(self.name) + " gives you" + actor.numCopperToText(2000,true) + ".");
			actor.send(capFirstLetter(self.name) + " gives you a long stiletto with blue and white grip shagreen.");
			waitpulse 21;
			self.comm("sa I appreciate your help on this job. I may have more for you to do in the future. For now, goodbye.");
			return;
		}
	}
			

};

