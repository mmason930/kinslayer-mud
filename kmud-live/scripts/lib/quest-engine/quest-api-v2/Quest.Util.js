if (typeof (Quest) === "undefined")
	Quest = {};

/**
 * @namespace Quest.Util
 */
Quest.Util = (function() {
	/**
	 * This namespace is mainly used to define functionality that involves sending output to the player.
	 * Consolidating these functions here means less clutter in the core Quest classes, and easier
	 * maintenance of the cosmetic aspects of the questing system.
   * @lends Quest.Util
	 */
	var Util = {
		/**
		 * Attempts to begin the quest for actor. Messages are only sent if questMaster parameter
		 * is present. This method assumes quest.canAccess(actor) is true.
		 */
		tryBeginQuest: function(actor, quest, questMaster, qNum) {
			if (quest.hasBegun(actor)) {
				questMaster && actor.send("You have already begun that quest!");
				return;
			}

			if (quest.hasCompleted(actor) && !quest.canRepeat(actor)) {
				questMaster && actor.send("You have already completed that quest!");
				return;
			}

			var self = questMaster;
			// Execute beginning dialogue entries
			for (var i = 0, dlg; dlg = quest.dialogue[i++];) {
				if (dlg.isBeginDialogue) {
					eval(dlg.handle);
				}
			}

			if (questMaster) {
				actor.send("You begin the quest '" + bld + cyn + quest.name + nrm + "'");
				actor.send("To receive information on this quest, type '" + cyn + "quest info " + (qNum + 1) + nrm + "' or read your quest journal using the " + cyn + "journal" + nrm + " command.");
			}

			quest.begin(actor, (questMaster ? questMaster.vnum : -1));
			Quest.Util.journalUpdate(actor, quest, 7);
		},

		tryAbandonQuest: function(actor, quest, questMaster) {
			if (!quest.hasBegun(actor)) {
				questMaster && actor.send("You have not yet begun that quest, so there's nothing to abandon!");
				return;
			}
			if (quest.hasCompleted(actor)) {
				questMaster && actor.send("You have already completed that quest!");
				return;
			}

			actor.send("You abandon the quest '" + quest.name + "'.");
			quest.abandon(actor);
		},

		tryCompleteQuest: function(actor, quest, questMaster) {
			if (quest.hasCompleted(actor)) {
				questMaster && actor.send("You have already completed that quest!");
				return;
			}

			getCharCols(actor, constants.CL_OFF);

			// Check required items
			var toExtract = [];
			var allItemsFound = true;
			for (var i = 0; i < quest.items.length; i++) {
				var vnumOrName = quest.items[i][0];
				var amtReq = quest.items[i][1];
				var foundItems = actor.getItems((isNumber(vnumOrName) ? parseInt(vnumOrName) : vnumOrName), true, true, true);
				if (foundItems.length < amtReq) {
					allItemsFound = false;
					break;
				}

				toExtract = toExtract.concat(foundItems.slice(0, amtReq));
			}

			if (!allItemsFound || !quest.readyToComplete(actor)) {
				// Only send failure messages if interacting with quest master
				if (questMaster) {
					actor.send("");
					actor.send("You have not yet completed this quest. You still have work to do.");
					actor.send("");
					actor.send(cyn + "1" + nrm + ")  You may want to check your " + bld + "quest journal" + nrm + " by typing '" + cyn + "journal" + nrm + "'.");
					actor.send(cyn + "2" + nrm + ")  Remember to check the tasks in your journal to see if you are\n    ready to complete a quest.");
				}
				return;
			}

			//===================
			// Requirements met
			//===================

			// Extract the required items
			toExtract.forEach(function(item) {
				item.extract();
			});

			// Clear quest's items from actor's quest inventory
			actor.clearQuestItems(quest);

			var self = questMaster;
			// Evaluate completion dialogue
			for (var i in quest.dialogue) {
				var dlg = quest.dialogue[i];
				if (dlg.isCompleteDialogue)
					eval(dlg.handle);
			}

			// Award quest points
			if (quest.qp > 0) {
				var clan = (questMaster ? questMaster.findClans(true)[0] : actor.findClans(true)[0]);
				var guardian = getMobAtRoom(500, 500);
				guardian.comm("award " + actor.name + " " + clan + " " + quest.qp + " Quest Completion");
			}

			// Award FP
			actor.addFeatPoints(quest.fp);

			// Award EXP
			if (quest.exp) {
				actor.experience += quest.exp;
				actor.send("You receive some experience!");
			}

			// Award gold
			if (quest.gold) {
				actor.gold += quest.gold;
				actor.send("You acquire" + actor.numCopperToText(quest.gold, true) + ".");
			}

			// Award items
			if (quest.itemRewards && !isEmpty(quest.itemRewards)) {
				var slots = quest.itemRewards;
				for (var id in slots) {
					var slot = slots[id];
					for (var i = 0; i < slot.length; i++) {
						var item = slot[i];

						if (random(1, 100) > item.loadPercent)
							continue;

						// Item load successful
						for (var j = 0; j < item.count; j++) {
							var obj = actor.loadObj(item.vnum);
							if (item.isRetooled) {
								obj.setRetoolSDesc(item.retoolShortDesc);
								obj.setRetoolDesc(item.retoolRoomDesc);
								obj.setRetoolName(item.retoolNameList);
								obj.setRetoolExDesc(item.retoolExtraDesc);
							}
							if (questMaster) {
								actor.send(capFirstLetter(questMaster.name) + " gives you " + obj.name + ".");
								actor.room.echoaround(actor, capFirstLetter(questMaster.name) + " gives " + actor.name + " " + obj.name + ".");
								continue;
							}
							actor.send("You acquire " + obj.name + ".");
						}
						break;	// Don't try to load another item in this slot
					}
				}
			}

			// Mark quest as complete
			var remainingReps = quest.complete(actor);
			Quest.Util.journalUpdate(actor, quest);
			if (remainingReps) {
				var countStr = (remainingReps > 0 ? intToText(remainingReps) : "many");
				actor.send(bld + "You may complete the following quest " + cyn + countStr + nrm + bld + " more time(s): '" + nrm + grn + quest.name + nrm + bld + "'." + nrm);
				if (!quest.canAccess(actor)) {
					actor.send(bld + red + "But you are ineligible to restart it at this moment. Check back with the quest master later." + nrm);
				}
				else {
					// Automatically restart the repeatable quest if actor can still access it
					quest.begin(actor);
					Quest.Util.journalUpdate(actor, quest);
				}
			}

			// Notify actor of any unlocked quests
			Quest.Util.notifyQuestsUnlocked(actor, quest);
		},

		listQuests: function(actor, quests, questMaster) {
			//List all the available quests.
			getCharCols(actor, constants.CL_OFF);
			actor.send(red + " Available Quests" + nrm);
			actor.send(bld + strPadding("", "-", 19, "right") + nrm);
			for (var i = 0; i < quests.length; ++i) {
				var quest = quests[i];
				var sFinal = cyn + (i + 1) + nrm + ") " + grn + quest.name;
				sFinal = strPadding(sFinal, " ", 40, "right");
				actor.send(sFinal + nrm + "  (" + quest.statusString(actor, true) + ")");
			}
		},

		/**
		 * Notifies actor that a task has been completed. Also sends messages regarding newly unlocked tasks.
		 */
		notifyTaskCompleted: function(actor, task) {
			actor.send(grn + bld + "\nTask Completed: '" + task.name + "'" + nrm);
			actor.send("");
			actor.send(task.dependentTasks.map(function(task) {
				return (bld + "New task for " + nrm + grn + task.name + nrm + bld + ": " + nrm + cyn + task.name + nrm);
			}).join("\n") + "\n");
		},

		/**
		 * Sends messages to actor regarding quests which are unlocked by the given quest.
		 * Actor will also be informed of which quest master(s) has new quests available.
		 * Unlocked quests with no quest master will begin automatically.
		 * @param {number} wait Number of pulses to wait before sending messages.
		 */
		notifyQuestsUnlocked: function(actor, quest) {
			var dependentQuests = quest.getDependentQuests();
			var unlockedQuests = dependentQuests.filter(function(q) {
				return q.canAccess(actor);
			});

			if (unlockedQuests.length > 0) {
				var names = [], masterVnums = {};
				for (var i = 0, q; q = unlockedQuests[i++];) {
					names.push(q.name);
					for (var k = 0, vnum; vnum = q.masterVnums[k++];) {
						masterVnums[vnum] = true;
					}
					// Go ahead and begin this quest since the actor has no way to manually start it
					if (q.isMasterless) {
						Quest.Util.tryBeginQuest(actor, q);
					}
				}

				actor.send(bld + "You have unlocked the following quest" + (names.length > 1 ? "s" : "") + ": " +
							  nrm + grn + names.join(nrm + bld + ", " + nrm + grn) + nrm + bld + ".\n" + nrm);

				for (var vnum in masterVnums) {
					var mobName = getMobName(vnum);
					actor.send(bld + cyn + capFirstLetter(mobName) + nrm + bld + " has new quests for you." + nrm);
				}
			}
		},

		/**
		 * Notifies actor of an update to the status of quest.
		 */
		journalUpdate: function(actor, quest, timeout) {
			function notify() {
				getCharCols(actor, constants.CL_OFF);
				actor.send(yel + bld + "Journal Updated -- Quest: " + quest.name + " -- " + cyn + quest.statusString(actor) + nrm);
			};
			if (isNumber(timeout) && timeout > 0) {
				setTimeout(timeout, notify);
				return;
			}
			notify();
		},

		/**
		 * @returns {string} - A string containing the text for the quest's journal entry.
		 */
		getJournalEntry: function(actor, quest) {
			getCharCols(actor, constants.CL_OFF);
			var bright = nrm + bld;
			var mobName = getMobName(quest.giverVnum(actor));
			var qName = quest.name;
			var summary = quest.summary;
			try {
				summary = eval(summary);
			} catch (e) {
				// must be plaintext
			}
			var skillsBuff = "\n\t";	// Align skills on separate lines
			var taskBuff = "\n";			// Align tasks on separate lines
			/**ENTRY SKILLS**/
			var skills = (quest.skills.length ? cyn + "\n\nSkills: " + nrm + quest.skills.join(skillsBuff) : "");

			/**ENTRY TASKS**/
			var tasks = "";
			if (quest.tasks.length) {
				var taskLines = [];
				for (var i = 0, task; task = quest.tasks[i++];) {
					// Hide locked tasks
					if (!task.hasUnlocked(actor))
						continue;

					var status = task.getStatus(actor);
					var CURR = status.progress;
					var REQ = task.completedValue;

					if (quest.hasCompleted(actor)) {
						// Since quest is completed, always show tasks as complete
						CURR = REQ;
					}

					// Brackets and / are colored depending on quest status
					var clr = bld + (CURR < REQ ? grn : red);
					taskLines.push("\t" + strPadding(task.name, " ", 60, "right") + clr + "[" + bright + CURR + nrm + clr + "/" + bright + REQ + nrm + clr + "]" + nrm)
				}
				tasks = cyn + "\n\nTasks: " + nrm + taskLines.join(taskBuff);
			}

			/**ENTRY TITLE**/
			var givenBy = (!mobName ? '' : " -- Given by " + mobName);
			var title = ") " + grn + qName + nrm + "  (" + quest.statusString(actor, true) + ")" + yel + givenBy + nrm;

			/**ENTRY SUMMARY**/
			var summary = "\n" + strColFormat(summary, 55);

			/**ENTRY REQUIREMENTS**/
			var requirements = skills + tasks;

			/**COMBINE: TITLE + SUMMARY + REQUIREMENTS**/
			return title + summary + requirements;
		},

		getString: function(actor, quest) {
			getCharCols(actor);
			return yel + quest.name + nrm + " [" + cyn + quest.id + nrm + "]";
		}
	};

	return Util;
})();