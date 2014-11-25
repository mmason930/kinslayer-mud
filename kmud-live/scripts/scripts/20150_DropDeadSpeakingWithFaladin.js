var script20150 = function(self, actor, here, args, extra) {
	var qName = "Drop Dead";
	var quest = Quest.getByName(qName);
	var qval = actor.quest(qName);
	if ( qval < 0 || qval > 9 ) {
		return;
	}
	var word = getArgList(args)[0];
	var job = ( strn_cmp(word,"job",3) == true ? true : false );
	var yes = ( strn_cmp(word,"yes",1) == true ? true : false );
	var no = ( strn_cmp(word,"no",2) == true ? true : false );
	if ( job == true && Quest.getByName(qName).tasks[0].isInProgress(actor) && actor.checkQuestItem("a small porcelain urn",1,quest) == false ) {
		_block;
		waitpulse 3;
		self.comm("say I see. Zind must have sent you. I'll keep this short.");
		waitpulse 7;
		self.comm("say Take this urn. Do not open it. Do not lose it.");
		actor.addQuestItem("a small porcelain urn",1,quest);
		waitpulse 18;
		getCharCols(actor);
		self.comm("say You will travel to the mausoleum in the Borderlands and "+cyn+"place"+nrm+" the urn inside.");
		waitpulse 16;
		self.comm("say It's a dead drop, so don't expect anyone to be there.");
		waitpulse 7;
		self.comm("say Return to me when you've finished the job and I'll compensate you with a bonus on top of what you're getting from Zind.");
		waitpulse 20;
		self.comm("say But either way I'll know if you've done the job.");
		actor.updateJournalTask(qName,0);
	}
	else if ( yes == true && qval == 5 ) {
		_block;
		var r1 = 21 - actor.intelligence;
		var r2 = 21 - actor.wisdom;
		var r3 = r1+r2;
		var roll = random(0,r3);
		actor.send(bld+"You lie to Faladin about killing the man."+nrm);
		getCharCols(actor,constants.CL_OFF);
		actor.qval(qName,20);
		if ( roll == 0 ) {
			actor.send(cyn+bld+"Succeeded"+nrm);
			actor.experience += 100000;
			waitpulse 13;
			self.comm("sa Well at least he didn't get away; no doubt he was sent by someone who knew about the dead drop.");
			waitpulse 26;
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
		else {
			actor.send(red+bld+"Failed"+nrm);
			waitpulse 13;
			self.comm("sa Do not lie to me, "+actor.name+". All you had to do was tell me the truth.");
			waitpulse 18;
			self.comm("sa Now you will receive no bonus from me. I hope as you run back to Zind you feel the regret of this poor decision.");
			waitpulse 24;
			self.comm("sa Goodbye.");
			return;
		}
	}
	else if ( no == true && qval == 5 ) {
		_block;
		waitpulse 7;
		self.comm("sa Well at least he wasn't able to seriously damage our operation; no doubt he was sent by someone who knew about the dead drop.");
		waitpulse 21;
		self.comm("sa As promised, your bonus.");
		self.comm("mgold "+actor.name+" 2000");
		actor.send(capFirstLetter(self.name) + " gives you" + actor.numCopperToText(2000,true) + ".");
		waitpulse 20;
		self.comm("sa I appreciate your help on this job. I may have more for you to do in the future. For now, goodbye.");
		return;
	}
		
}