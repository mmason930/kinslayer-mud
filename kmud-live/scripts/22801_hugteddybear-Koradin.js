var script22801 = function(self, actor, here, args, extra)
{
	var arg_array = getArgList(args);
if (!arg_array[1])
{
	_noblock;
	return;
}
else
{
	if (arg_array[1] == "teddy" || arg_array[1] == "bear" || arg_array[1] == "enormous")
	{
	actor.send("You squeeze " + self.name + ", bringing back memories of your childhood.");
	echoaround(actor, actor.name + " squeezes " + self.name + " fondly, remembering " + actor.his_her() + " childhood.");
	_block;
	}
}

};

