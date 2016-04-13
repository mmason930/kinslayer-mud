var script4598 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var obj = extra.obj;
	var prices = [ [4583, 5],			// a chunk of impure coal
				   [4587, 9],			// a chunk of tin ore (small)
				   [4597, 23],			// a chunk of tin ore (large)
				   [4588, 11],			// a chunk of copper ore (small)
				   [4595, 26],			// a chunk of copper ore (large)
				   [4586, 21],			// a chunk of aluminum ore (small)
				   [4597, 42],			// a chunk of aluminum ore (large)
				   [4598, 76],			// a chunk of iron ore (small)
				   [4599, 100],			// a chunk of iron ore (large)
				   [4590, 988],			// a chunk of silver ore (small)
				   [4593, 2200],		// a chunk of silver ore (large)
				   [4591, 2303],		// a chunk of gold ore (small)
				   [4592, 5191],		// a chunk of gold ore (large)
				   [4589, 2617],		// a chunk of platinum ore (small)
				   [4594, 5932],		// a chunk of platinum ore (large)
				   [4585, 10000] ];		// a chunk of obsidian ore
				   
	if( actor.race != self.race )
	{
		waitpulse 3;
		self.say("We don't serve your kind here!");
		waitpulse 3;
		self.comm("drop all");
		return;
	}
	else
	{				   
		for( var i = 0; i < 16; i++ )
		{
			if( prices[i][0] == obj.vnum )
			{
				waitpulse 3;
				self.comm("smile");
				wait 1;
				self.say("Thank you, " + actor.name + ".");
				wait 1;
				actor.send( self.name + " gives you some coins." );
				actor.gold += prices[i][1];
				obj.extract();
				return;
			}
		}
		waitpulse 5;
		self.say("What am I supposed to do with this?");
		waitpulse 5;
		self.comm("drop all");
	}
	
}