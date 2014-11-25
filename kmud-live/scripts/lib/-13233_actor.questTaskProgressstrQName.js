//Alder January 2010
//Rhollor September 2014 - Updated to a wrapper for Quest API v2
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

RETURNS: [boolAllTasksCompleted, taskObject, taskObject, ... , boolAnyProgressMade]
*************************************************************************/
JSCharacter.prototype.questTaskProgress = function (strQName) {
	mudLog(constants.BRF, 100, "In questTaskProgress");
	var actor = this;
	var quest = Quest.getByName(strQName);
	var tasks = quest.tasks;
	
	/** Initialize vars **/
	var progressArray = [];//This is returned at the end
	progressArray.push(quest.hasCompleted(actor));
	
	/** Recreate previous taskObjects **/
	for (var i = 0; i < tasks.length; i++) {
		var crnt = tasks[i].getProgress(actor);
		var req = tasks[i].completedValue;
		var vTask = {
			crnt:		crnt,
			req:		req,
			quotient:	crnt/req,
			comp:		tasks[i].getStatus(actor).completed
		};
 
		progressArray.push(vTask);
	}
	
	progressArray.push(quest.anyTaskStarted(actor));

	return (progressArray);				//Should look similar to: [bool][taskObject][taskObject]...[bool]
}




