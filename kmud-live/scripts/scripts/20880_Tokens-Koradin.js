var script20880 = function(self, actor, here, args, extra) {
	//TOKENS global command
	//Koradin - May 2010
	_block;
	var trigger = false;
	for (var _autoKey in getAllTokens()) {
		var token = getAllTokens()[_autoKey];
		if (actor.quest("TOKENS_"+token[0]) > 0)
			trigger = true;
	}
	if (trigger == false)
	{
		actor.send("You haven't gathered any tokens yet.");
		return;
	}
	getCharCols(actor);
	actor.send(" ");
	actor.send("You have gathered the following tokens:");
	actor.send("_______________________________________");
	actor.send(" ");
	for (var _autoKey in getAllTokens()) {
		var token = getAllTokens()[_autoKey];
		if (actor.quest("TOKENS_"+token[0]) > 0)
			actor.send("  a token of "+token[0]+token[1]+"  ["+cyn+actor.quest("TOKENS_"+token[0])+nrm+"]");
	}
	actor.send("_______________________________________");
}