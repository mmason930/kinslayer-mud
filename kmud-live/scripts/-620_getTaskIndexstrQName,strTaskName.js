//Alder
//May 2010
/*******************************************************************************
 * Returns: index of task with strTaskName in taskArray of quest with strQName *
 *******************************************************************************/
function getTaskIndex( strQName, strTaskName ) {
	strQName += "";
	strTaskName += "";
	var quest = getQuestByName(strQName);
	var taskArray = quest.taskArray;
	for ( var i = 0; i < taskArray.length; i++ ) {
		var task = taskArray[i];
		if ( task[0] == strTaskName ) {
			return (i);
		}
	}
	return;
}
