//Alder January 2010
/*************************************************************************
This function is used to see: 1) If all tasks for the supplied quest are 
complete, and 2) What the actual values are for each individual task.
The function returns an array that would be in the following format:
[booleanAllTasksCompleted,firstTaskObject,secondTaskObject]<----NOTE: Elements after 0 correspond with the tasks.
So the first task in the journal would correspond with element 1 of 
progressArray, and so on.
To read the number of times actor has done a task, the required number of times
a task must be done, the quotient thereof, or booleanCompleted use:
actor.questTaskProgress(strQName)[i].crnt/req/quotient/comp
The final element in progressArray is a boolean value that is true if the
player has done ANY TASK GREATER THAN ZERO TIMES. The value is false otherwise.
*************************************************************************/
JSCharacter.prototype.questTaskProgress = function(strQName) {
	var quest = getQuestByName(strQName);
	var qName = strQName;
	var taskArray = quest.taskArray;
	/** Initialize vars **/
	var progressArray = [];//This is returned at the end
	progressArray.push("boolPlaceHolder");
	var completed = true;
	var started = false;
	var actor = this;
	/** Check tasks progress **/
	for ( var i = 0; i < taskArray.length; i++ ) {
		var task = taskArray[i];
		// this.send("task[1]: "+(task[1].length == 0 || task[1] == null ? 'null' : task[1]));
		
		if ( !task[1] || task[1] == '' || task[1] == 'null' || task[1] == 'undefined' || task[1].length == 0 ) {
			// this.send("Task for action");
			var crnt = this.quest(qName + "_" + task[0]);//Qval for currently examined task in myQuest
			// this.send("crnt: "+crnt);
		}
		else {
			// this.send("Task for item "+task[1]);
			var sCode = "var fPtr = " + task[1];
			eval(sCode);
			if( !fPtr ) {
				//mudLog(constants.BRF, 102, "fPtr is null for quest '" + quest.questName + "' - Task #" + i);
				var crnt = 0;
			}
			else {
				// this.send("fPtr exists");
				var crnt = fPtr(this);
				// this.send(crnt);
			}
			// this.send("crnt: "+crnt);
		}
		var req = eval(task[2]);
		if ( this.quest(qName) == -1 )
			var crnt = req;//Set task to complete if quest is done
		if ( crnt < req ) {		//This task is not done, so overall completion is now false
			// this.send("Completed is false");
			completed = false;
		}
		if ( crnt > 0 )
			started = true;//The player has done this task at least once
		var element = new Object();		//Each element of progressArray greater than zero is an 
			element.crnt = crnt;
			// this.send("element.crnt: "+element.crnt);//object that contains crnt, req, and their quotient...[crnt/req] 
			element.req = req;			//Basically the stuff in the counter in quest journal
			// this.send("element.req: "+element.req);
			element.quotient = crnt/req;
			element.comp = (element.quotient == 1 ? true : false);
			// this.send("element.quotient: "+element.quotient);
		progressArray.push(element);//Assign the completion info for given task to corresponding element in progressArray
	}
	progressArray[0] = completed;
	//getRoom(13203).echo("All tasks completed: " + progressArray[0]);
	progressArray.push(started);
	//getRoom(13203).echo("At least one task begun: " + progressArray[progressArray.length-1]);
	return (progressArray);//Should look similar to: [bool][taskObject][taskObject]...[bool]
}


