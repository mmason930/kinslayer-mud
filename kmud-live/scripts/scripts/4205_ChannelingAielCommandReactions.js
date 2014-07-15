var script4205 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( strn_cmp(vArgs[0], "order", 2) )
	{
		_block;
		actor.send( self.name + " makes a slashing motion, and you cry out in pain!" );
		echoaround( actor, self.name + " makes a slashing motion at " + actor.name + ", who cries out in pain!" );
		actor.damage( 25, self );
		return;
	}
	else if( strn_cmp(vArgs[0], "emote", 3) )
	{
		_block;
		actor.send( self.name + " wraps you in cords of fire, preventing you from moving!" );
		echoaround( actor, self.name + " points at " + actor.name + ", who is suddenly wrapped in cords of fire!" );
		actor.damage( 25, self );
		return;
	}
	else if( strn_cmp(vArgs[0], "say", 1) || vArgs[0] == "'" )
	{
		_block;
		actor.send( self.name + " laughs maniacally as you feel the air get sucked out of your lungs!" );
		echoaround( actor, self.name + " laughs maniacally and " + actor.name + " begins to clutch at " + actor.his_her() + " throat!" );
		actor.damage( 25, self );
		if( !actor.affectedBy( constants.AFF_SILENCE ) )
			actor.affect( constants.AFF_SILENCE, 1, 0, 0 );
		return;
	}
	else if( strn_cmp(vArgs[0], "bash", 3) && ( vArgs[1] == "channeling" || vArgs[1] == "aiel" 
			|| vArgs[1] == "warrior" || vArgs[1] == "male" || vArgs[1] == "channeler" || (!vArgs[1] && actor.fighting == self) ) )
	{
		_block;
		actor.send( self.name + " whips up a small whirlwind, preventing your attack!" );
		echoaround( actor, self.name + " twirls around " + self.his_her() + " staff, surrounding " + actor.name + " with a small whirlwind!" );
		actor.lag(12);
		waitpulse 12;
		here.echo("The whirlwind breaks off and animates a cloud of dust!");
		var minion = here.loadMob(4206);
		minion.comm("follow channeling");
		minion.comm("assist channeling");
		return;
	}
}