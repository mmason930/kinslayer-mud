var script22328 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);

if(actor.inClan(constants.CLAN_GAIDIN) || (actor.inClan(constants.CLAN_WHITE_TOWER) && actor.getRank(constants.CLAN_WHITE_TOWER) >= 5))
{
	wait 1;
	self.comm("unlock cell");
	self.comm("open cell");
	wait 5;
	self.comm("close cell");
	self.comm("lock cell");
}
else
{
	_block;
}

};

