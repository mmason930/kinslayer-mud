//Line count: 2311 w/newline
delete bootQueditOLC;
function bootQueditOLC() {
	var oConfig = new Object();
	var mode;
	oConfig.type = "quedit";
	oConfig.modes = [];
	/*** Main Menu ***/
	mode = new Object();
	mode.mode = "MODE_MAIN";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == 'Q') {
			actor.cleanupOLC();
		}
		else if (fLetter == 1) {
			actor.getOLC().listVersion = 0;
			actor.getOLC().switchToMode("MODE_QUESTS_BY_NAME");
			return;
		}
		else if (fLetter == 2) {
			actor.getOLC().switchToMode("MODE_QUESTS_BY_OWNER");
			return;
		}
		else if (fLetter == 3) {
			actor.getOLC().switchToMode("MODE_QUESTS_BY_TAG");
			return;
		}
		else if (fLetter == 4) {
			actor.getOLC().switchToMode("MODE_RECENT_QUESTS");
			return;
		}
		else if (fLetter == "C") {
			actor.getOLC().switchToMode("MODE_QUESTS_BY_CREATOR");
			return;
		}
		else if (fLetter == "D") {
			actor.qval("QUEDIT_DISP_TOG", !actor.quest("QUEDIT_DISP_TOG"));
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Valid commands are, 1, 2, 3, N, Q.\nTry again: ");
		}
	}
	mode.display = function(actor) {
		actor.getOLC().quest = new Quest();
		actor.getOLC().questOwner = undefined;
		actor.getOLC().name = undefined;
		/**INITIALIZE DATA FOR NEW QUEST**/
		actor.getOLC().idLoaded = undefined;
		actor.getOLC().databaseID = undefined;
		actor.getOLC().ownerVnums = [];
		actor.getOLC().listVersion = undefined;
		actor.getOLC().completionLimit = 1;
		actor.getOLC().summary = "<none>";
		actor.getOLC().exp = 0;
		actor.getOLC().qp = 0;
		actor.getOLC().fp = 0;
		actor.getOLC().gold = 0;
		actor.getOLC().items = [];
		actor.getOLC().itemRewards = {};
		actor.getOLC().dialogue = [];
		actor.getOLC().skills = [];
		actor.getOLC().tasks = [];
		actor.getOLC().skills = [];
		actor.getOLC().priorQuests = [];
		actor.getOLC().tags = [];
		actor.getOLC().unlockRank = 0;
		actor.getOLC().showGivenBy = true;
		actor.getOLC().loaded = false;
		actor.getOLC().setDelete = false;
		actor.getOLC().allowedRaces = [];
		actor.getOLC().extraChecks = [];
		actor.getOLC().editors = [];
		actor.send("Quest Editor\n" + strPadding("", "_", 70, "right") + "\n");
		actor.send(grn + "1" + nrm + ") Quests By Name");
		actor.send(grn + "2" + nrm + ") Quests By Owner");
		actor.send(grn + "3" + nrm + ") Quests By Tag");
		actor.send(grn + "4" + nrm + ") Recently Viewed Quests");
		actor.send("");
		actor.send(grn + "C" + nrm + ") Quests By Creator");
		var qval = actor.quest("QUEDIT_DISP_TOG");
		var dOpt = (!qval ? "Display Only Your Quests" : "Display All Quests");
		actor.send(grn + "D" + nrm + ") " + dOpt);
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Quests By Creator ***/
	mode = new Object();
	mode.mode = "MODE_QUESTS_BY_CREATOR";
	mode.parser = function(actor, fLetter, vArgs) {
		var creators = actor.getOLC().vCreators;
		if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		if (!isNumber(vArgs[0])) {
			actor.send("Input must be numeric, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		if (vArgs[0] < 1 || vArgs[0] > creators.length) {
			actor.send("Your input is outside the specified range.\r\nTry again: ");
			return;
		}
		actor.getOLC().questOwner = creators[vArgs[0] - 1];
		actor.getOLC().listVersion = 3;
		actor.getOLC().switchToMode("MODE_QUESTS_BY_NAME");
		return;
	}
	mode.display = function(actor) {
		var creators = [];
		for (var _autoKey in Quest.allQuests) {
			var quest = Quest.allQuests[_autoKey];
			var creator = quest.editors[0];
			if (!arrContains(creators, creator)) {
				creators.push(creator);
			}
		}
		creators.sort();
		actor.getOLC().vCreators = creators;
		getCharCols(actor);
		actor.send("Quests By Creator\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < creators.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + yel + creators[i] + nrm);
		}
		actor.send("");
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Recently Viewed Quests ***/
	mode = new Object();
	mode.mode = "MODE_RECENT_QUESTS";
	mode.parser = function(actor, fLetter, vArgs) {
		var questIDArray = actor.getRecentlyViewedQuests();
		var quests = [];
		for (var _autoKey in questIDArray) {
			var id = questIDArray[_autoKey];
			for (var _autoKey in Quest.allQuests) {
				var quest = Quest.allQuests[_autoKey];
				if (quest.databaseID == id) {
					quests.push(quest);
					break;
				}
			}
		}
		if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		if (!isNumber(vArgs[0])) {
			actor.send("Input must be numeric, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		if (vArgs[0] < 1 || vArgs[0] > quests.length) {
			actor.send("Your input is outside the specified range.\r\nTry again: ");
			return;
		}
		actor.getOLC().idLoaded = quests[vArgs[0] - 1].databaseID;
		actor.getOLC().switchToMode("MODE_QUEST_DATA");
		return;
	}
	mode.display = function(actor) {
		//Establish array of quests
		var questIDArray = actor.getRecentlyViewedQuests();
		var recentQuests = [];
		for (var _autoKey in questIDArray) {
			var id = questIDArray[_autoKey];
			for (var _autoKey in Quest.allQuests) {
				var quest = Quest.allQuests[_autoKey];
				if (quest.databaseID == id) {
					recentQuests.push(quest);
					break;
				}
			}
		}
		getCharCols(actor);
		actor.send("Recently Viewed Quests\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < recentQuests.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + Quest.Util.getString(actor, recentQuests[i]));
		}
		actor.send("");
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Quests By Owner ***/
	mode = new Object();
	mode.mode = "MODE_QUESTS_BY_OWNER";
	mode.parser = function(actor, fLetter, vArgs) {
		var masters = getAllQuestMasters(actor);
		var masterCount = masters.length;
		if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		if (!isNumber(vArgs[0])) {
			actor.send("Input must be numeric, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		if (vArgs[0] < 1 || vArgs[0] > masterCount) {
			actor.send("Your input is outside the specified range.\r\nTry again: ");
			return;
		}
		actor.getOLC().questOwner = masters[vArgs[0] - 1];
		actor.getOLC().listVersion = 1;
		actor.getOLC().switchToMode("MODE_QUESTS_BY_NAME");
		return;
	}
	mode.display = function(actor) {
		//Establish array of names
		var masters = getAllQuestMasters(actor);
		var masterCount = masters.length;
		getCharCols(actor);
		actor.send("Quests By Owner\n" + strPadding("", "_", 70, "right") + "\n");
		if (masterCount < 10)
			padVal = 2;
		else if (masterCount < 100)
			padVal = 3;
		else
			padVal = 4;
		for (var i = 0; i < masters.length; i++) {
			var name = masters[i];
			actor.send(strPadding(grn + (i + 1) + nrm, " ", padVal, "left") + ") " + yel + name + nrm);
		}
		actor.send("");
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Quests By Tag ***/
	mode = new Object();
	mode.mode = "MODE_QUESTS_BY_TAG";
	mode.parser = function(actor, fLetter, vArgs) {
		var tags = getAllQuestTags(actor);
		var tagCount = tags.length;
		if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		if (!isNumber(vArgs[0])) {
			actor.send("Input must be numeric, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		if (vArgs[0] < 1 || vArgs[0] > tagCount) {
			actor.send("Your input is outside the specified range.\r\nTry again: ");
			return;
		}
		actor.getOLC().questOwner = tags[vArgs[0] - 1];
		actor.getOLC().listVersion = 2;
		actor.getOLC().switchToMode("MODE_QUESTS_BY_NAME");
		return;
	}
	mode.display = function(actor) {
		var quests = [];
		for (var _autoKey in Quest.allQuests) {
			var quest = Quest.allQuests[_autoKey];
			if (actor.quest("QUEDIT_DISP_TOG") != 0 && !arrContains(quest.editors, actor.name)) {
				continue;
			}
			else {
				quests.push(quest);
			}
		}
		//Establish array of names
		var tags = getAllQuestTags(actor);
		var tagCount = tags.length;
		getCharCols(actor);
		actor.send("Quests By Tag\n" + strPadding("", "_", 70, "right") + "\n");
		if (tagCount < 10)
			padVal = 2;
		else if (tagCount < 100)
			padVal = 3;
		else
			padVal = 4;
		for (var i = 0; i < tags.length; i++) {
			var name = tags[i];
			actor.send(strPadding(grn + (i + 1) + nrm, " ", padVal, "left") + ") " + yel + name + nrm);
		}
		actor.send("");
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Quests By Name ***/
	mode = new Object();
	mode.mode = "MODE_QUESTS_BY_NAME";
	function findQuests(actor) {
		var vList = actor.getOLC().listVersion;
		getCharCols(actor);
		var master = actor.getOLC().questOwner;
		if (vList == 0) {
			var title = strPadding("Quests By Name\n", "_", 70, "right") + "\n";
			var quests = [];
			for (var _autoKey in Quest.allQuests) {
				var quest = Quest.allQuests[_autoKey];
				if (actor.quest("QUEDIT_DISP_TOG") != 0 && !arrContains(quest.editors, actor.name)) {
					continue;
				}
				else {
					quests.push(quest);
				}
			}
		}
		else {
			var title = undefined;
			var quests = [];
			if (vList == 2) {
				if (title == undefined)
					title = "Quests With Tag: " + bld + master + nrm + "\n" + strPadding("", "_", 70, "right") + "\n";
				quests = getQuestsByTag(master, [actor]);
			}
			else if (vList == 1) {
				if (title == undefined)
					title = "Quests Belonging To: " + bld + master + nrm + "\n" + strPadding("", "_", 70, "right") + "\n";
				quests = getQuestsByMasterName(master);
			}
			else if (vList == 3) {
				if (title == undefined) {
					title = "Quests Created By: " + bld + master + nrm + "\n" + strPadding("", "_", 70, "right") + "\n";
				}
				var len = Quest.allQuests.length;
				for (var i = 0; i < len; i++) {
					var quest = Quest.allQuests[i];
					if (quest.editors[0] == master) {
						quests.push(quest);
					}
				}
			}
		}
		return ([quests, title]);
	}
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == 'Q') {
			if (actor.getOLC().listVersion == 0) {
				actor.getOLC().switchToMode("MODE_MAIN");
			}
			else if (actor.getOLC().listVersion == 1) {
				actor.getOLC().switchToMode("MODE_QUESTS_BY_OWNER");
			}
			else if (actor.getOLC().listVersion == 2) {
				actor.getOLC().switchToMode("MODE_QUESTS_BY_TAG");
			}
			else if (actor.getOLC().listVersion == 3) {
				actor.getOLC().switchToMode("MODE_QUESTS_BY_CREATOR");
			}
			return;
		}
		else if (fLetter == "N") {
			actor.getOLC().editors.push(actor.name);
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		if (!isNumber(vArgs[0])) {
			actor.send("Input must be numeric, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		var quests = findQuests(actor)[0];
		var nr = parseInt(vArgs[0]) - 1;
		if (nr < 0 || nr >= quests.length) {
			actor.send("That quest doesn't exist.\r\nTry again: ");
			return;
		}
		actor.getOLC().idLoaded = quests[vArgs[0] - 1].databaseID;
		actor.getOLC().switchToMode("MODE_QUEST_DATA");
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(findQuests(actor)[1]);//Title
		var quests = findQuests(actor)[0];
		var questCount = quests.length;
		if (questCount < 10)
			padVal = 2;
		else if (questCount < 100)
			padVal = 3;
		else
			padVal = 4;
		var sBuffer = "";
		for (var i = 0; i < quests.length; ++i) {
			sBuffer += grn + strPadding((i + 1) + "", " ", padVal, "left") + nrm + ") " + Quest.Util.getString(actor, quests[i]) + "\r\n";
		}
		actor.send(sBuffer);
		actor.send(grn + "N" + nrm + ") Create New Quest");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/**** Quest Confirm Save ****/
	mode = new Object();
	mode.mode = "MODE_CONFIRM_SAVE";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		if (OLC.action == 5) {
			if (fLetter == "Y") {
				OLC.action = 10;
				OLC.switchToMode("MODE_CONFIRM_SAVE");
			}
			else if (fLetter == "N") {
				OLC.switchToMode("MODE_QUEST_DATA");
			}
			else {
				actor.send("Invalid input. Try again: ");
			}
			return;
		}
		if (fLetter == "Y") {
			if (!OLC.name) {
				OLC.switchToMode("MODE_QUEST_DATA");
				actor.send(bld + "\nYou must name the quest before trying to save." + nrm);
				return;
			}

			var existingQuest = Quest.getByName(OLC.name);
			if (existingQuest && OLC.databaseID != existingQuest.databaseID) {
				OLC.switchToMode("MODE_QUEST_DATA");
				actor.send(bld + "\nA quest with this name already exists. You must change the name to continue." + nrm);
				return;
			}

			var loadedIndex;
			var quest = OLC.quest;

			if (OLC.setDelete == false) {
				var needsInsertion = true;

				quest.name = OLC.name;
				quest.databaseID = OLC.databaseID;
				quest.ownerVnums = OLC.ownerVnums;
				quest.qp = OLC.qp;
				quest.fp = OLC.fp;
				quest.exp = OLC.exp;
				quest.gold = OLC.gold;
				quest.maxCompletions = OLC.completionLimit;
				quest.summary = OLC.summary;
				quest.skills = OLC.skills;
				quest.dialogue = OLC.dialogue;
				quest.tasks = OLC.tasks;
				quest.items = OLC.items;
				quest.itemRewards = OLC.itemRewards;
				quest.priorQuests = OLC.priorQuests;
				quest.tags = OLC.tags;
				quest.showGivenBy = OLC.showGivenBy;
				quest.unlockRank = OLC.unlockRank;
				quest.allowedRaces = OLC.allowedRaces;
				quest.extraChecks = OLC.extraChecks;
				quest.editors = OLC.editors;

				if (!_.contains(quest.editors, actor.name))
					quest.editors.push(actor.name);

				Quest.addQuest(quest, true);
			}
			else if (OLC.setDelete == true) {
				Quest.deleteQuest(quest, true);
			}
			mudLog(3, 102, actor.name + " edited quest " + quest.databaseID + " (" + quest.name + ")");
		}
		else if (fLetter == "N") {
			actor.send(bld + red + "Changes discarded...\n" + nrm);
			//mudLog(3,102,actor.name+" has finished editing quest: " + OLC.name + " (" + OLC.databaseID + ")");
		}
		else {
			actor.send("Invalid input. Try again: ");
			return;
		}
		var recentQuests = actor.getRecentlyViewedQuests();
		var thisQuest = OLC.databaseID;
		if (!arrContains(recentQuests, thisQuest)) {
			if (recentQuests.length < 5) {
				recentQuests.unshift(thisQuest);
			}
			else if (recentQuests.length == 5) {
				recentQuests.pop();
				recentQuests.unshift(thisQuest);
			}
			setSval(getRoom(98), -9811, actor.name + " RVQ", recentQuests);
		}
		OLC.switchToMode("MODE_MAIN");
	}
	mode.display = function(actor) {
		getCharCols(actor, constants.CL_OFF);
		if (actor.getOLC().setDelete == true && actor.getOLC().action != 5 && actor.getOLC().action != 10) {
			actor.send(bld + "This quest is set to delete, continue? (Y/N)" + nrm);
			actor.getOLC().action = 5;
			return;
		}
		actor.send("Would you like to save this quest (Y/N) : ");
	}
	oConfig.modes.push(mode);
	/*** Quest Data ***/
	mode = new Object();
	mode.mode = "MODE_QUEST_DATA";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_CONFIRM_SAVE");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().switchToMode("MODE_QNAME");
			return;
		}
		else if (fLetter == "B") {
			actor.getOLC().switchToMode("MODE_OWNER_VNUM");
			return;
		}
		else if (fLetter == "C") {
			actor.getOLC().switchToMode("MODE_COMPLETION_LIMIT");
			return;
		}
		else if (fLetter == "D") {
			actor.getOLC().switchToMode("MODE_SUMMARY");
			return;
		}
		else if (fLetter == "E") {
			actor.getOLC().switchToMode("MODE_EXP");
			return;
		}
		else if (fLetter == "F") {
			actor.getOLC().switchToMode("MODE_QP");
			return;
		}
		else if (fLetter == "G") {
			actor.getOLC().switchToMode("MODE_GOLD");
			return;
		}
		else if (fLetter == "H") {
			actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS");
			return;
		}
		else if (fLetter == "I") {
			actor.getOLC().switchToMode("MODE_REWARDED_ITEMS");
			return;
		}
		else if (fLetter == "J") {
			actor.getOLC().switchToMode("MODE_DIALOGUE");
			return;
		}
		else if (fLetter == "K") {
			actor.getOLC().switchToMode("MODE_SKILLS");
			return;
		}
		else if (fLetter == "L") {
			actor.getOLC().switchToMode("MODE_TASKS");
			return;
		}
		else if (fLetter == "M") {
			actor.getOLC().switchToMode("MODE_PRIORQUESTS");
			return;
		}
		else if (fLetter == "S") {
			actor.getOLC().showGivenBy = !actor.getOLC().showGivenBy;
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "T") {
			actor.getOLC().action = 0;
			actor.getOLC().switchToMode("MODE_TAGS");
			return;
		}
		else if (fLetter == "U") {
			actor.getOLC().switchToMode("MODE_RANK");
			return;
		}
		else if (fLetter == "V") {
			actor.getOLC().switchToMode("MODE_EDITORS");
			return;
		}
		else if (fLetter == "O") {
			actor.getOLC().switchToMode("MODE_OPEN");
			return;
		}
		else if (fLetter == "X") {
			actor.getOLC().switchToMode("MODE_EXTRAS");
			return;
		}
		else if (fLetter == "Y") {
			actor.getOLC().switchToMode("MODE_FP");
			return;
		}
		else if (fLetter == "*") {
			actor.getOLC().setDelete = !actor.getOLC().setDelete;
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Valid inputs are green on the left.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		var OLC = actor.getOLC();

		getCharCols(actor);
		OLC.action = undefined;
		OLC.slot = undefined;
		OLC.tier = undefined;
		var quest = Quest.getById(OLC.idLoaded);
		//Populate OLC fields with quest object data if loading existing quest
		if (quest && OLC.loaded == false) {
			OLC.loaded = true;
			OLC.quest = quest;
			OLC.databaseID = quest.databaseID;
			OLC.name = quest.name;
			OLC.showGivenBy = quest.showGivenBy;
			OLC.ownerVnums = [];
			if (getObjectClass(quest.ownerVnums) == "Array") {
				for (var _autoKey in quest.ownerVnums) {
					var vnum = quest.ownerVnums[_autoKey];
					OLC.ownerVnums.push(vnum);
				}
			}
			else {
				OLC.ownerVnums[0] = quest.ownerVnums;
			}
			if (!OLC.ownerVnums.length) {
				OLC.showGivenBy = false;
			}
			OLC.completionLimit = quest.maxCompletions;
			OLC.summary = quest.summary;
			OLC.exp = quest.exp;
			OLC.qp = quest.qp;
			OLC.fp = quest.fp;
			OLC.gold = quest.gold;
			OLC.unlockRank = quest.unlockRank;
			OLC.extraChecks = clone(quest.extraChecks);
			OLC.allowedRaces = clone(quest.allowedRaces);
			OLC.tags = clone(quest.tags);
			OLC.items = clone(quest.items);
			OLC.itemRewards = clone(quest.itemRewards);
			OLC.dialogue = clone(quest.dialogue);
			OLC.skills = clone(quest.skills);

			for (var i = 0, task; task = quest.tasks[i++];) {
				OLC.tasks.push(new Quest.Task(task));
			}
			OLC.priorQuests = clone(quest.priorQuests);
			OLC.editors = clone(quest.editors);
		}
		actor.send("Quest Data Input\n" + strPadding("", "_", 70, "right") + "\n");
		actor.send("-- ID: [" + cyn + OLC.databaseID + nrm + "]");
		actor.send("");
		actor.send(strPadding(grn + "A" + nrm + ") ", " ", 3, "left") + strPadding("Name", " ", 18, "right") + ": " + cyn + (OLC.name ? OLC.name : "<none>") + nrm);
		var parts = [];
		for (var i = 0, num; num = OLC.ownerVnums[i++];) {
			parts.push(getMobName(num) + " (" + num + ")");
		}
		actor.send(strPadding(grn + "B" + nrm + ") ", " ", 3, "left") + strPadding("Owner Vnums", " ", 18, "right") + ": " + cyn + (OLC.ownerVnums.length == 0 ? "No Master" : parts.join(" ")) + nrm);
		actor.send(strPadding(grn + "C" + nrm + ") ", " ", 3, "left") + strPadding("Completion Limit", " ", 18, "right") + ": " + cyn + OLC.completionLimit + nrm);
		var wrap = true;
		var summary = OLC.summary;
		for (var i = 0; i < summary.length; i++) {
			if (summary[i] == "\n") {
				wrap = false;
				break;
			}
		}
		actor.send(strPadding(grn + "D" + nrm + ") ", " ", 3, "left") + strPadding("Summary", " ", 18, "right") + ": " + cyn + "\n" + (wrap == true ? strColFormat(OLC.summary, 55) : OLC.summary) + nrm);
		actor.send(strPadding(grn + "E" + nrm + ") ", " ", 3, "left") + strPadding("Exp Reward", " ", 18, "right") + ": " + cyn + OLC.exp + nrm);
		actor.send(strPadding(grn + "F" + nrm + ") ", " ", 3, "left") + strPadding("QP Reward", " ", 18, "right") + ": " + cyn + OLC.qp + nrm);
		actor.send(strPadding(grn + "G" + nrm + ") ", " ", 3, "left") + strPadding("Copper Reward", " ", 18, "right") + ": " + cyn + OLC.gold + nrm);
		actor.send(strPadding(grn + "H" + nrm + ") ", " ", 3, "left") + strPadding("Required Items", " ", 18, "right") + ": " + cyn + (OLC.items.length > 0 ? OLC.items : "<none>") + nrm);
		actor.send(strPadding(grn + "I" + nrm + ") ", " ", 3, "left") + strPadding("Rewarded Items", " ", 18, "right") + ": " + cyn + (!isEmpty(OLC.itemRewards) ? keyCount(OLC.itemRewards) + " slots" : "<none>") + nrm);
		actor.send(strPadding(grn + "J" + nrm + ") ", " ", 3, "left") + strPadding("Info", " ", 18, "right") + ": " + cyn + (OLC.dialogue.length > 0 ? OLC.dialogue.length + " lines" : "<none>") + nrm);
		actor.send(strPadding(grn + "K" + nrm + ") ", " ", 3, "left") + strPadding("Skills", " ", 18, "right") + ": " + cyn + (OLC.skills.length > 0 ? OLC.skills : "<none>") + nrm);
		actor.send(strPadding(grn + "L" + nrm + ") ", " ", 3, "left") + strPadding("Tasks", " ", 18, "right") + ": " + cyn + (OLC.tasks.length > 0 ? OLC.tasks.length + " tasks" : "<none>") + nrm);
		actor.send(strPadding(grn + "M" + nrm + ") ", " ", 3, "left") + strPadding("Prior Quests", " ", 18, "right") + ": " + cyn + (OLC.priorQuests.length > 0 ? OLC.priorQuests : "<none>") + nrm);
		actor.send(strPadding(grn + "S" + nrm + ") ", " ", 3, "left") + strPadding("Show Quest Giver", " ", 18, "right") + ": " + cyn + capFirstLetter(yesNo(OLC.showGivenBy)) + nrm);
		actor.send(strPadding(grn + "T" + nrm + ") ", " ", 3, "left") + strPadding("Tags", " ", 18, "right") + ": " + cyn + (OLC.tags.length != 0 ? OLC.tags : "<none>") + nrm);
		actor.send(strPadding(grn + "U" + nrm + ") ", " ", 3, "left") + strPadding("Unlock Rank", " ", 18, "right") + ": " + cyn + OLC.unlockRank + nrm);
		actor.send(strPadding(grn + "V" + nrm + ") ", " ", 3, "left") + strPadding("Editors", " ", 18, "right") + ": " + cyn + (OLC.editors.length != 0 ? OLC.editors : "<none>") + nrm);
		actor.send(strPadding(grn + "X" + nrm + ") ", " ", 3, "left") + strPadding("Extra Checks", " ", 18, "right") + ": " + cyn + (OLC.extraChecks.length > 0 ? strAbbrev(OLC.extraChecks + "", 65) : "<none>") + nrm);
		actor.send(strPadding(grn + "Y" + nrm + ") ", " ", 3, "left") + strPadding("FP Reward", " ", 18, "right") + ": " + cyn + OLC.fp + nrm);
		var names = [];
		for (var _autoKey in OLC.allowedRaces) {
			var num = OLC.allowedRaces[_autoKey];
			names.push(capFirstLetter(raceNumToText(num)) + "s");
		}
		actor.send(strPadding(grn + "O" + nrm + ") ", " ", 3, "left") + strPadding("Open to", " ", 18, "right") + ": " + cyn + (OLC.allowedRaces.length == 0 ? "None" : names.join(", ")) + nrm);
		actor.send("\n" + strPadding(grn + "*" + nrm + ") ", " ", 3, "left") + strPadding("Delete", " ", 18, "right") + ": " + (OLC.setDelete == true ? (bld + red) : (cyn)) + capFirstLetter(yesNo(OLC.setDelete)) + nrm);
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Quest Name ***/
	mode = new Object();
	mode.mode = "MODE_QNAME";
	mode.parser = function(actor, fLetter, vArgs) {
		var invalid = /[^a-z0-9\.,\?\!\&\'\"\s]/gi;
		var name = vArgs.join(" ");
		var problems = name.match(invalid) || [];
		if (problems.length > 0) {
			actor.send(bld + "There were invalid characters in your input: " + nrm + red + _.uniq(problems).join(", ") + nrm + bld + "\nTry again: " + nrm);
			return;
		}
		actor.getOLC().name = name;
		actor.getOLC().switchToMode("MODE_QUEST_DATA");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter a name for this quest: " + nrm);
	}
	oConfig.modes.push(mode);
	/*** Quest OPEN TO ***/
	mode = new Object();
	mode.mode = "MODE_OPEN";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		if (isNumber(cmd)) {
			cmd = parseInt(cmd);
			if (cmd == 0) {
				if (!arrContains(actor.getOLC().allowedRaces, 0))
					actor.getOLC().allowedRaces.push(cmd);
				else
					actor.getOLC().allowedRaces = reconArray(actor.getOLC().allowedRaces, arrGetObjIndex(actor.getOLC().allowedRaces, cmd), "remove");
			}
			else if (cmd == 1) {
				if (!arrContains(actor.getOLC().allowedRaces, 1))
					actor.getOLC().allowedRaces.push(cmd);
				else
					actor.getOLC().allowedRaces = reconArray(actor.getOLC().allowedRaces, arrGetObjIndex(actor.getOLC().allowedRaces, cmd), "remove");
			}
			else if (cmd == 2) {
				if (!arrContains(actor.getOLC().allowedRaces, 2))
					actor.getOLC().allowedRaces.push(cmd);
				else
					actor.getOLC().allowedRaces = reconArray(actor.getOLC().allowedRaces, arrGetObjIndex(actor.getOLC().allowedRaces, cmd), "remove");
			}
			else if (cmd == 3) {
				if (!arrContains(actor.getOLC().allowedRaces, 3))
					actor.getOLC().allowedRaces.push(cmd);
				else
					actor.getOLC().allowedRaces = reconArray(actor.getOLC().allowedRaces, arrGetObjIndex(actor.getOLC().allowedRaces, cmd), "remove");
			}
			else {
				if (!arrContains(actor.getOLC().allowedRaces, 5))
					actor.getOLC().allowedRaces.push(5);
				else
					actor.getOLC().allowedRaces = reconArray(actor.getOLC().allowedRaces, arrGetObjIndex(actor.getOLC().allowedRaces, 5), "remove");
			}
			actor.getOLC().switchToMode("MODE_OPEN");
		}
		else if (fLetter == 'A') {
			actor.getOLC().allowedRaces = [0, 1, 2, 3, 5];
			actor.getOLC().switchToMode("MODE_OPEN");
		}
		else if (fLetter == 'N') {
			actor.getOLC().allowedRaces = [];
			actor.getOLC().switchToMode("MODE_OPEN");
		}
		else if (fLetter == 'Q') {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
		}
		else
			actor.send("Your input was invalid. Try again.\n");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var names = [];
		for (var _autoKey in actor.getOLC().allowedRaces) {
			var num = actor.getOLC().allowedRaces[_autoKey];
			names.push(capFirstLetter(raceNumToText(num)) + "s");
		}
		actor.send(grn + "0" + nrm + ") Humans");
		actor.send(grn + "1" + nrm + ") Trollocs");
		actor.send(grn + "2" + nrm + ") Seanchan");
		actor.send(grn + "3" + nrm + ") Aiel");
		actor.send(grn + "4" + nrm + ") Animals");
		actor.send(grn + "A" + nrm + ") All");
		actor.send(grn + "N" + nrm + ") None");
		actor.send(grn + "\nQ" + nrm + ") Quit");
		actor.send(bld + "\nOpen to the following races: " + nrm + cyn + (actor.getOLC().allowedRaces.length == 0 ? "None" : names.join(", ")) + nrm);
	}
	oConfig.modes.push(mode);
	/*** Quest Unlock Rank ***/
	mode = new Object();
	mode.mode = "MODE_RANK";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		if (isNumber(cmd) && cmd >= 0 && cmd < 15) {
			actor.getOLC().unlockRank = cmd;
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer zero to 14.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter the rank at which this quest becomes available(0 if available to any clan, any rank): \nOnly checked for clanned quest masters." + nrm);
	}
	oConfig.modes.push(mode);
	/*** Quest Tags ***/
	mode = new Object();
	mode.mode = "MODE_TAGS";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			if (actor.getOLC().tags.length < 5) {
				actor.getOLC().tags.push("");
				actor.getOLC().index = (actor.getOLC().tags.length - 1);//Store index of item to edit
				actor.getOLC().action = 1;//Edit item
				actor.getOLC().switchToMode("MODE_TAGS_EDIT");
				return;
			}
			else {
				getCharCols(actor);
				actor.send(bld + "You may only place up to five tags on a quest. Remove or replace an existing one.");
				return;
			}
		}
		else if (fLetter == "B" && actor.getOLC().tags.length > 0) {
			actor.getOLC().action = -1;//Remove item
			actor.getOLC().switchToMode("MODE_TAGS_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= actor.getOLC().tags.length) {
			actor.getOLC().action = 1;//Edit item
			actor.getOLC().index = (vArgs[0] - 1);//Store index of item to edit
			actor.getOLC().switchToMode("MODE_TAGS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Tags Menu\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < actor.getOLC().tags.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + cyn + actor.getOLC().tags[i] + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Tag");
		actor.send(grn + "B" + nrm + ") Remove Tag");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Tags Edit ***/
	mode = new Object();
	mode.mode = "MODE_TAGS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().tags.length) {
				actor.getOLC().tags = reconArray(actor.getOLC().tags, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_TAGS");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_TAGS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().tags.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else {
			var index = actor.getOLC().index;
			actor.getOLC().tags[index] = vArgs.join(" ");
			actor.getOLC().switchToMode("MODE_TAGS");
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select tag to remove or Q to quit: " + nrm);
		}
		else if (action == 1) {
			actor.send(bld + "Enter quest tag (used to identify related quests): " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Quest Editors ***/
	mode = new Object();
	mode.mode = "MODE_EDITORS";
	mode.parser = function(actor, fLetter, vArgs) {
		var editorCount = 5;
		var cmd = vArgs[0];
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		//else if (fLetter == "A") {
		//	if (actor.getOLC().editors.length < editorCount) {
		//		actor.getOLC().editors.push("");
		//		actor.getOLC().index = (actor.getOLC().editors.length - 1);//Store index of item to edit
		//		actor.getOLC().action = 1;//Edit item
		//		actor.getOLC().switchToMode("MODE_EDITORS_EDIT");
		//		return;
		//	}
		//	else {
		//		getCharCols(actor);
		//		actor.send(bld + "You may only place up to " + intToText(editorCount) + " editors on a quest. Remove or replace an existing one." + nrm);
		//		return;
		//	}
		//}
		//else if (fLetter == "B" && actor.getOLC().editors.length > 0) {
		//	actor.getOLC().action = -1;//Remove item
		//	actor.getOLC().switchToMode("MODE_EDITORS_EDIT");
		//	return;
		//}
		//else if (isNumber(vArgs[0]) && (vArgs[0] > 1 || arrContains(global.questMasters, actor.name)) && vArgs[0] <= actor.getOLC().editors.length) {
		//	actor.getOLC().action = 1;//Edit item
		//	actor.getOLC().index = (vArgs[0] - 1);//Store index of item to edit
		//	actor.getOLC().switchToMode("MODE_EDITORS_EDIT");
		//	return;
		//}
		//else if (isNumber(cmd) && cmd == 1 && actor.name != 'Alder') {
		//	actor.send("You can not remove the original creator of the quest from the editors list.");
		//	return;
		//}
		//else {
		//	actor.send("Valid inputs are A, B, Q, and any number greater than one from the above list.\nTry again: ");
		//	return;
		//}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Editors Menu\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < actor.getOLC().editors.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + cyn + (i == 0 ? bld : "") + actor.getOLC().editors[i] + nrm);
		}
		//actor.send(grn + "\nA" + nrm + ") Add Editor");
		//actor.send(grn + "B" + nrm + ") Remove Editor");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Editors Edit ***/
	//mode = new Object();
	//mode.mode = "MODE_EDITORS_EDIT";
	//mode.parser = function(actor, fLetter, vArgs) {
	//	var cmd = vArgs[0];
	//	//If removing, select integer from list
	//	if (actor.getOLC().action == -1) {
	//		if ((isNumber(cmd) && cmd > 1 || arrContains(global.questMasters, actor.name)) && cmd <= actor.getOLC().editors.length) {
	//			actor.getOLC().editors = reconArray(actor.getOLC().editors, (cmd - 1), "remove");
	//			actor.getOLC().switchToMode("MODE_EDITORS");
	//			return;
	//		}
	//		if (isNumber(cmd) && cmd == 1 && actor.name != 'Alder') {
	//			actor.send("You can not remove the original creator of the quest from the editors list.");
	//			return;
	//		}
	//		else if (fLetter == "Q") {
	//			actor.getOLC().switchToMode("MODE_EDITORS");
	//			return;
	//		}
	//		else {
	//			actor.send("Input must be an integer between two and " + intToText(actor.getOLC().editors.length) + " or Q to quit.\nTry again: ");
	//			return;
	//		}
	//	}
	//		//Editing
	//	else {
	//		var index = actor.getOLC().index;
	//		actor.getOLC().editors[index] = vArgs.join(" ");
	//		actor.getOLC().switchToMode("MODE_EDITORS");
	//	}
	//}
	//mode.display = function(actor) {
	//	getCharCols(actor);
	//	var action = actor.getOLC().action;
	//	if (action == -1) {
	//		actor.send(bld + "Select editor to remove or Q to quit: " + nrm);
	//	}
	//	else if (action == 1) {
	//		actor.send(bld + "Enter name of immortal to be included among this quest's editors: " + nrm);
	//	}
	//}
	//oConfig.modes.push(mode);
	/*** Quest Owner Vnum ***/
	mode = new Object();
	mode.mode = "MODE_OWNER_VNUM";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().ownerVnums.push("");
			actor.getOLC().index = (actor.getOLC().ownerVnums.length - 1);//Store index of item to edit
			actor.getOLC().action = 1;//Edit item
			actor.getOLC().switchToMode("MODE_OWNER_VNUM_EDIT");
			return;
		}
		else if (fLetter == "B" && actor.getOLC().ownerVnums.length > 0) {
			actor.getOLC().action = -1;//Remove item
			actor.getOLC().switchToMode("MODE_OWNER_VNUM_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= actor.getOLC().ownerVnums.length) {
			actor.getOLC().action = 1;//Edit item
			actor.getOLC().index = (vArgs[0] - 1);//Store index of item to edit
			actor.getOLC().switchToMode("MODE_OWNER_VNUM_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
		if (isNumber(vArgs[0])) {
			actor.getOLC().ownerVnums = parseInt(vArgs[0]);
			if (actor.getOLC().ownerVnums[actor.getOLC().ownerVnums.length - 1])
				actor.getOLC().ownerVnums[actor.getOLC().ownerVnums.length - 1] = -1;
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Owner Vnums Menu\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < actor.getOLC().ownerVnums.length; i++) {
			var vnum = actor.getOLC().ownerVnums[i];
			actor.send(grn + (i + 1) + nrm + ") " + cyn + (getMobProto(vnum) != undefined ? getMobProto(vnum).name : "No Master") + " (" + vnum + ")" + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Vnum");
		actor.send(grn + "B" + nrm + ") Remove Vnum");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Owner Vnum Edit ***/
	mode = new Object();
	mode.mode = "MODE_OWNER_VNUM_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().ownerVnums.length) {
				actor.getOLC().ownerVnums = reconArray(actor.getOLC().ownerVnums, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_OWNER_VNUM");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_OWNER_VNUM");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().ownerVnums.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else {
			var index = actor.getOLC().index;
			if (isNumber(cmd) && cmd >= 0) {
				actor.getOLC().ownerVnums[index] = cmd;
				actor.getOLC().switchToMode("MODE_OWNER_VNUM");
			}
			else {
				actor.send("Input must be an integer greater than zero.\nTry again: ");
			}
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select owner vnum to remove or Q to quit: " + nrm);
		}
		else if (action == 1) {
			actor.send(bld + "Enter a mob vnum: " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Completion Limit ***/
	mode = new Object();
	mode.mode = "MODE_COMPLETION_LIMIT";
	mode.parser = function(actor, fLetter, vArgs) {
		if (isNumber(vArgs[0])) {
			actor.getOLC().completionLimit = vArgs[0];
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter the number of times this quest may be completed(-1 for infinite): " + nrm);
	}
	oConfig.modes.push(mode);
	/*** Summary ***/
	mode = new Object();
	mode.mode = "MODE_SUMMARY";
	mode.parser = function(actor, fLetter, vArgs) {
		actor.getOLC().summary = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_QUEST_DATA");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter a summary of this quest (may contain JS that evaluates to a string): " + nrm);
	}
	oConfig.modes.push(mode);
	/*** EXP ***/
	mode = new Object();
	mode.mode = "MODE_EXP";
	mode.parser = function(actor, fLetter, vArgs) {
		if (isNumber(vArgs[0])) {
			actor.getOLC().exp = vArgs[0];
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter experience reward for quest completion: " + nrm);
	}
	oConfig.modes.push(mode);
	/*** QP ***/
	mode = new Object();
	mode.mode = "MODE_QP";
	mode.parser = function(actor, fLetter, vArgs) {
		if (isNumber(vArgs[0])) {
			actor.getOLC().qp = vArgs[0];
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter quest point reward for quest completion: " + nrm);
	}
	oConfig.modes.push(mode);
	/*** FP ***/
	mode = new Object();
	mode.mode = "MODE_FP";
	mode.parser = function(actor, fLetter, vArgs) {
		if (isNumber(vArgs[0])) {
			actor.getOLC().fp = vArgs[0];
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter feat point reward for quest completion: " + nrm);
	}
	oConfig.modes.push(mode);
	/*** Gold ***/
	mode = new Object();
	mode.mode = "MODE_GOLD";
	mode.parser = function(actor, fLetter, vArgs) {
		if (isNumber(vArgs[0])) {
			actor.getOLC().gold = vArgs[0];
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else {
			actor.send("Input must be an integer.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld + "Enter the amount of copper to be awarded for completing this quest: " + nrm);
	}
	oConfig.modes.push(mode);
	/*** Required Items ***/
	mode = new Object();
	mode.mode = "MODE_REQUIRED_ITEMS";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().items.push(["<vnum>", "#"]);
			actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS");
			return;
		}
		else if (fLetter == "B" && actor.getOLC().items.length > 0) {
			actor.getOLC().action = -1;//Remove item
			actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= actor.getOLC().items.length) {
			actor.getOLC().action = 0;//Edit item
			actor.getOLC().index = (vArgs[0] - 1);//Store index of item to edit
			actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Required Items Menu\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0; i < actor.getOLC().items.length; i++) {
			var arr = actor.getOLC().items[i];
			actor.send(grn + (i + 1) + nrm + ") [" + cyn + arr[1] + nrm + "] " + grn + bld + (getObjProto(arr[0]) != undefined ? (getObjProto(arr[0]).name + nrm + " - " + cyn + arr[0] + nrm) : arr[0]) + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Item");
		actor.send(grn + "B" + nrm + ") Remove Item");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Required Items Edit ***/
	mode = new Object();
	mode.mode = "MODE_REQUIRED_ITEMS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().items.length) {
				actor.getOLC().items = reconArray(actor.getOLC().items, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().items.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else {
			var index = actor.getOLC().index;
			var action = actor.getOLC().action;
			//Entering short-desc/vnum
			if (action == 0) {
				actor.getOLC().items[index] = [];
				if (isNumber(cmd))
					var data = parseInt(vArgs.join(" "));
				else
					var data = vArgs.join(" ");
				actor.getOLC().items[index].push(data);
				actor.getOLC().action = 1;
				actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS_EDIT");
				return;
			}
				//Entering item amount--int
			else if (action == 1) {
				if (isNumber(cmd) && cmd > 0) {
					actor.getOLC().items[index].push(cmd);
					actor.getOLC().switchToMode("MODE_REQUIRED_ITEMS");
					return;
				}
				else {
					actor.send("Input must be an integer greater than zero.\nTry again: ");
					return;
				}
			}
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select item to remove or Q to quit: " + nrm);
		}
		else if (action == 0) {
			actor.send(bld + "Enter object vnum or short-desc: " + nrm);
		}
		else if (action == 1) {
			actor.send(bld + "Enter object amount: " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Rewarded Items ***/
	mode = new Object();
	mode.mode = "MODE_REWARDED_ITEMS";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().action = "NEW SLOT";//Edit slot
			actor.getOLC().switchToMode("MODE_REWARDED_ITEMS_SLOT");
			return;
		}
		else if (fLetter == "B" && !isEmpty(actor.getOLC().itemRewards)) {
			actor.getOLC().action = "REM SLOT";//Remove slot
			actor.getOLC().switchToMode("MODE_REWARDED_ITEMS_SLOT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= keyCount(actor.getOLC().itemRewards)) {
			actor.getOLC().action = "EDIT SLOT";//Edit slot
			actor.getOLC().slot = Object.keys(actor.getOLC().itemRewards)[vArgs[0] - 1]; //Store id of slot to edit
			actor.getOLC().switchToMode("MODE_REWARDED_ITEMS_SLOT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.getOLC().action = undefined;
		actor.getOLC().slot = undefined;
		actor.send("Rewarded Items Menu\n" + strPadding("", "_", 70, "right") + "\n");
		quest = getQuestByName(actor.getOLC().name);
		var itemRewards = actor.getOLC().itemRewards;
		var num = 1;
		for (var id in itemRewards) {
			var slot = itemRewards[id];
			var tierCount = slot.length;
			var s = (tierCount == 1 ? "" : "s");
			actor.send(grn + (num++) + nrm + ") " + cyn + id + nrm + bld + " [" + tierCount + " item" + s + "]" + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Slot");
		actor.send(grn + "B" + nrm + ") Remove Slot");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Rewarded Items Slot***/
	mode = new Object();
	mode.mode = "MODE_REWARDED_ITEMS_SLOT";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		var action = OLC.action;
		var cmd = vArgs[0];
		// When creating a new slot, prompt ID
		if (action == "NEW SLOT" || action == "RENAME") {
			if (!str_cmp(vArgs.join(" "), "Q")) {
				OLC.action = undefined;
				var toMode = "MODE_REWARDED_ITEMS";
				if (action == "RENAME") {
					toMode = "MODE_REWARDED_ITEMS_SLOT";
				}
				OLC.switchToMode(toMode);
				return;
			}
			var id = vArgs.join(" ");
			if (OLC.itemRewards[id]) {
				actor.send("That slot ID already exists for this quest.\nTry again or press Q to return: ");
				return;
			}
			if (action == "NEW SLOT")
				OLC.itemRewards[id] = [];
			else {
				OLC.itemRewards[id] = OLC.itemRewards[OLC.slot];
				delete OLC.itemRewards[OLC.slot];
			}
			OLC.slot = id;
			OLC.action = 0;
			OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			return;
		}
		// When removing a slot, prompt slot list position
		if (action == "REM SLOT") {
			if (isNumber(cmd) && cmd > 0 && cmd <= keyCount(OLC.itemRewards)) {
				var id = Object.keys(OLC.itemRewards)[cmd - 1];
				delete OLC.itemRewards[id];
				OLC.action = undefined;
				OLC.switchToMode("MODE_REWARDED_ITEMS");
			}
			else if (fLetter == "Q") {
				OLC.switchToMode("MODE_REWARDED_ITEMS");
			}
			else {
				actor.send("You must select a slot in the above list to remove, or press Q to return.\nTry again: ");
			}
			return;
		}
			// When creating a new item, prompt tier position
		else if (action == "SPLICE") {
			if (isNumber(cmd) && cmd > 0) {
				var tier = (parseInt(cmd) - 1);
				if (tier > OLC.itemRewards[OLC.slot].length) {
					tier = OLC.itemRewards[OLC.slot].length;
				}
				OLC.action = 0;
				OLC.tier = tier;
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			}
			else if (fLetter == "Q") {
				OLC.action = undefined;
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			}
			else {
				actor.send("You must input an integer greater than zero.\nTry again: ");
			}
			return;
		}
			// When removing an item, prompt tier position
		else if (action == "REMOVE") {
			if (isNumber(cmd) && cmd > 0 && cmd <= OLC.itemRewards[OLC.slot].length) {
				OLC.itemRewards[OLC.slot].splice((parseInt(cmd) - 1), 1);
				OLC.action = undefined;
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			}
			else if (fLetter == "Q") {
				OLC.action = undefined;
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			}
			else {
				actor.send("You must select an item in the above list to remove, or press Q to return.\nTry again: ");
			}
			return;
		}
		else {
			if (fLetter == "Q") {
				if (!OLC.itemRewards[OLC.slot].length) {
					delete OLC.itemRewards[OLC.slot];
				}
				else {
					var slot = OLC.itemRewards[OLC.slot];
					// Attribute each item in the slot with id and tier
					for (var i = 0; i < slot.length; i++) {
						slot[i].tier = i;
						slot[i].id = OLC.slot;
					}
				}
				OLC.switchToMode("MODE_REWARDED_ITEMS");
				return;
			}
			else if (fLetter == "A") {
				if (!OLC.itemRewards[OLC.slot].length) {
					OLC.tier = 0;
					OLC.action = 0;
					OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
					return;
				}
				else {
					OLC.action = "SPLICE";
				}
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
				return;
			}
			else if (fLetter == "B" && OLC.itemRewards[OLC.slot].length > 0) {
				OLC.action = "REMOVE";//Remove item
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
				return;
			}
			else if (fLetter == "C") {
				OLC.action = "RENAME";//Remove item
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
				return;
			}
			else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= OLC.itemRewards[OLC.slot].length) {
				OLC.action = "OVERWRITE";//Edit tier
				OLC.tier = (vArgs[0] - 1);//Store index of tier to edit
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
				return;
			}
			else {
				actor.send("Valid inputs are A, B, C, Q, and any number from the above list.\nTry again: ");
				return;
			}
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		if (actor.getOLC().action == "NEW SLOT" || actor.getOLC().action == "RENAME") {
			actor.send(bld + "Enter this slot's unique ID or Q to return: " + nrm);
			return;
		}
		else if (actor.getOLC().action == "REM SLOT") {
			actor.send(bld + "Select a slot to remove or press Q to return: " + nrm);
			return;
		}
			// When creating a new item, prompt tier position
		else if (actor.getOLC().action == "SPLICE") {
			actor.send("Input the tier position of the new item (tiers attempt to load in ascending order): \nPress Q to return.");
			return;
		}
			// When removing an item, prompt tier position
		else if (actor.getOLC().action == "REMOVE") {
			actor.send("Select the item you wish to remove or press Q to return: ");
			return;
		}
		else {
			actor.send("Rewarded Items Slot: " + cyn + actor.getOLC().slot + nrm + "\n" + strPadding("", "_", 70, "right") + "\n");
			quest = getQuestByName(actor.getOLC().name);
			var itemList = actor.getOLC().itemRewards[actor.getOLC().slot];
			for (var i = 0; i < itemList.length; i++) {
				var item = itemList[i];
				var amount = item.count;
				var loadPerc = item.loadPercent;
				var vnum = item.vnum;
				var name = vnum + nrm + red + " (not made yet)" + nrm;
				if (getObjProto(vnum) != undefined) {
					if (item.isRetooled) {
						name = item.retoolShortDesc + nrm + mag + " (retool)" + nrm + " - " + cyn + vnum + nrm + " - { " + yel + item.retoolNameList + nrm + " } (" + bld + loadPerc + "%)" + nrm;
						name += "\nRoom Description : " + cyn + item.retoolRoomDesc + nrm;
						name += "\nExtra Description: " + cyn + hangIndentStr(item.retoolExtraDesc, 60, 19) + nrm;
					}
					else {
						name = getObjProto(vnum).name + nrm + " - " + cyn + vnum + nrm + " (" + bld + loadPerc + "%)" + nrm;
					}
				}
				actor.send(grn + (i + 1) + nrm + ") [" + cyn + amount + nrm + "] " + grn + bld + name + nrm);
			}
			actor.send(grn + "\nA" + nrm + ") Add Item");
			actor.send(grn + "B" + nrm + ") Remove Item");
			actor.send(grn + "C" + nrm + ") Change Slot ID");
			actor.send("\n" + grn + "Q" + nrm + ") Quit");
		}
	}
	oConfig.modes.push(mode);
	/*** Rewarded Items Edit ***/
	mode = new Object();
	mode.mode = "MODE_REWARDED_ITEMS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		var cmd = vArgs[0];
		var action = OLC.action;
		var tier = OLC.tier;
		var slot = OLC.slot;
		//Entering vnum
		if (action == 0 || action == "OVERWRITE") {
			if (isNumber(cmd)) {
				if (action == 0) {
					OLC.itemRewards[OLC.slot].splice(tier, 0, new Quest.itemRewards("<vnum>", "#", "%"));
				}
				else {
					OLC.itemRewards[OLC.slot][tier].retoolShortDesc = "";
					OLC.itemRewards[OLC.slot][tier].retoolRoomDesc = "";
					OLC.itemRewards[OLC.slot][tier].retoolNameList = "";
					OLC.itemRewards[OLC.slot][tier].retoolExtraDesc = "";
				}
				var data = parseInt(vArgs.join(" "));
				OLC.itemRewards[slot][tier].vnum = data;
				OLC.action = 1;
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			}
			else if (fLetter == "Q") {
				OLC.action = undefined;
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			}
			else {
				actor.send("Item vnum must be an integer greater than zero or Q to return.\nTry again: ");
			}
			return;
		}
			//Entering item amount--int
		else if (action == 1) {
			if (isNumber(cmd) && cmd > 0) {
				OLC.itemRewards[slot][tier].count = (parseInt(cmd));
				OLC.action = 2;
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
				return;
			}
			else {
				actor.send("Input must be an integer greater than zero.\nTry again: ");
				return;
			}
		}
			//Entering load percent
		else if (action == 2) {
			if (isNumber(cmd) && cmd > 0) {
				OLC.itemRewards[slot][tier].loadPercent = (parseInt(cmd));
				OLC.action = 3;
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
				return;
			}
			else {
				actor.send("Input must be an integer greater than zero.\nTry again: ");
				return;
			}
		}
			//Ask if you want to retool
		else if (action == 3) {
			if (fLetter == "Y") {
				OLC.itemRewards[slot][tier].isRetooled = true;
				OLC.action = 4;
				OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			}
			else {
				OLC.action = undefined;
				OLC.itemRewards[slot][tier].isRetooled = false;
				OLC.itemRewards[OLC.slot][tier].retoolShortDesc = "";
				OLC.itemRewards[OLC.slot][tier].retoolRoomDesc = "";
				OLC.itemRewards[OLC.slot][tier].retoolNameList = "";
				OLC.itemRewards[OLC.slot][tier].retoolExtraDesc = "";
				OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			}
			return;
		}
			//Retooling shortdesc
		else if (action == 4) {
			OLC.itemRewards[slot][tier].retoolShortDesc = vArgs.join(" ");
			OLC.action = 5;
			OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			return;
		}
			//Retooling room description
		else if (action == 5) {
			OLC.itemRewards[slot][tier].retoolRoomDesc = vArgs.join(" ");
			OLC.action = 6;
			OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			return;
		}
			//Retooling extra description
		else if (action == 6) {
			OLC.itemRewards[slot][tier].retoolExtraDesc = vArgs.join(" ");
			OLC.action = 7;
			OLC.switchToMode("MODE_REWARDED_ITEMS_EDIT");
			return;
		}
			//Retooling namelist
		else if (action == 7) {
			OLC.itemRewards[slot][tier].retoolNameList = vArgs.join(" ");
			OLC.action = undefined;
			OLC.switchToMode("MODE_REWARDED_ITEMS_SLOT");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select item to remove or Q to return: " + nrm);
		}
		else if (action == 0 || action == "OVERWRITE") {
			actor.send(bld + "Enter item vnum or Q to return: " + nrm);
		}
		else if (action == 1) {
			actor.send(bld + "Enter item amount: " + nrm);
		}
		else if (action == 2) {
			actor.send(bld + "Enter item load percent: " + nrm);
		}
		else if (action == 3) {
			actor.send(bld + "Do you wish to retool the item (Y/N): " + nrm);
		}
			//Retooling name
		else if (action == 4) {
			actor.send(bld + "Enter the item's new name (Example: a sharp axe): " + nrm);
		}
			//Retooling room description
		else if (action == 5) {
			actor.send(bld + "Enter the item's new room description (Shown on ground): " + nrm);
		}
			//Retooling extra desc
		else if (action == 6) {
			actor.send(bld + "Enter the item's extra description (Shown when examined: " + nrm);
		}
			//Retooling namelist
		else if (action == 7) {
			actor.send(bld + "Enter the item's new alias list (Separate aliases by spaces): " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Dialogue ***/
	mode = new Object();
	mode.mode = "MODE_DIALOGUE";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		if (actor.getOLC().action == 1) {
			if (cmd <= actor.getOLC().dialogue.length && cmd > 0) {
				actor.getOLC().dialogue = reconArray(actor.getOLC().dialogue, (cmd - 1), "inject");
				actor.getOLC().dialogue[cmd - 1] = new Quest.Dialogue('#', "<empty>");
				actor.getOLC().action = undefined;
				actor.getOLC().switchToMode("MODE_DIALOGUE");
			}
			else if (cmd > actor.getOLC().dialogue.length && cmd > 0) {
				actor.getOLC().dialogue.push(new Quest.Dialogue("#", "<empty>"));
				actor.getOLC().action = undefined;
				actor.getOLC().switchToMode("MODE_DIALOGUE");
			}
			else {
				actor.send("List positions must be greater than zero.\nTry again: ");
			}
			return;
		}
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().action = 1;
			actor.getOLC().switchToMode("MODE_DIALOGUE");
			return;
		}
		else if (fLetter == "B" && actor.getOLC().dialogue.length > 0) {
			actor.getOLC().action = -1;//Remove dialogue line
			actor.getOLC().switchToMode("MODE_DIALOGUE_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= actor.getOLC().dialogue.length) {
			actor.getOLC().action = 0;//Edit dialogue line
			actor.getOLC().index = (vArgs[0] - 1);//Store index of line to edit
			actor.getOLC().switchToMode("MODE_DIALOGUE_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		var OLC = actor.getOLC();
		getCharCols(actor);
		if (OLC.action == 1) {
			actor.send(bld + "Input list position of new dialogue line: " + nrm);
		}
		else {
			OLC.action = undefined;
			actor.send("Info Menu\n" + strPadding("", "_", 70, "right") + "\n");
			for (var i = 0; i < OLC.dialogue.length; i++) {
				var sCode;
				var pulses;
				if (getObjectClass(OLC.dialogue[i]) != "String") {
					if (OLC.dialogue[i].pulses > -1) {
						pulses = OLC.dialogue[i].pulses;//Waitpulse value
					}
					else if (OLC.dialogue[i].isBeginDialogue) {
						pulses = "SPECIAL_BEGIN";
					}
					else {
						OLC.dialogue[i].pulses = -2;
						pulses = "SPECIAL_COMPLETE";
					}
					sCode = OLC.dialogue[i].handle + "";//String
				}
				else if (getObjectClass(OLC.dialogue[i]) == "String") {
					pulses = "EXTRA";
					sCode = OLC.dialogue[i];
				}
				actor.send(grn + (i + 1) + nrm + ") [" + cyn + pulses + nrm + "] " + yel + sCode + nrm);
			}
			actor.send(grn + "\nA" + nrm + ") Add Dialogue Line");
			actor.send(grn + "B" + nrm + ") Remove Dialogue Line");
			actor.send("\n" + grn + "Q" + nrm + ") Quit");
		}
	}
	oConfig.modes.push(mode);
	/*** Dialogue Edit ***/
	mode = new Object();
	mode.mode = "MODE_DIALOGUE_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().dialogue.length) {
				actor.getOLC().dialogue = reconArray(actor.getOLC().dialogue, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_DIALOGUE");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_DIALOGUE");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().dialogue.length) + ", or Q to quit.\nTry again: ");
				return;
			}
		}
			//Entering dialogue string
		else if (actor.getOLC().action == 0) {
			// Don't want to quit if first word just starts with a Q
			if (fLetter == "Q" && vArgs.length === 1) {
				actor.getOLC().switchToMode("MODE_DIALOGUE");
				return;
			}
			var index = actor.getOLC().index;
			var str = vArgs.join(" ");
			var isPlaintext = function(str) {
				var self = getMobAtRoom(500, 500);
				var actor = self;
				var quest = new Quest();
				try {
					eval(str);
					return false;
				} catch (e) {
					return true;
				}
			};
			if (isPlaintext(str)) {
				str = "self.tell(actor, \"" + str + "\");";
			}
			actor.getOLC().dialogue[index] = new Quest.Dialogue('#', "<empty>");
			actor.getOLC().dialogue[index].handle = str;
			actor.getOLC().action = 1;
			actor.getOLC().switchToMode("MODE_DIALOGUE_EDIT");
			return;
		}
			//Entering waitpulse value
		else {
			var index = actor.getOLC().index;
			if (isNumber(cmd) && cmd >= -2) {
				actor.getOLC().dialogue[index].pulses = Math.floor(parseInt(cmd));
				actor.getOLC().action = undefined;
				actor.getOLC().switchToMode("MODE_DIALOGUE");
				return;
			}
			else {
				actor.send("Input must be an integer -2 or greater.\nTry again: ");
			}
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select dialogue line to remove or Q to quit: " + nrm);
		}
		else if (action == 0) {
			var vars = ["actor - Player", "self - Quest Master", "quest - Current Quest Object"];
			actor.send("Available variables: \n  " + vars.join("\n  "));
			actor.send(bld + '\nEnter dialogue JS code or Q to quit: \n\n' + nrm);
			actor.send('Ex: self.tell( actor, "\Hello, "\ + actor.name);');
			actor.send("You may also enter plaintext as a shortcut to " + yel + "self.tell(actor, \"<your_input>\")" + nrm);
		}
		else {
			actor.send(bld + "Enter waitpulse value(7 = 1 sec) or -1 for begin dialogue, or -2 for completion dialogue: " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Skills ***/
	mode = new Object();
	mode.mode = "MODE_SKILLS";
	mode.parser = function(actor, fLetter, vArgs) {
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().action = 1;
			actor.getOLC().switchToMode("MODE_SKILLS_EDIT");
			return;
		}
		else if (fLetter == "B" && actor.getOLC().skills.length > 0) {
			actor.getOLC().action = -1;//Remove item
			actor.getOLC().switchToMode("MODE_SKILLS_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= actor.getOLC().skills.length) {
			actor.getOLC().action = 0;//Edit item
			actor.getOLC().index = (vArgs[0] - 1);//Store index of item to edit
			actor.getOLC().switchToMode("MODE_SKILLS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Skills Menu\n" + strPadding("", "_", 70, "right") + "\n");
		quest = getQuestByName(actor.getOLC().name);
		for (var i = 0; i < actor.getOLC().skills.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + cyn + actor.getOLC().skills[i] + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Skill");
		actor.send(grn + "B" + nrm + ") Remove Skill");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Skills Edit ***/
	mode = new Object();
	mode.mode = "MODE_SKILLS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().skills.length) {
				actor.getOLC().skills = reconArray(actor.getOLC().skills, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_SKILLS");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_SKILLS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().skills.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else if (actor.getOLC().action == 0) {
			var index = actor.getOLC().index;
			actor.getOLC().skills[index] = (vArgs.join(" "));
			actor.getOLC().action = undefined;
			actor.getOLC().switchToMode("MODE_SKILLS");
			return;
		}
		else if (actor.getOLC().action == 1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().skills.length) {
				actor.getOLC().skills = reconArray(actor.getOLC().skills, (cmd - 1), "inject");
				actor.getOLC().skills[cmd - 1] = "<empty>";
				actor.getOLC().action = undefined;
			}
			else if (isNumber(cmd) && cmd > 0 && cmd > actor.getOLC().skills.length) {
				actor.getOLC().skills.push("<empty>");
				actor.getOLC().action = undefined;
			}
			actor.getOLC().switchToMode("MODE_SKILLS");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select skill to remove or Q to quit: " + nrm);
		}
		else if (action == 0) {
			actor.send(bld + "Enter skill string: " + nrm);
		}
		else {
			actor.send(bld + "Input list position of new skill: " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Tasks ***/
	mode = new Object();
	mode.mode = "MODE_TASKS";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			actor.getOLC().action = 1;
			actor.getOLC().switchToMode("MODE_TASKS_EDIT");
			return;
		}
		else if (fLetter == "B" && actor.getOLC().tasks.length > 0) {
			actor.getOLC().action = -1;//Remove item
			actor.getOLC().switchToMode("MODE_TASKS_EDIT");
			return;
		}
		else if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().tasks.length) {
			actor.getOLC().action = 0;//Edit item
			actor.getOLC().index = (cmd - 1);//Store index of item to edit
			actor.getOLC().switchToMode("MODE_TASKS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		var OLC = actor.getOLC();
		getCharCols(actor);
		OLC.val = undefined;
		actor.send("Tasks Menu\n" + strPadding("", "_", 70, "right") + "\n");
		var display = [];
		for (var i = 0, task; task = OLC.tasks[i++];) {
			actor.send(
				grn + (i) + nrm + ") " +
				yel + task.name + nrm +
				"\n   Progress: " + cyn + (task.isQvalTask ? "(qval task)" : task._dynamicProgress) + nrm +
				"\n   Completion value: " + cyn + task._completedExpr + nrm +
				"\n   Unlocker task: " + cyn + (task.unlockerTaskIndex >= 0 ? "task " + grn + (1 + task.unlockerTaskIndex) : "<none>") + nrm
			);
		}
		actor.send(grn + "\nA" + nrm + ") Add Task");
		actor.send(grn + "B" + nrm + ") Remove Task");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Tasks Edit ***/
	mode = new Object();
	mode.mode = "MODE_TASKS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (OLC.action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= OLC.tasks.length) {
				OLC.tasks = reconArray(OLC.tasks, (cmd - 1), "remove");
				OLC.switchToMode("MODE_TASKS");
				return;
			}
			else if (fLetter == "Q") {
				OLC.switchToMode("MODE_TASKS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(OLC.tasks.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else if (OLC.action == 0) {
			var index = OLC.index;
			var val = OLC.val;
			//Task string
			if (val == 0) {
				if (fLetter == "Q") {
					OLC.switchToMode("MODE_TASKS");
					return;
				}
				OLC.tasks[index].name = (vArgs.join(" "));
				OLC.val++
				OLC.switchToMode("MODE_TASKS_EDIT");
				return;
			}
				//Current value
			else if (val == 1) {
				if (cmd < 0) {
					OLC.tasks[index][val] = null;
				}
				else {
					OLC.tasks[index]._dynamicProgress = vArgs.join(" ");
				}
				OLC.val++;
				OLC.switchToMode("MODE_TASKS_EDIT");
				return;
			}
				//Required value
			else if (val == 2) {
				var input = (vArgs.join(" "));
				if (input.search('getExplorationTask') != -1) {
					var result = (getObjectClass(eval(input.split('(')[0])) == 'Function' && arrContains(input.split('(')[1], ')') && arrGetObjIndex(input.split('(')[1], ';') == (input.split('(')[1].length - 1) ? eval(input) : undefined);
					actor.send(result);
				}
				if (isNumber(cmd) == true && cmd > 0) {
					OLC.val++;
					OLC.tasks[index]._completedExpr = parseInt(cmd);
					OLC.switchToMode("MODE_TASKS_EDIT");
					return;
				}
				else {
					if (result != undefined && isNumber(result) && result > 0) {
						OLC.val++;
						OLC.tasks[index]._completedExpr = input;
						OLC.switchToMode("MODE_TASKS_EDIT");
						return;
					}
					actor.send("Input must be an integer greater than zero (or a valid getExplorationTask(roomVnum)[1]; input).\nTry again: ");
				}
			}
				//Unlocker index
			else if (val == 3) {
				if (isNumber(cmd) == true && cmd > -2 && cmd <= OLC.tasks.length) {
					OLC.val++;
					OLC.tasks[index].unlockerTaskIndex = parseInt(cmd) - 1;
					// If task keeps track of an item, automatically add item to required items array
					var counter = OLC.tasks[index]._dynamicProgress;			// How the task is tracked
					var requiredAmount = eval(OLC.tasks[index]._completedExpr);	// Amount of the item that is required
					if (getObjectClass(counter) == "String" && counter.substr(0, 9) == "itemCount" /*&& getObjectClass(eval(counter)) == "Function"*/ && arrGetObjIndex(counter, "(") > 0 && arrGetObjIndex(counter, "(") < arrGetObjIndex(counter, ")")) {
						var fStr = counter.split("(")[1]; 	// 'item')
						var expression = fStr.split(")")[0];// 'item'
						expression = (isNumber(expression) ? parseInt(expression) : expression.substr(1, expression.length - 2)); // If expression is a vnum, parseInt, otherwise keep item name string
						OLC.items.push([expression, requiredAmount]); // Create new required item element for this task item
					}
					OLC.switchToMode("MODE_TASKS");
					return;
				}
				else {
					actor.send("Input must be 0 or 1.\nTry again: ");
				}
			}
		}
			//Adding
		else if (OLC.action == 1) {
			if (isNumber(cmd) && cmd > 0) {
				var newId = _.max(OLC.tasks, "id") + 1;	// Guarantee an unused id
				OLC.tasks.splice((cmd - 1), 0, new Quest.Task(newId, OLC.quest));
				OLC.action = undefined;
				OLC.switchToMode("MODE_TASKS");
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(OLC.tasks.length) + " or Q to quit.\nTry again: ");
				return;
			}
			OLC.switchToMode("MODE_TASKS");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		if (actor.getOLC().val == undefined) {
			actor.getOLC().val = 0;
		}
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select task to remove or Q to quit: " + nrm);
		}
		else if (action == 0) {
			var val = actor.getOLC().val;
			if (val == 0)
				actor.send(bld + "Enter task string (Q to quit): " + nrm);
			else if (val == 1)
				actor.send(bld + "Enter JSCode which tracks player's task status, or -1 for qval task: \nTo track item posession: itemCount(itemVnum); or itemCount('item name');" + nrm);
			else if (val == 2)
				actor.send(bld + "Enter value signifying task completion: " + nrm);
			else if (val == 3) {
				actor.send(bld + "Enter menu list position of task which unlocks this task, -1 if already unlocked: " + nrm);
				actor.send(bld + "Locked tasks are opened to the player when the specified task is completed." + nrm);
			}
		}
		else {
			actor.send(bld + "Input list position of new task: " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Prior Quests ***/
	mode = new Object();
	mode.mode = "MODE_PRIORQUESTS";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		if (fLetter == "Q") {
			OLC.switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			OLC.action = 1;//Edit item
			OLC.index = OLC.priorQuests.length;// Store index of item to edit
			OLC.switchToMode("MODE_PRIORQUESTS_EDIT");
			return;
		}
		else if (fLetter == "B" && OLC.priorQuests.length > 0) {
			OLC.action = -1;//Remove item
			OLC.switchToMode("MODE_PRIORQUESTS_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= OLC.priorQuests.length) {
			OLC.action = 1;//Edit item
			OLC.index = (vArgs[0] - 1);//Store index of item to edit
			OLC.switchToMode("MODE_PRIORQUESTS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		var OLC = actor.getOLC();
		getCharCols(actor);
		actor.send("Prior Quests Menu\n" + strPadding("", "_", 70, "right") + "\n");
		for (var i = 0, priorId; priorId = OLC.priorQuests[i++];) {
			var quest = Quest.getById(priorId);
			actor.send(grn + (i) + nrm + ") " + Quest.Util.getString(actor, quest));
		}
		actor.send(grn + "\nA" + nrm + ") Add Prior Quest");
		actor.send(grn + "B" + nrm + ") Remove Prior Quest");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Prior Quests Edit ***/
	mode = new Object();
	mode.mode = "MODE_PRIORQUESTS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (OLC.action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= OLC.priorQuests.length) {
				OLC.priorQuests.splice((cmd - 1), 1);
				OLC.switchToMode("MODE_PRIORQUESTS");
				return;
			}
			else if (fLetter == "Q") {
				OLC.switchToMode("MODE_PRIORQUESTS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(OLC.priorQuests.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
		else if (fLetter == "Q") {
			OLC.switchToMode("MODE_PRIORQUESTS");
			return;
		}
			//Editing
		else {
			var index = Math.max(0, OLC.index);
			var input = vArgs.join(" ");
			var prior = input;
			if (!isNumber(input)) {
				var quest = Quest.getByName(input);
				if (!quest) {
					actor.send("Input must be a valid quest name or ID.\nTry again: ");
					return;
				}
				prior = quest.databaseID;
			}
			OLC.priorQuests[index] = _.parseInt(prior);
			OLC.switchToMode("MODE_PRIORQUESTS");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select item to remove or Q to quit: " + nrm);
		}
		else if (action == 1) {
			actor.send(bld + "Enter name or ID of prior quest (Q to quit): " + nrm);
		}
	}
	oConfig.modes.push(mode);
	/*** Extra Checks ***/
	mode = new Object();
	mode.mode = "MODE_EXTRAS";
	mode.parser = function(actor, fLetter, vArgs) {
		var OLC = actor.getOLC();
		if (fLetter == "Q") {
			OLC.switchToMode("MODE_QUEST_DATA");
			return;
		}
		else if (fLetter == "A") {
			OLC.action = 1;//Edit item
			OLC.index = OLC.extraChecks.length;
			OLC.switchToMode("MODE_EXTRAS_EDIT");
			return;
		}
		else if (fLetter == "B" && OLC.extraChecks.length > 0) {
			OLC.index = (vArgs[0] - 1);//Store index of item to edit
			OLC.action = -1;//Remove item
			OLC.switchToMode("MODE_EXTRAS_EDIT");
			return;
		}
		else if (isNumber(vArgs[0]) && vArgs[0] > 0 && vArgs[0] <= OLC.extraChecks.length) {
			OLC.action = 1;//Edit item
			OLC.index = (vArgs[0] - 1);//Store index of item to edit
			OLC.switchToMode("MODE_EXTRAS_EDIT");
			return;
		}
		else {
			actor.send("Valid inputs are A, B, Q, and any number from the above list.\nTry again: ");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Extra Checks Menu\n" + strPadding("", "_", 70, "right") + "\n");
		quest = getQuestByName(actor.getOLC().name);
		for (var i = 0; i < actor.getOLC().extraChecks.length; i++) {
			actor.send(grn + (i + 1) + nrm + ") " + cyn + actor.getOLC().extraChecks[i] + nrm);
		}
		actor.send(grn + "\nA" + nrm + ") Add Check Code");
		actor.send(grn + "B" + nrm + ") Remove Check Code");
		actor.send("\n" + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	/*** Extra Checks Edit **/
	mode = new Object();
	mode.mode = "MODE_EXTRAS_EDIT";
	mode.parser = function(actor, fLetter, vArgs) {
		var cmd = vArgs[0];
		//If removing, select integer from list
		if (actor.getOLC().action == -1) {
			if (isNumber(cmd) && cmd > 0 && cmd <= actor.getOLC().extraChecks.length) {
				actor.getOLC().extraChecks = reconArray(actor.getOLC().extraChecks, (cmd - 1), "remove");
				actor.getOLC().switchToMode("MODE_EXTRAS");
				return;
			}
			else if (fLetter == "Q") {
				actor.getOLC().switchToMode("MODE_EXTRAS");
				return;
			}
			else {
				actor.send("Input must be an integer between one and " + intToText(actor.getOLC().extraChecks.length) + " or Q to quit.\nTry again: ");
				return;
			}
		}
			//Editing
		else {
			var index = actor.getOLC().index;
			actor.getOLC().extraChecks[index] = (vArgs.join(" "));
			actor.getOLC().switchToMode("MODE_EXTRAS");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var action = actor.getOLC().action;
		if (action == -1) {
			actor.send(bld + "Select code to remove or Q to quit: " + nrm);
		}
		else if (action == 1) {
			var vars = ["actor - Player", "self - Quest Master", "quest - Current Quest Object"];
			actor.send("Available variables: \n  " + vars.join("\n  "));
			actor.send(bld + "\nEnter JS code: \nExample: actor.name == 'Joe'" + nrm);
		}
	}
	oConfig.modes.push(mode);
	vOLC.push(oConfig);
}
bootOLC();



