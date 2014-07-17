var script20900 = function(self, actor, here, args, extra) {
	//QUEST TEMPLATE for 'Snakes and Berries'
	//Koradin - January 2010
	/*******A bustling cook Quest Master*******/
		//--Snakes and Berries--//
		 /**Summary: Player must find the berry bush north of EF by the river,
		 and pick berries to get a handful. They must also kill 5 scarlet snakes,
		 then return to the cook to receive their reward.
			_____________________________________
		   |Requirements: none                   |
		   |                                     |
		   |			                         |
		   |Reward(s):  1500 exp, 150 copper     |
		   |_____________________________________|		
			**/
	var clanVnum = -1;
	_block;
	var vArgs = getArgList(args);
	if( vArgs.length < 2 ) {
	    questCommandUsage(self,actor);
	        return;
	}
	var quests = getQuestsByMob( self.vnum );
	if( !str_cmp(vArgs[1], "list") ) {
	    questList(self,actor,quests);
	}
	else if( !str_cmp(vArgs[1], "complete") ) {
		if (actor.quest('Snakes and Berries') >= 100 && actor.hasItem(21431,true,true,true))
		{
			self.comm("smile");
			self.say("Bran is going to love this pie! Thank you " + actor.name + "!");
			here.echo(capFirstLetter(self.name) + " puts the berries away in preparation for her pie.");
		}
	    questComplete(self,actor,quests,vArgs[2],clanVnum);
	}
	else if( !str_cmp(vArgs[1], "info") ) {
	    //Provide quest dialog for quest.
	        if( vArgs.length < 3 ) {
	            actor.send("Yes, but WHICH quest? Specify the quest number.");
	                return;
	        }
	        var qNum = vArgs[2]-1;
	        if( qNum >= quests.length || qNum < 0 ) {
	            actor.send("That is not a valid quest. Type '"+actor.cyan(constants.CL_OFF)+"quest list"+actor.normal(constants.CL_OFF)+"' and pick a number from the list.");
	                return;
	        }
	        waitpulse 5;
	        if( actor.quest(quests[qNum].questName) == -1 ) {
	            actor.send("You've already completed that quest!");
					return;
	        }
	        // if( actor.quest(quests[qNum].questName) != 1 ) {
	            // actor.send("You have not yet begun that quest.");
					// return;
	        // }
		/**** Quest Dialog goes below ****/
		if( quests[qNum].questName == 'Snakes and Berries' ) {
			self.comm("nod");
			wait 2;
			self.comm("tell " + actor.name + " There are some wild berries growing along the river north of town.");
			wait 3;
			self.comm("tell " + actor.name + " I would love to have some to bake Bran a pie, but...");
			wait 3;
			self.comm("tell " + actor.name + " I'm deathly afraid of the snakes that inhabit the grassy fields up there!");
			wait 3;
			self.comm("tell " + actor.name + " Would you go pick some berries for me? And kill some snakes, too, so they can't bother me anymore!");
			wait 2;
		}
	}
	else if( !str_cmp(vArgs[1], "start") || !str_cmp(vArgs[1], "begin") ) {
	        if( vArgs.length < 3 ) {
	            actor.send("Yes, but WHICH quest? Specify the quest number.");
	                return;
	        }
	        var qNum = vArgs[2]-1;
	        if( qNum >= quests.length || qNum < 0) {
	            actor.send("That is not a valid quest. Type 'quest list' and pick a number from the list.");
	                return;
	        }
	        waitpulse 5;
	        if( actor.quest(quests[qNum].questName) == -1 ) {
	            actor.send("You've already completed that quest!");
	                return;
	        }
			else if ( actor.quest(quests[qNum].questName) > 0 ) {
				actor.send("You've already begun that quest!");
	                return;
			}
			self.say("Fantastic! Thank you so much, " + actor.name + "!");
	        actor.qval(quests[qNum].questName, 1);
	        actor.send("You begin the quest '" + actor.bold(constants.CL_OFF) + actor.cyan(constants.CL_OFF) + quests[qNum].questName + actor.normal(constants.CL_OFF) + "'");
	        actor.send("To receive information on this quest, type "+actor.cyan(constants.CL_OFF)+"'quest info " + (qNum+1) + "'"+actor.normal(constants.CL_OFF));
			//If actor doesn't have a journal, give them one
			if ( !actor.hasItem(13220,true,true,true) ) {
				self.say("Here, have this to keep track of any quests you're given.");
				waitpulse 5;
				self.loadObj(13220);
				self.comm("give quest_journal " + actor.name);
				wait 1;
				self.say("To look at current and past quests, just 'read journal' when you're carrying that.");
				wait 1.5;
				self.say("And if for any reason you lose your journal, just type 'quest journal please' at a quest master and you'll be given a new one.");
			}
			var quest = quests[qNum];
			var qName = quests[qNum].questName;
			//Create a journal entry for this quest
			waitpulse 1;
			actor.editQuestJournal(qName,true);
	}
	else if ( !str_cmp(vArgs[1], "journal") && !str_cmp(vArgs[2], "please") ) {
		if ( !actor.hasItem(13220,true,true,true) ) {
			self.say("Here, have this to keep track of any quests you're given.");
			waitpulse 5;
			self.loadObj(13220);
			self.comm("give quest_journal " + actor.name);
			wait 1;
			self.say("To look at current and past quests, just 'read journal' when you're carrying that.");
			wait 1.5;
			self.say("And if for any reason you lose your journal, just type 'quest journal please' at a quest giver and you'll be given a new one.");
		}
		else
			self.say("What's wrong with the journal you have already?");
	}
	else {
		waitpulse 5;
	    questCommandUsage(self,actor);
	}
}