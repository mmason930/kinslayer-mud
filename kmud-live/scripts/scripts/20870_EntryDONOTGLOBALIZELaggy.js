var script20870 = function(self, actor, here, args, extra) {
	//Global ENTER script for exploration quests
	//Koradin - May 2010
	function afterWait(args) {
		var actor = args[0];
		function roomsTraveled(player) { return player.quest("ROOMS_TRAVELED_COUNTER"); };
		actor.qval("ROOMS_TRAVELED_COUNTER", roomsTraveled(actor)+1);
		if ( roomsTraveled(actor) == 1000 ) {
			//unlock Runner title
			actor.unlockTitle("Runner","normal","silly");
		}
		if ( roomsTraveled(actor) == 10000 ) {
			//unlock 'Major Mileage' achievement (unlocks Marathoner title)
			actor.unlockAchievement(9803);
		}
		if (actor.vnum != -1 || actor.room == undefined || getExplorationTask(actor.room.vnum)[1] == 0)
			return;
		for (var _autoKey in actor.getIncompleteQuests()) {
			var quest = actor.getIncompleteQuests()[_autoKey];
			//here.echo(quest.questName);
			if (checkTag(quest.questName,"Exploration") == true && actor.room.zoneVnum == getExplorationTask(actor.room.vnum)[2].zoneVnum)
			{
				//here.echo(actor.name +" is on the quest");
				var task_name = getExplorationTask(actor.room.vnum);
				if (task_name)
					task_name = task_name[0];
				//here.echo(task_name);
				var index = getTaskIndex(quest.questName,task_name);
				//here.echo(index);
				if (index > -1 && actor.checkVisitedRooms(quest.questName+"_"+task_name) == false)
				{
					actor.updateJournalTask(quest.questName,index);
					actor.updateVisitedRooms(quest.questName+"_"+task_name);
				}
			}
		}
	}
	setTimeout(1,afterWait,[actor]);
}