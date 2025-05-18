var script9880 = function(self, actor, here, args, extra) {
	//Alder
	//Monopolize: Smuggling Figures Die
		function afterWait(args) {
			var people = args[0];
			var vnum = args[1];
			var qName = "Monopolize";
			for (var _autoKey in people) {
				var person = people[_autoKey];
				if ( person.quest(qName) > 0 ) {
					if ( vnum == 4519 )
						person.updateJournalTask(qName,0);
					else if ( vnum == 370 )
						person.updateJournalTask(qName,2);
					else if ( vnum == 1801 )
						person.updateJournalTask(qName,1);
				}
			}
		}
		setTimeout(1,afterWait,[here.people,self.vnum]);
}