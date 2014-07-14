var script8102 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs[1] == "flesh" )
{
	_block;
	waitpulse 2;
	actor.send("You reach into " + self.name + " and rip off a piece and it falls to the ground.");
	echoaround( actor, actor.name + " reaches into " + self.name + " and rips a piece off, which falls to the ground." );
	here.loadObj(2106);
}

};

