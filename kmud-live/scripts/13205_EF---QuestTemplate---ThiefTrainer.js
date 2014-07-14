var script13205 = function(self, actor, here, args, extra)
{
	//Alder December 2009//
/*******Thief Trainer Quest Master*******/
	//--Finding the Blade--//
	 /**Summary: Player must find and retreive parts of a weapon (blade, handle).
		One part will be in the Sharpened Necessities storage room, the other is 
		on Luhhan's anvil. Bring the pieces back to the thief trainer to receive
		a dagger and some exp as a reward.
		_____________________________________
	   |Requirements: Pick - 55%             |
	   |              Hide - 55%             |
	   |			                         |
	   |Reward(s): dagger, 50k exp           |
	   |_____________________________________|		
		**/
	//--A Lethal Art--//
	 /**Summary: Player must find a cutthroat in the Greater Emond's Field Area, then
		stab the cutthroat dead (the mob will not be touchable in melee). Bring the
		scalp back to receive a shield, experience points, and some coin.
		_____________________________________
	   |Requirements: Short Blades - 85%     |
	   |			  Backstab - 85%         |
	   |			  Hide - 85%             |
	   |                                     |
	   |Reward(s): shield, 50k exp, 5 silver |
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
            actor.send("That is not a valid quest. Type 'quest list' and pick a number from the list.");
                return;
        }
        waitpulse 5;
        if( actor.quest(quests[qNum].questName) == -1 ) {
            actor.send("You've already completed that quest!");
				return;
        }
        if( actor.quest(quests[qNum].questName) != 1 ) {
            actor.send("You have not yet begun that quest.");
				return;
        }
        /** 'Finding the Blade' Dialogue **/
		if( quests[qNum].questName == 'Finding the Blade' ) {
				actor.send( capFirstLetter(self.name) + " looks you up and down with a smug expression on her face." );
				wait 2.5;
                self.comm("tell " + actor.name + " Since you don't look to be a useless fool, I'll let you in on a little secret.");
				wait 3.5;
				self.comm("tell " + actor.name + " Luhhan, as you may know, is the smithy around here. He smiths most of the weapons sold in ole Dowtry's shop.");
				wait 5;
				self.comm("tell " + actor.name + " Now, I don't know about you but I'm not too keen on handin' a copper to Dowtry for a new blade. So I say, why not cut out the middle man and go straight to the source?");
				wait 5.5;
				self.comm("tell " + actor.name + " You bring me the parts to one of those blades Dowtry sells and I'll be so kind to fashion a proper weapon out of 'em.");
				wait 5.5;
				self.comm("tell " + actor.name + " Oh, and by the way, if I were you I would get pretty good at picking locks and hiding from people.");
		}
		/** 'A Lethal Art' Dialogue **/
		else if ( quests[qNum].questName == 'A Lethal Art' ) {
			actor.send( capFirstLetter(self.name) + " looks around carefully, as if there may be someone eavesdropping on her.");
			wait 2.5;
			self.comm("tell " + actor.name + " What I'm about to tell you can never be repeated anywhere around this village, you hear?");
			wait 3.5;
			self.comm("tell " + actor.name + " There's a man that goes by the name of Hirsk. He and I...well...we used to 'know' each other you could say.");
			wait 5;
			self.comm("tell " + actor.name + " At any rate, we aren't on any kind of friendly terms any more, especially since he's been encroaching on my dealings around here.");
			wait 5;
			self.comm("tell " + actor.name + " He's been runnin' his operations from an abandoned farm out east. I want you to take him out for me.");
			wait 4;
			self.comm("tell " + actor.name + " Be careful though. He can wield a dagger like noone you've seen before. Your best bet is to catch him unawares...with your blade in his back.");
			wait 5;
			self.comm("tell " + actor.name + " And don't you pull any tricks and try to get someone else to do the dirty work for you.");
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
        actor.qval(quests[qNum].questName, 1);
        actor.send("You begin the quest '" + actor.bold(constants.CL_COMPLETE) + actor.cyan(constants.CL_COMPLETE) + quests[qNum].questName + actor.normal(constants.CL_COMPLETE) + "'");
        actor.send("To receive information on this quest, type 'quest info " + (qNum+1) + "'");
		//If actor doesn't have a journal, give them one
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

};

