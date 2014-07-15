//Alder January 2010
//Do not alter	
/***************************************
 * EntryObj class definition           *
 ***************************************/
function EntryObj(actor,quest) {
	/**CREATE ENTRY FIELDS**/
	this.journal = actor.journal;//Entry's journal
	this.quest = quest;
	this.questOwnerName = (quest.ownerVnum.length > 0 && quest.showGivenBy ? getMobName(actor.quest(quest.questName+"_GIVER")) : '');
	this.entryNum = actor.journal.entries.length + 1;
}

/***************************************
 * Display status of entry's quest     *
 ***************************************/	
EntryObj.prototype.dispStatus = function(actor) {
	var qName = this.quest.questName;
	/**COMPLETED**/
	if ( actor.quest(qName) == -1 )
		return ( "Completed" );
	else if ( actor.quest(qName) > 0 ) {
		var taskData = actor.questTaskProgress(this.quest.questName);
		var tasksCompleted = taskData[0];//Boolean--Are all tasks complete?
		var taskStarted = taskData[taskData.length-1];//Boolean--Is any task started?
		/**READY TO TURN IN**/
		if ( tasksCompleted == true )
			return ( "Ready to Turn In" );
		/**IN PROGRESS**/
		else if ( taskStarted == true )
			return ( "In Progress" );
		/**NEW QUEST**/
		else if ( taskStarted == false )
			return ( "New Quest" );
	}
	return;
}
/**********************************************************************************
 * This function returns a string that is in the format of a quest journal entry. *
 **********************************************************************************/
EntryObj.prototype.compile = function(actor,quest) {
	/**MAKE SHORTCUTS FOR ENTRY FIELDS AND COLORS**/
	var mobName = this.questOwnerName;
	var qName = quest.questName;
	var strSummary = quest.strSummary;
	var skillArray = quest.skillArray;
	var taskArray = quest.taskArray;
	var cyn = actor.cyan(constants.CL_OFF);
	var ylw = actor.yellow(constants.CL_OFF);
	var grn = actor.green(constants.CL_OFF);
	var red = actor.red(constants.CL_OFF);
	var mag = actor.magenta(constants.CL_OFF);
	var blu = actor.blue(constants.CL_OFF);
	var bld = actor.bold(constants.CL_OFF);
	var nrm = actor.normal(constants.CL_OFF);
	var skillsBuff = "\n\t";//Align skills on separate lines
	var taskBuff = "\n";//Align tasks on separate lines
	var taskData = actor.questTaskProgress(qName);
	/**ENTRY SKILLS**/
	if ( !skillArray[0] )
		var skills = "";
	else
		var skills = cyn + "\n\nSkills: " + nrm + skillArray.join(skillsBuff);
	
	/**ENTRY TASKS**/
	var tasks = [];
	if ( taskArray.length > 0 ) {
		for ( var i = 0; i < taskArray.length; i++ ) {
			var task = taskArray[i];
			if ( i != 0 && task[3] != undefined && task[3] > -1 && actor.quest(qName) != -1 ) {
				if ( taskData[parseInt(task[3])+1].crnt < taskData[parseInt(task[3])+1].req )
					continue;//This task is locked and the unlocker task is not complete,do not show in journal
			}
			if ( !task[1] || task[1] == '' || task[1] == 'null' || task[1] == 'undefined' || task[1].length == 0 )
				var crnt = actor.quest(qName + "_" + task[0]);//Qval for currently examined task in myQuest
			else {
				eval("var fPtr = " + task[1] + ";");
				//mudLog(constants.BRF, 105, "var fPtr = " + task[1] + ";");
				if( fPtr ) {
					var crnt = fPtr(actor);
				} else {
					var crnt = actor.quest(qName + "_" + task[0]);//Qval for currently examined task in myQuest
				}
			}
			if ( isNumber(task[2]) == false ) {
				// actor.room.echo('String function: '+task[2]);
				var req = eval(task[2]);
				// actor.room.echo("Req: "+req);
			}
			else {
				var req = task[2];			//Regular
			}
			if ( (crnt/req) > 1 ) {
				crnt = req;
			}
			//Brackets and / are colored depending on quest status
			if ( actor.quest(qName) == -1 ) {
				crnt = req;//Since quest is completed, set tasks to complete
			}
			if ( crnt < req ) {
				var clr = grn + bld;
			}
			else {
				var clr = red + bld;
			}
//					global.alder.send(task[0]);
			tasks.push("\t"+strPadding(task[0]," ",60,"right")+clr+"["+nrm+bld+crnt+nrm+clr+"/"+nrm+bld+req+nrm+clr+"]"+nrm)
		}
		var tasks = cyn + "\n\nTasks: " + nrm + tasks.join(taskBuff);
	}
	else {
		var tasks = "";
	}
	/**Determine quest status**/
	var comp;
	var tasksCompleted = taskData[0];
	//getRoom(13203).echo("taskData[0] = " + taskData[0]);
	var taskStarted = taskData[taskData.length-1];//Last element in actor array--boolean value
	if ( actor.quest(qName) == -1 )
		comp = red + "Complete" + nrm;
	else if ( actor.quest(qName) > 0 ) {
		if ( !taskStarted )//Player has quest, but no tasks have been attempted
			comp = cyn + bld + "New Quest" + nrm;
		else if ( tasksCompleted ) //All tasks complete
			comp = mag + bld + "Ready to Turn In" + nrm;
		else if ( taskStarted )//At least one task is partially complete
			comp = grn + bld + "In Progress" + nrm;
	}
	/**ENTRY TITLE**/
	var givenBy = (!mobName ? '' : " -- Given by "+mobName);
	var title = ") " + grn + qName + nrm + "  (" + comp + ")" + ylw + givenBy + nrm;
	/**ENTRY SUMMARY**/
	var wrap = true;
	if ( arrContains(quest.tags,"Zind Password") == true ) {
		var summArray = strSummary.split("ALDER_ZIND_PASSWORD");
		strSummary = summArray[0];
		strSummary += cyn+(actor.getPval("ZIND_PASS") != null ? actor.getPval("ZIND_PASS") : "no longer available")+nrm;
		strSummary += summArray[1];
	}
	var summary = "\n" + ( wrap == true ? strColFormat(strSummary,55) : strSummary );//Summary is wrapped at column 55
	/**ENTRY REQUIREMENTS**/
	if ( tasks.length > 0 || skills.length > 0 )
		var requirements = cyn + skills + tasks;
	else
		var requirements = "";
	/**COMBINE: TITLE + SUMMARY + REQUIREMENTS**/
	var entry = title + summary + requirements;
	/*****RETURN COMPILED AND FORMATTED JOURNAL ENTRY*****/
	return ( entry );
}

