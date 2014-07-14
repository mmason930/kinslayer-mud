var script22329 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);

if(getArg(vArgs, 0).toLowerCase() == "yell" && getArg(vArgs, 1).toLowerCase() == "open" && getArg(vArgs, 2).toLowerCase() == "cell")
{
	if(actor.inClan(constants.CLAN_GAIDIN) || (actor.inClan(constants.CLAN_WHITE_TOWER) && actor.getRank(constants.CLAN_WHITE_TOWER) >= 5))
	{
		
		var guardRoom = getRoom(22328);
		var guard = null;
		
		for each(var mob in guardRoom.people)
		{
			if(mob.vnum == 22328)
			{
				guard = mob;
				break;
			}
		}
		
		if(guard != null)
		{
			wait 1;
			guard.comm("unlock cell");
			guard.comm("open cell");
			wait 5;
			guard.comm("close cell");
			guard.comm("lock cell");
		}
	}
}



};

