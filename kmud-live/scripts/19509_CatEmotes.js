var script19509 = function(self, actor, here, args, extra)
{
	var rand = random(1,4);
if( rand == 1 )
    here.echo(self.name + " sniffs at something on the ground...");
else if( rand == 2 )
    here.echo(self.name + " purrs softly.");
else if( rand == 3 )
{
    target = getRandomPersonInRoom(here);
    if( target != self )
    {
        here.echoaround(target, self.name + " growls at " + target.name + ".");
        target.send(self.name + " growls at you.");
    }
}

};

