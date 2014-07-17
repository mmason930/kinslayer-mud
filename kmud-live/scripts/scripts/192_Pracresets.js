var script192 = function(self, actor, here, args, extra) {
	var arg_array = getArgList(args);
	if( arg_array[0].toLowerCase() == "help" )
	{
		waitpulse 1;
		self.comm("tell " + actor.name + " To reset your trades, say \"reset trades\".");
		self.comm("tell " + actor.name + " To reset your spells, say \"reset spells\".");
		self.comm("tell " + actor.name + " To reset your skills, say \"reset skills\".");
		self.comm("tell " + actor.name + " Or, say \"reset all\" to reset everything.");
		return;
	}	
	if( arg_array[0].toLowerCase() != "reset" )
		return;
	else if( arg_array[0].toLowerCase() == "reset" && !arg_array[1] )
		self.comm("tell " + actor.name + " Reset what?");
	if( arg_array[1].toLowerCase() == "trades" )
	{
		waitpulse 1;
		self.comm("tell " + actor.name + " Your trade skills have been reset.");
		actor.resetTrades();
		return;
	}
	if( arg_array[1].toLowerCase() == "spells" )
	{
		waitpulse 1;
		self.comm("tell " + actor.name + " Your spells have been reset.");
		actor.resetSpells();
		return;
	}
	if( arg_array[1].toLowerCase() == "skills" )
	{
		waitpulse 1;
		self.comm("tell " + actor.name + " Your skills have been reset.");
		actor.resetSkills();
		return;
	}
	if( arg_array[1].toLowerCase() == "all" )
	{
		waitpulse 1;
		self.comm("tell " + actor.name + " All of your practices have been reset.");
		actor.resetTrades();
		actor.resetSpells();
		actor.resetSkills();
		return;
	}
}