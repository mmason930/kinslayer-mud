var script9871 = function(self, actor, here, args, extra) {
	//Alder
	//Behind Closed Doors: Inquire Woman's Help
		var qName = "Behind Closed Doors";
		if ( actor.quest(qName) == 3 ) {
			waitpulse 3;
			getCharCols(actor);
			self.comm("sa Thank you for sparing me.");
			waitpulse 9;
			self.comm("sa I am aligned with very powerful people, and thus you have the potential to be as well.");
			waitpulse 17;
			self.comm("sa If you kill me however, these people will find out, and you will never earn their attention beyond a bounty on your head.");
			waitpulse 25;
			self.comm("sa You are treading in very dangerous waters right now, but if you wish to learn more, I will be in Baerlon in a few days.");
			waitpulse 25;
			self.comm("sa I shall have a room in the Stag and Lion. Until we meet again, goodbye. You shall not regret this.");
			waitpulse 14;
			self.comm("stand");
			self.comm("south");
			self.moveToRoom(getRoom(501));
			self.extract();
			actor.qval(qName,4);
			actor.journalEdit("ADD",'Where Ends Meet');
			return;
		}
}