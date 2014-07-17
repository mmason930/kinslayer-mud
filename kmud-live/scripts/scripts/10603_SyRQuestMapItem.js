var script10603 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var targetStr = vArgs[1];
	if( isName(targetStr, self.namelist) ) {
	     _block;
	     actor.send("You read the map...");
	     wait 1
	     actor.send(" ");
	     actor.send("-=====================~=====================-");
	     actor.send("|                   Lugard                  |");
	     actor.send("-=====================~=====================-");
	     actor.send("|                                           |");
	     actor.send("|                     S                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                     G                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                     |                     |");
	     actor.send("|                ---L-|                     |");
	     actor.send("|                T                          |");
	     actor.send("|                                           |");
	     actor.send("|                                           |");
	     actor.send("-===================~KEY~===================-");
	     actor.send("| T = Terrion's Base    S = Square          |");
	     actor.send("| G = South Gate        L = Leaftrap        |");
	     actor.send("-=====================~=====================-");
	     actor.send(" ");
	}
	
}