/*************************************************************************************
 *                                                                 		             *
 * questCompleteNoMaster()                                                           *
 * Arguments: quest - The quest from which the dialogue is derived                   *
 *            clanNum(optional) - number of clan governing this quest                *
 * Returns: Nothing                                                		             *
 * Purpose: Attempts to complete a quest that has no master                          *
 *                                                                                   *
 * ~~~ By: Rhollor 4/11/2010                                                         *
 *                                                                                   *
 *************************************************************************************/
JSCharacter.prototype.questCompleteNoMaster = function (quest, clanNum) {
	var qName = quest.questName;
	var guardian = getMobAtRoom(500, 500);
	var here = this.room;
	//Quest is valid and not yet completed.
	//In order for a quest to be completed, either the this.quest must equal 100, or, if there are item requirements,
	//those items must be held by the this. The item requirement outweighs the qval requirement unless
	//quest.requiresQval is set to true.
	var completed = false;
	var vObjects = [];//This will be an array of objects that we need to extract from the this.
	if (quest.items.length > 0) {
		//This quest requires items. Make sure the this has them.
		var allItemsFound = true;
		for (var i = 0; i < quest.items.length; ++i) {
			var tObjArr = this.getItems(quest.items[i][0], true, true, true);//Get the item.
			if (tObjArr.length >= quest.items[i][1]) {//Make sure the this HAS the items
				for (var n = 0; n < quest.items[i][1]; ++n) {
					vObjects.push(tObjArr[n]);
				}
			}
			else {//Otherwise, this has failed to complete the quest. Break out.
				allItemsFound = false;
				break;
			}
		}
		if (allItemsFound == true)
			completed = true;
	}
	// getRoom(13225).echo("All items found: " + allItemsFound);
	/**Check to see if all tasks are complete**/
	if ((quest.items.length && completed == true) || !quest.items.length) {
		if (this.questTaskProgress(qName)[0] == true)
			completed = true;
		else
			completed = false;
	}
	if (!completed) {
		this.send(" ");
		this.send("You have not yet completed this quest. You still have work to do.");
		this.send(" ");
		this.send(this.cyan(constants.CL_OFF) + "1" + this.normal(constants.CL_OFF) + ")  You may want to check your " + this.bold(constants.CL_OFF) + "quest journal" + this.normal(constants.CL_OFF) + " by typing '" + this.cyan(constants.CL_OFF) + "read journal" + this.normal(constants.CL_OFF) + "'.");
		this.send(this.cyan(constants.CL_OFF) + "2" + this.normal(constants.CL_OFF) + ")  Remember to check the tasks in your journal to see if you are\n    ready to complete a quest.");
		return;
	}
	//At this point, we know the this has completed the quest. Be sure to extract all of the items that were required.
	var actor = this;
	for (var i = 0; i < quest.dialogue.length; i++) {
		var line = quest.dialogue[i];
		if (line.pulses == '-2' || line.pulses == -2) {
			eval(line.handle);
		}
	}
	for (var i = 0; i < vObjects.length; ++i) {
		vObjects[i].extract();
	}
	//Reward QP.
	if (clanNum != -1 && clanNum != undefined && quest.unlockRank != 0) {
		guardian.comm("award " + this.name + " " + clanNum + " " + quest.qp + " Quest Completion");
	}
	//Reward FP
	this.addFeatPoints(quest.fp);

	//Reward EXP
	if (quest.exp) {
		this.experience += quest.exp;
		this.send("You receive some experience!");
	}

	//Reward Gold
	if (quest.gold) {
		this.gold += quest.gold;
		this.send("You acquire" + this.numCopperToText(quest.gold, true) + ".");
	}
	//Reward items.	
	if (quest.itemReward && !isEmpty(quest.itemReward)) {
		var slots = quest.itemReward;
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
					actor.send("You acquire " + obj.name + ".");
				}
				break;	// Don't try to load another item in this slot
			}
		}
	}
	//Mark quest as complete.
	var nrComp = this.quest(qName + "-NR_COMPLETED");
	++nrComp;
	if (quest.num == 1 || nrComp >= quest.num)
		this.qval(qName, -1);
	this.qval(qName + "-NR_COMPLETED", nrComp);
	getCharCols(this, constants.CL_OFF)
	this.send(yel + bld + "Journal Updated -- Quest: '" + qName + "' -- " + cyn + "Completed" + nrm);
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
			actor.send(bld + "You have unlocked the following quest" + end + ": " + nrm + grn + questNames.join(", ") + nrm + bld + ".\n" + nrm);
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
	setTimeout(7, afterWait, [quest, this]);
}


