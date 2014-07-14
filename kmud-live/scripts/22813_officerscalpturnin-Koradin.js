var script22813 = function(self, actor, here, args, extra)
{
	var item = extra.obj;
var args = item.name.split(" ");
if (args.length < 5)
	return;
if (args[0] == "a" && args[1] == "bloody")
{
	if (args[2] == "head" || args[2] == "scalp")
	{
		if (args[3] == "of")
		{
		var scalp = []
		for (i = 4; i < args.length; i++)
			scalp.push(args[i])
		var scalp_name = scalp.join(" ");
		wait 1;
		item.extract()
		self.comm("yell " + capFirstLetter(scalp_name) + " has been brought to the Light!");
		wait 1;
		self.say("Bless the Light, great job " + actor.name + "!");
		
		}
	}
}

};

