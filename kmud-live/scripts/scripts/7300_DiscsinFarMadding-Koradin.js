var script7300 = function(self, actor, here, args, extra) {
	//This script detects men and women channeling on the outskirts of Far Madding.
	//Koradin - December 2009
	var west = [7317,7318,7319,7320,7321,7322,7323,7324,7325,7326,7327,7328,7329,7330,7331,7332,7333,7334,7335,7336,7337,7338,7339,7348,7349];
	var northwest = [7340,7341,7342,7343,7344,7345,7346,7347,7232,7226,7220,7221,7227,7350];
	var north = [7228,7229,7230,7231,7225,7224,7223,7222,7218,7217,7219,7216,7215,7210,7213,7212,7209,7207,7205,7206,7203,7204,7208,7211,7214,7202,7201];
	var area = [north,west,northwest];
	var arg_array = getArgList(args);
	_noblock;
		for each(var room in area[0])
		{
			if (actor.room.vnum == room)
				var direction = "north"
		}
		for each(var room in area[1])
		{
			if (actor.room.vnum == room)
				var direction = "west"
		}
		for each(var room in area[2])
		{
			if (actor.room.vnum == room)
				var direction = "northwest"
		}
	if (direction)
	{
		if (actor.sex == constants.SEX_MALE && strn_cmp(arg_array[0],"seize",3) && (actor.class == constants.CLASS_CHANNELER || actor.class == constants.CLASS_DREADLORD || actor.class == constants.CLASS_DREADGUARD))
			var color = "swirl black";
		if (actor.sex == constants.SEX_FEMALE && strn_cmp(arg_array[0],"embrace",2) && (actor.class == constants.CLASS_CHANNELER || actor.class == constants.CLASS_DREADLORD || actor.class == constants.CLASS_DREADGUARD))
			var color = "burn an ominous red";
		if (color)
		{
			waitpulse 1;
			here.zecho("The crystal wedges of the Guardian begin to " + color + " and triangulate, pointing " + direction + ".");
		}
	}
}