var script1001 = function(self, actor, here, args, extra)
{
	waitpulse 2;
here.echo("A blasted tree flails randomly!");
if (actor.dodge < 100)
{
	waitpulse 4;
	if (actor.room.vnum == 10614)
		{actor.damage(12);
		actor.send("A blasted tree smacks you in the face!");
		actor.say("Ow!")
		waitpulse 2;
		here.echo("A blasted tree says, 'Bangarang, motherfucker'");
		}
		
}
else
{
	waitpulse 4;
	if (actor.room.vnum == 10614)
		{actor.damage(3);	
		actor.send("You dodge to the side and a branch grazes your shoulder");
		}
}

};

