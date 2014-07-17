var script13214 = function(self, actor, here, args, extra) {
	//Alder January 2010
	_block; //Re-engaged by Koradin March 28 2012 because it was making silly messages when people used Journal
	var vArgs = getArgList(args);
	var cmd = vArgs[0];
	for ( var i = 1; i < vArgs.length; i++ ) {
		cmd += " "+vArgs[i];
	}
	actor.dispJournal(cmd);
	
	
}