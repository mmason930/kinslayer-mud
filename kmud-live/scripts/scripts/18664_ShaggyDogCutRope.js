var script18664 = function(self, actor, here, args, extra) {
	_block;
	if( getSval(self, 18664, "HasBeenCut") == 1 ) {
		actor.send("There is nothing to cut.");
	}
	else {
		var vArgs = getArgList(args);
		if( vArgs.length >= 2 && !str_cmp(vArgs[1],"rope") )
		{
			_block;
			actor.send("You cut the rope from around the stray's neck.");
			here.echoaround(actor, actor.name + " cuts the rope from around the stray's neck.");
			self.comm("follow " + actor.namelist.split(" ")[0]);
			setSval(self, 18664, "HasBeenCut", 1);
		}
		else
			actor.send("Cut what?");
	}
}