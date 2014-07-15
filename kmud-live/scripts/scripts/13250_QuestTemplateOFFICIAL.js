var script13250 = function(self, actor, here, args, extra) {
		/**** Generic Quest Template ****/
		function checkQvals(element, elementIndex, targetArray) {
			if ( actor.quest(element.questName) == -1 )
				return ( element );
		}
		var quests = actor.getAvailableQuests(self.vnum);
		_block;
		if ( quests.every(checkQvals) == true ) {
			actor.send("There are no quests available here at this time.");
			return;
		}
		var vArgs = getArgList(args);
		if( vArgs.length < 2 ) {
			questCommandUsage(self,actor);
			return;
		}
		if( !str_cmp(vArgs[1], "list") ) {
			questList(self,actor,quests);
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
			else if ( strn_cmp(cmd,'updates',3) == true ) {
				qval = actor.quest('QUEST_UPDATES');
				actor.qval('QUEST_UPDATES',!qval);
				actor.send("Quest updates toggled "+(actor.quest('QUEST_UPDATES') == 0 ? 'ON' : 'OFF' )+".");
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
		if( qNum >= quests.length || qNum < 0 || isNaN(qNum) ) {
			actor.send("That is not a valid quest. Type '"+cyn+"quest list"+nrm+"' and pick a number from the list.");
			return;
		}
	
		/********CREATE VARIABLES FOR TEMPLATE********/
		var quest = quests[qNum];//Quest object
		var qName = quest.questName;//Quest name
		var taskInfo = actor.questTaskProgress(qName);//Array with taskInfo from questTaskProgress
		actor.recreateJournal();
		if( actor.quest(qName) == -1 ) {
			actor.send("You have already completed that quest.");
				return;
		}
		/********QUEST COMPLETE********/
		else if( !str_cmp(vArgs[1], "complete") ) {
			//Attempt to complete a quest.
			if( actor.quest(qName) == 0 ) {
				actor.send("You have not yet begun that quest.");
				return;
			}
			//Access any special dialogue for beginning and completing a quest.
			if ( taskInfo != null ) {
				questComplete(self,actor,quests,vArgs[2],clanNum);
			}
		}
		/********QUEST INFO********/
		else if( !str_cmp(vArgs[1], "info") ) {
			//Provide quest dialog for quest.
			actor.send("You request information on '" +grn+ qName +nrm+ "' from " + self.name + ".");
			waitpulse 4;
			// actor.send(red + capFirstLetter(self.name) + " nods to you, saying 'Listen closely, " + actor.name + "!'" + nrm);
			questDialogue( self, actor, quest );
		}
		/********QUEST BEGIN********/
		else if( !str_cmp(vArgs[1], "start") || !str_cmp(vArgs[1], "begin") ) {
				if ( actor.quest(qName) > 0 ) 	{
					actor.send("You have already begun that quest!");
						return;
				}
				/**SPECIAL DIALOGUE**/
				for each ( var line in quest.dialogue ) {
					if ( line.pulses == '-1' || line.pulses == -1 ) {
						if ( line.handle[line.handle.length - 1] != ';' ) {
							line.handle += ';';
						}
						eval(line.handle);
					}
				}
				if ( getObjectClass(quest.dialogue[quest.dialogue.length - 1]) == "String" ) {
					var special_dialogue = quest.dialogue[quest.dialogue.length - 1].split("~");
					self.comm("tell " + actor.name + " " + special_dialogue[0]);
				}
				actor.send("You begin the quest '" + bld + cyn + qName + nrm + "'");
				actor.send("To receive information on this quest, type '"+cyn+"quest info " + (qNum+1)+nrm + "' or read your quest journal using the "+cyn+"journal"+nrm+" command.");
				/**CREATE JOURNAL ENTRY FOR QUEST**/
				waitpulse 1;
				actor.qval(qName+"_GIVER",self.vnum);
				actor.journalEdit("ADD",qName);
		}
		/********QUEST ABANDON********/
		else if( !str_cmp(vArgs[1], "abandon") ) {
			if( actor.quest(qName) == 0 ) {
				actor.send("You have not yet begun that quest, so there's nothing to abandon!");
				return;
			}
			else if( actor.quest(qName) == -1 ) {
				actor.send("You have already completed that quest!");
				return;
			}
			actor.send("You abandon the quest '" + qName + "'.");
			var len = actor._questInv.length;
			for ( var i = 0; i < len; i++ ) {
				var item = actor._questInv[i];
				if ( item.questName == qName ) {
					actor.remQuestItem(item.itemName, item.itemAmount, item.questName);
				}
			}
			waitpulse 1;
			actor.journalEdit("REMOVE",qName);
		}
		else {
			waitpulse 5;
			questCommandUsage(self,actor);
		}
	
	
}