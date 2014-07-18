var script108 = function(self, actor, here, args, extra) {
	_block;
	var players = getConnectedPlayers();
	var arg_array = getArgList(args);
	if( actor.level < 102 )
	{
		_noblock;
		return;
	}
	if( arg_array.length != 3 )
	{
		actor.send("Format is qps [clan] [player name]");
		return;
	}
	if( arg_array[1] < 1 || arg_array[1] > constants.CLANS_MAX )
	{
		actor.send("Not a real clan.");
		return;
	}
	for each( var player in players )
	{
		if ( player.name.toLowerCase() == arg_array[2].toLowerCase() )
		{
			actor.send(player.qp(arg_array[1]));
		}
	}
};