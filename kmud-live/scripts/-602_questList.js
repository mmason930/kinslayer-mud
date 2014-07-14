function questList(self,actor,quests) {
    //List all the available quests.
	getCharCols(actor,constants.CL_OFF);
    actor.send(red+" Available Quests"+nrm);
    actor.send(bld+strPadding("","-",19,"right")+nrm);
	for(var i = 0;i < quests.length;++i) {
		var qName = quests[i].questName;
		var taskData = actor.questTaskProgress(qName);
		var tasksCompleted = taskData[0];
		//getRoom(23514).echo("taskData[0] = " + taskData[0]);
		var taskStarted = taskData[taskData.length-1];//Last element in actor array--boolean value
		if ( actor.quest(qName) == -1 ) {
			comp = "Complete";
		}
		else if ( actor.quest(qName) == 0 )
				comp = "Not Started";
		else if ( actor.quest(qName) > 0 ) {
			if ( tasksCompleted == true ) //All tasks complete
				comp = "Ready to Turn In";
			else
				comp = "In Progress";
		}
		var sFinal = actor.cyan(constants.CL_COMPLETE) + (i+1) + actor.normal(constants.CL_COMPLETE) + ") "
			+ actor.green(constants.CL_COMPLETE) + quests[i].questName;
		sFinal = strPadding( sFinal, " ", 40, "right" );
        actor.send( sFinal  + actor.normal(constants.CL_COMPLETE) + "  (" + comp + ")");
    }
}
