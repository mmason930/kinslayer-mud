var script10699 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var targetStr = vArgs[1];
	if( isName(targetStr, self.namelist) ) {
	     _block
	     actor.send("You use a strangely twisted stone rod.");
	     actor.heal(100);
	     actor.mvs += 100;
	     actor.send("You feel better!"); 
	}
}