var script14204 = function(self, actor, here, args, extra)
{
	function afterWait(args) {
	var actor = args[0];
	var dir = args[1];
	if (dirToText(dir).toLowerCase() == "up") {
	actor.send("You plunge into the rushing waters.");
	act("$n plunges into the rushing waters from the bridge above.",true,actor,null,null,constants.TO_ROOM);
	}
}
setTimeout(1,afterWait,[actor,extra.direction]);

};

