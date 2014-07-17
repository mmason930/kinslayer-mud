var script1210 = function(self, actor, here, args, extra) {
	var vict = actor;
	var count = getSval(self, 1210, "timeToMorph");
	count = count ? count : 0;
	if( count <= 10 )
	{
		if( count == 5 )
		{
			waitpulse random(2, 7);
			here.echo(self.name + " suddenly begins to grow at an alarming pace!");
		}
		count++;
	}
	else
	{
		waitpulse random(3, 7);
		self.extract();
		here.echo("A small spider has fully grown into a huge spider!");
		var newSpider = here.loadMob(1207);
		waitpulse random(3, 7);
		here.echo("The huge spider glares around hungrily and begins to attack!");
		waitpulse 1;
		newSpider.comm("kill " + vict.name);
		return;
	}
	setSval(self, 1210, "timeToMorph", count);
}