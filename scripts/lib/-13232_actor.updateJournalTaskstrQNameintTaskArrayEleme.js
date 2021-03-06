//Alder July 2010
/******************************************************************************************
 *  For use when journal entry tasks need to be updated. IE Tasks: smash bugs [0/2].      *
 *  This will generally be in a script triggered by Object Get or Character Death.        *
 *  Function takes string quest name, and the element number of specific task.            *
 *  DO NOT WAITPULSE IN YOUR SCRIPTS ON TASK-RELATED ITEMS                                *
 ******************************************************************************************/
JSCharacter.prototype.updateJournalTask = function (strQName, tasksIndex, boolShowMessage) {
	Quest.getByName(strQName).tasks[tasksIndex].update(this);
	return;

	//var index = tasksIndex;
	//var myQuest = getQuestByName(strQName);
	///**ADD THIS ENTRY IF THE ACTOR IS QVALED FOR THE QUEST ALREADY(CRASHED)**/
	//if (this.journal == false) {
	//	this.recreateJournal();
	//}
	//var entries = this.journal.entries;
	//var targetEntry;
	//var dispMsg = false;//Determines whether update message is shown
	//for (var _autoKey in entries) {
	//	var entry = entries[_autoKey];
	//	if (!str_cmp(strQName, entry.quest.name)) {
	//		targetEntry = entry;
	//	}
	//}
	//if (targetEntry == undefined) {//If targetEntry is undefined, no message is displayed
	//	return;
	//}
	//var tasks = myQuest.tasks;
	//var task = tasks[tasksIndex];
	//var taskData = this.questTaskProgress(strQName);
	//var i = tasksIndex;
	//if (i != 0 && task != undefined && task[3] != undefined && task[3] > -1) {
	//	if (taskData[parseInt(task[3]) + 1].crnt < taskData[parseInt(task[3]) + 1].req) {
	//		return false;//Updating task is locked and the unlocker task is not complete
	//	}
	//}
	//if (tasksIndex != null && task != undefined) {
	//	var spamProt = getSval(this, -13232, strQName + "_SPAM PROTECTION_" + tasksIndex);
	//	var prevCount = getSval(this, -13232, strQName + "PREV ITEMCOUNT" + index);
	//	if (this.quest('QUEST_UPDATES') == 1 || (spamProt != undefined)) {
	//		boolShowMessage = false;
	//	}
	//	setSval(this, -13232, strQName + "_SPAM PROTECTION_" + tasksIndex, (spamProt == undefined ? 1 : spamProt + 1));
	//	if (spamProt == undefined) {
	//		function g(args) {
	//			setSval(args[0], -13232, args[1] + "_SPAM PROTECTION_" + args[2], undefined);
	//			setSval(args[0], -13232, args[1] + "PREV ITEMCOUNT" + args[2], undefined);
	//		}
	//		setTimeout(2, g, [this, strQName, tasksIndex]);
	//	}
	//	if (task[1] != undefined && task[1] != null && task[1] != '' && task[1] != 'null' && task[1] != 'undefined' && task[1].length != 0 && prevCount == undefined) {
	//		var sCode = "var fPtr = " + task[1];
	//		eval(sCode);
	//		setSval(this, -13232, strQName + "PREV ITEMCOUNT" + index, fPtr(this));
	//	}
	//}
	//function f(actor, strQName, tasksIndex, spamProt, boolShowMessage) {
	//	var myQuest = getQuestByName(strQName);
	//	/**ADD THIS ENTRY IF THE ACTOR IS QVALED FOR THE QUEST ALREADY(CRASHED)**/
	//	if (actor.journal == false) {
	//		actor.recreateJournal();
	//	}
	//	var entries = actor.journal.entries;
	//	var targetEntry;
	//	var dispMsg = false;//Determines whether update message is shown
	//	for (var _autoKey in entries) {
	//		var entry = entries[_autoKey];
	//		if (!str_cmp(strQName, entry.quest.name)) {
	//			targetEntry = entry;
	//		}
	//	}
	//	if (targetEntry == undefined) {//If targetEntry is undefined, no message is displayed
	//		return;
	//	}
	//	var tasks = myQuest.tasks;
	//	var task = tasks[tasksIndex];
	//	var taskData = actor.questTaskProgress(strQName);
	//	var i = tasksIndex;
	//	if (i != 0 && task != undefined && task[3] != undefined && task[3] > -1) {
	//		if (taskData[parseInt(task[3]) + 1].crnt < taskData[parseInt(task[3]) + 1].req) {
	//			return false;//Updating task is locked and the unlocker task is not complete
	//		}
	//	}
	//	if (tasksIndex != null && task != undefined) {//If tasksIndex is null, actor is being used in questComplete or object task, so only a message should show
	//		/***ONLY SET QVAL FOR QVAL TASKS(task[1]=null)***/
	//		if (!task[1] || task[1] == '' || task[1] == 'null' || task[1] == 'undefined' || task[1].length == 0 || task[1] == null) {//task[1]=null if task is an 'action' task
	//			var taskQuest = strQName + "_" + task[0];
	//			if (isNumber(task[2]) == false) {
	//				var req = eval(task[2]);
	//			}
	//			else {
	//				var req = task[2];
	//			}
	//			if (actor.quest(taskQuest) < req) {
	//				actor.qval(taskQuest, (actor.quest(taskQuest) + 1));
	//				var crnt = actor.quest(taskQuest);
	//				dispMsg = true;
	//			}
	//		}
	//		else {
	//			var sCode = "var fPtr = " + tasks[tasksIndex][1];
	//			eval(sCode);
	//			var crnt = fPtr(actor);
	//			var req = eval(tasks[tasksIndex][2]);
	//			// actor.send("["+crnt+"/"+req+"]");
	//			var prev = getSval(actor, -13232, strQName + "PREV ITEMCOUNT" + tasksIndex);
	//			if (prev < req) {
	//				dispMsg = true;
	//			}
	//		}
	//	}
	//	if (actor.questTaskProgress(strQName)[tasksIndex + 1].comp == true && boolShowMessage != undefined) {
	//		boolShowMessage = true;
	//	}
	//	if ((dispMsg == true || tasksIndex == null || tasks == undefined) && boolShowMessage != false && spamProt == undefined) {
	//		getCharCols(actor, constants.CL_OFF);
	//		/**SEND JOURNAL UPDATE MESSAGE**/
	//		var status = targetEntry.dispStatus(actor);
	//		if (crnt >= req) {
	//			actor.send(grn + bld + "\nTask Completed: '" + task[0] + "'\n" + nrm);
	//			var newTasks = [];
	//			for (var k = 0; k < tasks.length; k++) {
	//				var lockedTask = tasks[k];
	//				if (k != i && lockedTask[3] != undefined && lockedTask[3] == i) {
	//					newTasks.push(bld + "New task for " + nrm + grn + strQName + nrm + bld + ": " + nrm + cyn + tasks[k][0] + nrm);
	//					continue;
	//				}
	//			}
	//			if (newTasks.length) {
	//				actor.send(newTasks.join("\n") + "\n");
	//			}
	//		}
	//		if (myQuest.ownerVnums.length == 0 && actor.questTaskProgress(strQName)[0] == true) {
	//			actor.qval(strQName, -1);
	//			actor.questCompleteNoMaster(myQuest, actor.findClans());
	//		}
	//		else {
	//			actor.send(yel + bld + "Journal Updated -- Quest: " + strQName + " -- " + cyn + status + nrm);
	//		}
	//		actor.comm("save");
	//	}
	//	else if (dispMsg == false) {
	//		return false;
	//	}
	//}
	//function afterWait(args) {
	//	f(args[0], args[1], args[2], args[3], args[4]);
	//}
	//setTimeout(1, afterWait, [this, strQName, tasksIndex, spamProt, boolShowMessage]);
}