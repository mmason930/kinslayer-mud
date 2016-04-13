var script20882 = function(self, actor, here, args, extra) {
	// Drop script for the bright green stone object
	// Original by Koradin (DG #20882), rewritten in JavaScript by Koradin March 2011
	waitpulse 10;
	here.echo("The stone immediately breaks apart and sinks into the ground!");
	waitpulse 20;
	here.echo("Suddenly a strange plant starts growing at an alarming rate...");
	waitpulse 20;
	here.echo("The plant splits, multiplying and creeping along the ground...");
	waitpulse 20;
	here.echo("The forest of thick vines writhes here, making travel nearly impossible!");
	here.loadObj(9015);
	self.extract();
}