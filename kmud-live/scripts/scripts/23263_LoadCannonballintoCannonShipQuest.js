var script23263 = function(self, actor, here, args, extra) {
		var vArgs = getArgList( args );
		if (vArgs.length != 3)
			return;
		_block;
		var obj = getObjInListVis(actor, vArgs[1], actor.inventory);
		if ( !obj || obj.vnum != 23260 )
		{
			actor.send("For that you would need the proper ammo.");
			return;
		}
		var ball_alias = obj.namelist.split(" ");
		var cannon_alias = self.namelist.split(" ");
		for ( var i = 0; i < ball_alias.length; ++i )
		{
			if ( str_cmp( ball_alias[i], vArgs[1] ) )
				var cmd_pass1 = 1;
		}
		for ( var j = 0; j < cannon_alias.length; ++j )
		{
			if ( str_cmp( cannon_alias[j], vArgs[2] ) )
				var cmd_pass2 = 1;
		}
		if ( cmd_pass1 != 1 || cmd_pass2 != 1 )
			return;
		if ( getSval(self, 23263, "loaded") == 1 )
		{
			actor.send(capFirstLetter( self.name ) + " has already been loaded.");
			return;
		}
		actor.startTimer(3);
		var success = runTimer(actor);
		if ( success )
		{
			actor.send("\nYou roll " + obj.name + " into the barrel of " + self.name + ".");
			here.echoaround(actor, actor.name + " rolls " + obj.name + " into the barrel of " + self.name + ".");
			obj.extract();
			setSval(self, 23263, "loaded", 1);
		}
		else
		{
			var foot_owner = getRandomPersonInRoom( here );
			var damage_amt = ( (foot_owner.hps) / 20 );
			if ( foot_owner != actor )
				actor.send("\n"+capFirstLetter( obj.name ) + " slips from your hands and slams down on " + foot_owner.name + "'s foot before rolling off the deck into the water.");
			else
				actor.send("\n"+capFirstLetter( obj.name ) + " slips from your hands and slams down on your foot before rolling off the deck into the water.");
			here.echoaround(foot_owner, foot_owner.name + " screams in pain as " + foot_owner.his_her + " foot is smashed by " + obj.name + ".");
			foot_owner.send("You scream in pain as " + obj.name + " smashes your foot!");
			foot_owner.damage( damage_amt );
			obj.extract();
		}
				
				
				
}