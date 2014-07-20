var script20400 = function(self, actor, here, args, extra) {
	//For quest: Ghob'hlin Advancement 1
	//Ashivak - June 2010
	function f( vArgs ) {
		var killer = vArgs[0];
		var group = [];
		var onQuest = false;
		for (var _autoKey in killer.getIncompleteQuests()) {
			var quest = killer.getIncompleteQuests()[_autoKey];
			if (quest.questName == "Ghob'hlin Advancement 1")
				onQuest = true;
		}
		/**THE KILLER IS A MOB OR IS ON THE QUEST**/
		if (killer.vnum != -1 || onQuest == true)
		{
			/**THE KILLER IS A FOLLOWER IN THE GROUP**/
			if (killer.leader)
			{
				group.push(killer.leader);
				for (var _autoKey in killer.leader.followers) {
					var member = killer.leader.followers[_autoKey];
					if (member.room.zoneVnum == killer.room.zoneVnum)
						group.push(member);
				}
			}
			/**THE KILLER IS THE LEADER OF THE GROUP**/
			else if (killer.followers)
			{
				group.push(killer);
				for (var _autoKey in killer.followers) {
					var member = killer.followers[_autoKey];
					if (member.room.zoneVnum == killer.room.zoneVnum)
						group.push(member);
				}
			}
		}
		//killer.room.echo("onQuest: "+onQuest);
		for (var _autoKey in group) {
			var person = group[_autoKey];
			//killer.room.echo("TEST: " + person.name + "'s journal is getting updated!");
			person.updateJournalTask("Ghob'hlin Advancement 1",0);
		}
	}
	setTimeout(1,f,[actor]);
}