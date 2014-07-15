var script12 = function(self, actor, here, args, extra) {
	var arglist = args.split(" ");
	if( arglist[1] == 'timer' ) {
	    actor.startTimer(3);
	    runTimer(actor);
	    here.echo("Timer finished.");
	} else {
	    here.echo("Invalid argument.");
	}
}