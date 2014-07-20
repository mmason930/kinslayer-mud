var script20803 = function(self, actor, here, args, extra) {
	//This script will return the number of arrows in a quiver when you EXAMINE, LOOK AT, or LOOK IN it.
	//Koradin - December 2009
	var arg_array = getArgList(args);
	if (arg_array.length < 2 || arg_array.length > 3)
	{
		_noblock;
		return;
	}
	else if (arg_array.length == 3)
	{
		if (arg_array[0] == "l" || arg_array[0] == "lo" || arg_array[0] == "loo" || arg_array[0] == "look")
		{
			if (arg_array[1] == "i" || arg_array[1] == "a" || arg_array[1] == "in" || arg_array[1] == "at")
			{
				for (var _autoKey in self.namelist.split(" ")) {
					var word = self.namelist.split(" ")[_autoKey];
					if (word == arg_array[2])
						var trigger = 1;
				}
			}
			else
				_noblock;
		}
		else
			_noblock;
	}
	else if (arg_array.length == 2)
	{
		if (arg_array[0] == "exa" || arg_array[0] == "exam" || arg_array[0] == "exami" || arg_array[0] == "examin" || arg_array[0] == "examine")
		{
			for (var _autoKey in self.namelist.split(" ")) {
				var word = self.namelist.split(" ")[_autoKey];
				if (word == arg_array[1])
					var trigger = 1;
			}
		}
		else
			_noblock;
	}
	if (trigger == 1)
	{
		_block;
		if (self.vnum == 1720)
			var arrow = "a diamond-tipped arrow";
		else if (self.vnum == 1721)
			var arrow = "a streamlined arrow";
		else if (self.vnum == 1722)
			var arrow = "a burning arrow";
		else if (self.vnum == 1723)
			var arrow = "an icy steel arrow";
		else if (self.vnum == 1724)
			var arrow = "an explosive arrow";
		else if (self.vnum == 1725)
			var arrow = "an arrow dipped in venom";	
		else if (self.vnum == 1726)
			var arrow = "a decaying arrow";
		actor.send("It is " + self.name + ", in very good condition.");
		actor.send(" ");
		actor.send("When you look inside, you see:");
		actor.send(" ");
		if (self.getPval("arrows") > 0)
			actor.send(arrow + " [" + self.getPval("arrows") + "]");
		else
			actor.send("It is empty.");
	}
}