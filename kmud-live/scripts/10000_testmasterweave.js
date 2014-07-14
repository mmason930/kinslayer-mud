var script10000 = function(self, actor, here, args, extra)
{
	var arg_array = getArgList(args);
if( arg_array[1] == "'create" && arg_array[2] == "fire'" )
{
	var success = runTimer(actor);
	if(success)
	{
		actor.send("SUCCESS");
	}
	
}

};

