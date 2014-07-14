var script1208 = function(self, actor, here, args, extra)
{
	waitpulse 4;
if( !getSval(here, 1208, "web") )
{
	here.echo(self.name + " crawls in front of the entrance and spins a large web, sealing it off!");
	setSval(getRoom(1207), 1208, "web", getRoom(1207).loadObj(20639));
	setSval(getRoom(1208), 1208, "web", getRoom(1208).loadObj(20640));
}

};

