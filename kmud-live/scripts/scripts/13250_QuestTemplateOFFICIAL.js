var script13250 = function(self, actor, here, args, extra) {
	/**** Generic Quest Template ****/
	function checkQvals(element, elementIndex, targetArray) {
		if ( Quest.getByName(element.name).hasCompleted(actor) )
			return ( element );
			
		return null;
	}
	var quests = actor.getAvailableQuests(self.vnum);
	_block;
	if (!quests.length) {
		actor.send("There are no quests available here at this time.");
		return;
	}
	var vArgs = getArgList(args);
	if( vArgs.length < 2 ) {
		questCommandUsage(self,actor);
		return;
	}
	if( !str_cmp(vArgs[1], "list") ) {
		Quest.Util.listQuests(actor, quests, self);
		return;
	}
	var clanNum = self.findClans(true)[0];
	getCharCols(actor,constants.CL_OFF);
	/********GENERAL ARG CHECKS********/
	if( vArgs.length < 3 ) {
		var cmd = vArgs[1];
		if (vArgs[1] == "map") {
			actor.send("Type '" + cyn + "quest map please" + nrm + "' for a new " + vArgs[1] + ", or '" + cyn + "quest" + nrm + "' for more information.");
			return;
		}
		if (vArgs[1] == "begin" || vArgs[1] == "start" || vArgs[1] == "complete" || vArgs[1] == "info" || vArgs[1] == "abandon" ) {
			actor.send("Yes, but WHICH quest? Specify the quest number.");
			return;
		}
		else if ( strn_cmp(cmd,'greetings',5) == true ) {
			qval = actor.quest('QUEST_GREETINGS');
			actor.qval('QUEST_GREETINGS',!qval);
			actor.send("Quest greetings toggled "+(actor.quest('QUEST_GREETINGS') == 0 ? 'ON' : 'OFF' )+".");
			return;
		}
		else {
			questCommandUsage(self,actor);
			return;
		}
	}
	else if(vArgs.length >= 3)
	{
		/********QUEST MAP PLEASE********/
		if ( !str_cmp(vArgs[1], "map") && actor.room.zoneVnum == 132 ) {
			if ( !str_cmp(vArgs[2], "please") ) {
				if ( !actor.hasItem(13221,true,true,true) ) {
					self.say("Here you are, so you know where you're going!");
					waitpulse 5;
					self.loadObj(13221);
					self.comm("give quest_map " + actor.name);
					wait 1;
					self.say("To look at the map, just 'read map' when you're carrying that.");
					wait 1.5;
					self.say("And if for any reason you lose your map, just type 'quest map please' at a quest giver and you'll be given a new one.");
					return;
				}
				else {
					self.say("What's wrong with the map you have already?");
					return;
				}
			}
			else {
				actor.send("Type '"+cyn+"quest "+vArgs[1]+" please"+nrm+"' for a new "+vArgs[1]+", or '"+cyn+"quest"+nrm+"' for more information.");
				return;
			}
			
			return;
		}
	}
		
	var qNum = vArgs[2] - 1;//Element number of this quest in mob's quest array
	if(!quests[qNum]) {
		actor.send("That is not a valid quest. Type '" + cyn + "quest list" + nrm + "' and pick a number from the list.");
		return;
	}
	
	/********CREATE VARIABLES FOR TEMPLATE********/
	var quest = quests[qNum];//Quest object
	var qName = quest.name;//Quest name
	if (!str_cmp(vArgs[1], "complete")) {
		//Attempt to complete a quest.
		Quest.Util.tryCompleteQuest(actor, quest, self);
	}
	else if (!str_cmp(vArgs[1], "info")) {
		//Provide quest dialog for quest.
		actor.send("You request information on '" + grn + qName + nrm + "' from " + self.name + ".");
		waitpulse 4;
		questDialogue(self, actor, quest);
	}
	else if (!str_cmp(vArgs[1], "start") || !str_cmp(vArgs[1], "begin")) {
		Quest.Util.tryBeginQuest(actor, quest, self, qNum);
	}
	else if (!str_cmp(vArgs[1], "abandon")) {
		Quest.Util.tryAbandonQuest(actor, quest, self);
	}
	else {
		waitpulse 5;
		questCommandUsage(self,actor);
	}
};