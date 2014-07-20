function questComplete(self, actor, quests, arg, clanVnum) {
	//Attempt to complete a quest.
	if (!arg) {
		actor.send("Yes, but WHICH quest? Specify the quest number.");
		return;
	}
	var qNum = arg - 1;
	if (qNum >= quests.length || qNum < 0 || isNaN(qNum)) {
		actor.send("That is not a valid quest. Type '" + actor.cyan(constants.CL_OFF) + "quest list" + actor.normal(constants.CL_OFF) + "' and pick a number from the list.");
		return;
	}
	var quest = quests[qNum];
	if (actor.quest(quest.questName) == -1) {
		actor.send("You have already completed that quest.");
		return;
	}
	if (actor.quest(quest.questName) == 0) {
		actor.send("You have not yet begun that quest.");
		return;
	}
	var here = actor.room;
	//Quest is valid and not yet completed.
	//In order for a quest to be completed, either the actor.quest must equal 100, or, if there are item requirements,
	//those items must be held by the actor. The item requirement outweighs the qval requirement unless
	//quest.requiresQval is set to true.
	var completed = false;
	var vObjects = [];//This will be an array of objects that we need to extract from the actor.
	if (quest.items.length) {
		//This quest requires items. Make sure the actor has them.
		var allItemsFound = true;
		for (var i = 0; i < quest.items.length; i++) {
			var item = quest.items[i][0];
			var amtReq = quest.items[i][1];
			// getRoom(98).echo(item+" :: "+amtReq);
			var tObjArr = actor.getItems((isNumber(item) ? parseInt(item) : item), true, true, true);//Get the item.
			// getRoom(98).echo(tObjArr);
			if (tObjArr.length >= amtReq) {//Make sure the actor HAS the items
				for (var n = 0; n < amtReq; ++n) {
					// getRoom(98).echo("Deleting "+tObjArr[n].name);
					vObjects.push(tObjArr[n]);
				}
			}
			else {//Otherwise, actor has failed to complete the quest. Break out.
				allItemsFound = false;
				// getRoom(98).echo("All items not found");
				break;
			}
		}
		if (allItemsFound == true)
			completed = true;
	}
	/**Check to see if all tasks are complete**/
	if ((quest.items.length > 0 && completed == true) || !quest.items.length) {
		if (actor.questTaskProgress(quest.questName)[0] == true)
			completed = true;
		else
			completed = false;
	}
	if (completed == false) {
		actor.send(" ");
		actor.send("You have not yet completed this quest. You still have work to do.");
		actor.send(" ");
		actor.send(actor.cyan(constants.CL_OFF) + "1" + actor.normal(constants.CL_OFF) + ")  You may want to check your " + actor.bold(constants.CL_OFF) + "quest journal" + actor.normal(constants.CL_OFF) + " by typing '" + actor.cyan(constants.CL_OFF) + "journal" + actor.normal(constants.CL_OFF) + "'.");
		actor.send(actor.cyan(constants.CL_OFF) + "2" + actor.normal(constants.CL_OFF) + ")  Remember to check the tasks in your journal to see if you are\n    ready to complete a quest.");
		return;
	}
	//At this point, we know the actor has completed the quest. Be sure to extract all of the items that were required.
	for (var _autoKey in quest.dialogue) {
		var line = quest.dialogue[_autoKey];
		if (line.pulses == '-2' || line.pulses == -2) {
			eval(line.handle);
		}
	}
	if (getObjectClass(quest.dialogue[quest.dialogue.length - 1]) == "String") {
		var special_dialogue = quest.dialogue[quest.dialogue.length - 1].split("~");
		self.comm("tell " + actor.name + " " + special_dialogue[1]);
	}

	for (var i = 0; i < vObjects.length; ++i) {
		vObjects[i].extract();
	}
	//Remove any qinventory items
	for (var _autoKey in actor._questInv) {
		var item = actor._questInv[_autoKey];
		if (item.questName == quest.questName) {
			actor.remQuestItem(item.itemName, item.itemAmount, item.questName);
		}
	}
	//Reward QP.
	//actor.room.echo(quest.qp);
	if (clanVnum != -1 && quest.qp && quest.unlockRank != 0) {
		//actor.room.echo("award " + actor.name + " " + clanVnum + " " + quest.qp);
		self.comm("award " + actor.name + " " + clanVnum + " " + quest.qp + " Quest Completion");
	}
	//Reward FP
	actor.addFeatPoints(quest.fp);
	//Reward EXP
	if (quest.exp) {
		actor.experience += parseInt(quest.exp);
		actor.send("You receive some experience!");
	}
	//Reward Gold
	if (quest.gold) {
		actor.gold += quest.gold;
		actor.send(capFirstLetter(self.name) + " gives you" + actor.numCopperToText(quest.gold, true) + ".");
	}
	//Reward items.	
	if (quest.itemReward && !isEmpty(quest.itemReward)) {
		var slotArray = quest.itemReward;
		for (var id in slots) {
			var slot = slots[id];
			for (var i = 0; i < slot.length; i++) {
				var item = slot[i];
				if (random(1, 100) > item.loadPercent) {
					continue;
				}
				for (var j = 0; j < item.count; j++) {
					var obj = actor.loadObj(item.vnum);
					if (item.isRetooled) {
						obj.setRetoolSDesc(item.retoolShortDesc);
						obj.setRetoolDesc(item.retoolRoomDesc);
						obj.setRetoolName(item.retoolNameList);
						obj.setRetoolExDesc(item.retoolExtraDesc);
					}
					actor.send(capFirstLetter(self.name) + " gives you " + obj.name + ".");
					here.echoaround(actor, capFirstLetter(self.name) + " gives " + actor.name + " " + obj.name + ".");
				}
				break;	// Don't try to load another item in this slot
			}
		}
	}
	//Mark quest as complete.
	var nrComp = actor.quest(quest.questName + "-NR_COMPLETED") + 1;

	if (quest.num > 0 && (quest.num == 1 || nrComp >= quest.num))
		actor.qval(quest.questName, -1);

	actor.qval(quest.questName + "-NR_COMPLETED", nrComp);
	actor.updateJournalTask(quest.questName, null);//Displays a journal update to player
	if (nrComp < quest.num || quest.num < 1) {
		// actor.qval(quest.questName, 0);
		for (var _autoKey in quest.taskArray) {
			var task = quest.taskArray[_autoKey];
			if (task[1] == null || task[1] == '') {
				actor.qval(quest.questName + "_" + task[0], 0);
			}
		}
		actor.send(bld + "You may complete the following quest " + cyn + (quest.num > 0 ? intToText((quest.num - nrComp)) : "many") + nrm + bld + " more time(s): '" + nrm + grn + quest.questName + nrm + bld + "'." + nrm);
	}
	/**********AFTER THE WAIT CHECK TO UNLOCK*************/
	function afterWait(vArgs) {
		var quest = vArgs[0];
		var actor = vArgs[1];
		var unlockedQuests = actor.getUnlockedQuests(quest.questName);
		if (unlockedQuests && unlockedQuests.length > 0) {
			var end = "";
			if (unlockedQuests.length > 1) {
				end = "s";
			}
			var masterVnums = [];
			var questNames = [];
			for (var _autoKey in unlockedQuests) {
				var quest = unlockedQuests[_autoKey];
				if (quest.ownerVnum.length > 0) {
					for (var _autoKey in actor.isQuestAvailable(quest.questName)[1]) {
						var num = actor.isQuestAvailable(quest.questName)[1][_autoKey];
						if (arrContains(masterVnums, num) == false) {
							masterVnums.push(num);
						}
					}
				}
				questNames.push(quest.questName);
			}
			actor.send(bld + "You have unlocked the following quest" + end + ": " + nrm + grn + questNames.join(nrm + bld + ", " + nrm + grn) + nrm + bld + ".\n" + nrm);
			for (var _autoKey in unlockedQuests) {
				var quest = unlockedQuests[_autoKey];
				if (quest.ownerVnum.length == 0) {
					actor.journalEdit("ADD", quest.questName);
				}
			}
			for (var _autoKey in masterVnums) {
				var vnum = masterVnums[_autoKey];
				var count = 0;
				for (var _autoKey in unlockedQuests) {
					var quest = unlockedQuests[_autoKey];
					if (arrContains(quest.ownerVnum, vnum))
						++count;
				}
				var mobName = getMobName(vnum);
				if (count == 1)
					actor.send(bld + cyn + capFirstLetter(mobName) + nrm + bld + " has a new quest for you." + nrm);
				else if (count > 1)
					actor.send(bld + cyn + capFirstLetter(mobName) + nrm + bld + " has new quests for you." + nrm);
			}
		}
	}
	setTimeout(7, afterWait, [quest, actor]);
}

