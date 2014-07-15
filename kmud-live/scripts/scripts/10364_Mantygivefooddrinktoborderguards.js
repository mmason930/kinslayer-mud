var script10364 = function(self, actor, here, args, extra) {
	var argv = args.split(" ");
	if( argv[0] == "food" )
	{
		waitpulse 2;
		self.comm("say Let me see what I can get for you today " + actor.name + "!");
		self.loadObj(2104);
		waitpulse 4;
		self.comm("give meat " + actor.name);
	}
	else if( argv[0] == "ale" )
	{
		waitpulse 4;
		self.comm("say Coming right up " + actor.name + "!");
		waitpulse 4;
		self.loadObj(8410);
		self.comm("give ale " + actor.name);
		waitpulse 2;
		self.comm("smile " + actor.name);
	}
	else if( argv[0] == "wine" )
	{
		waitpulse 4;
		echoaround(actor, "Manty looks at " + actor.name + " skeptically.");
		actor.send("Manty looks at you skeptically.");
		waitpulse 3;
		self.comm("say Well, if you really want wine...");
		waitpulse 4;
		self.loadObj(2024);
		self.comm("give wine " + actor.name);
	}
		
}