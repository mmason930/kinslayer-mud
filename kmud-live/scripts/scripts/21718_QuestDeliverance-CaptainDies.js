var script21718 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Deliverance
	/** Death: a Captain of the Queen's Guard **/
	var pointMan = getSval(self, 386, "DELIVERANCE_POINTMAN");
	function afterWait(args) {
		pointMan = args[0];
		if (pointMan) {
			var qName = "Deliverance";
			var quest = Quest.getByName(qName);
			var task = quest.tasks[3];
			if (task.isInProgress(pointMan)) {
				task.update(pointMan);
			}
		}
	}
	setTimeout(1, afterWait, [pointMan]);

}