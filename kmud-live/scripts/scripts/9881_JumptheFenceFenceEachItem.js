var script9881 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//Jump the Fence: Fence Each Item
	var qName = "Jump the Fence";
	var quest = Quest.getByName(qName);
	if ( actor.quest(qName) != 1 )
		return;
	var aFence = 4541;	//Baerlon Thief Shopkeep
	var bFence = 352;	//Four Kings Merchant
	var cFence = 1751;	//Maika the Seamstress
	var vnum = self.vnum;
	args = getArgList(args);	
	switch ( vnum ) {
		case aFence://Fights
			if ( actor.checkQuestItem("a delicate ceremonial dagger",1,quest) ) {
				_block;
				waitpulse 7;
				self.comm("say Excellent. Yet another blade for my collection.");
				waitpulse 10;
				actor.remQuestItem("a delicate ceremonial dagger",-1,quest);
				waitpulse 10;
				actor.updateJournalTask(qName,2);
				waitpulse 10;
				actor.send(capFirstLetter(self.name)+" reaches behind her back and pulls out a pair of manacles.");
				waitpulse 15;
				self.comm("say By law of Andor, you are under arrest for trafficking stolen property.");
				waitpulse 18;
				getCharCols(actor,constants.CL_OFF);
				actor.send(cyn+bld+"It seems this fence is an undercover guard, you could run or -kill- her, though killing her would probably result in a warrant by Andor."+nrm);
			}
			break;
		case bFence:
			if ( actor.checkQuestItem("a small clay sculpture",1,quest) ) {
				_block;
				waitpulse 7;
				self.comm("say I've been seeking this piece for quite some time now. And it's even in good condition.");
				waitpulse 14;
				actor.remQuestItem("a small clay sculpture",-1,quest);
				waitpulse 7;
				actor.updateJournalTask(qName,3);
			}
			break;
		case cFence:
			if ( actor.checkQuestItem("a bolt of golden silk",1,quest) ) {
				_block;
				waitpulse 7;
				self.comm("say This will go perfectly with my rare Seanchan tapestries. Thank you "+actor.name+".");
				waitpulse 14;
				actor.remQuestItem("a bolt of golden silk",-1,quest);
				waitpulse 7;
				actor.updateJournalTask(qName,4);
			}
			break;
		default:
			_noblock;
			break;
	}
	
}