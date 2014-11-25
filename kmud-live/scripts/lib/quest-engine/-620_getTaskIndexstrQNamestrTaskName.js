//Alder
//May 2010
/*******************************************************************************
 * Returns: index of task with strTaskName in tasks of quest with strQName *
 *******************************************************************************/
function getTaskIndex( strQName, strTaskName ) {
	strQName += "";
	strTaskName += "";
	var quest = getQuestByName(strQName);
	var tasks = quest.tasks;
	for ( var i = 0; i < tasks.length; i++ ) {
		var task = tasks[i];
		if ( task[0] == strTaskName ) {
			return (i);
		}
	}
	return;
}
