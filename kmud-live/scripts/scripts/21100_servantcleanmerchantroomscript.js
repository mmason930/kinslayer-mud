var script21100 = function(self, actor, here, args, extra) {
	var rand = random(1,5);
	if( rand == 1 ) {
	waitpulse 99;
	here.echo("A diligent servant sweeps the floor.");
	}
	else if( rand == 2 ) {
	waitpulse 99;
	here.echo("A diligent servant averts his eyes.");
	}
	else if( rand == 3 ) {
	waitpulse 99;
	here.echo("A diligent servant dusts.");
	}
	else if( rand == 4 ) {
	waitpulse 99;
	here.echo("A diligent servant scrubs the floors.");
	}
	else if( rand == 5 ) {
	waitpulse 99;
	here.echo("A diligent servant cleans some cobwebs.");
	}
}