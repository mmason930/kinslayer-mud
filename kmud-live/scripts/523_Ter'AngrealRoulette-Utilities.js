var script523 = function(self, actor, here, args, extra)
{
	self.modes = [];
self.modes["SNOOZE"]  = 0;
self.modes["SIGNUP"]  = 1;
self.modes["REBOOT"]  = 2;
self.modes["PLAYING"] = 3;
self.modes["CYCLE"]   = 4;
self.mode = self.modes["SNOOZE"];
self.iPulsesToStart = 0;
self.getPlayers = function()
{
	if( !this.vPlayers ) this.vPlayers = [];
	return this.vPlayers;
}
self.isPlaying = function( ch )
{
	var vPlayers = this.getPlayers();
	for(var i = 0;i < vPlayers.length;++i) {
		if( ch == vPlayers[i] ) return true;
	}
	return false;
}
self.addPlayer = function( ch )
{
	var vPlayers = this.getPlayers();
	if( !this.isPlaying( ch ) )
		vPlayers.push( ch );
}
self.removePlayer = function( ch )
{
	var vPlayers = this.getPlayers();
	for(var i = 0;i < vPlayers.length;++i)
	{
		if( vPlayers[i] == ch ) {
			vPlayers.splice(i,1);
			return;
		}
	}
}
self.isMode = function( sMode )
{
	return self.mode == self.modes[ sMode ];
}
self.switchToMode = function( sMode )
{
	var iTargetMode = self.modes[ sMode ];
	if( iTargetMode == undefined ) {
		mudLog(constants.NRM, 102, "Ter'angreal Roulette : Attempting to switch to invalid mode: " + sMode);
		return;
	}
	else
		self.mode = iTargetMode;
}
self.beginCountdown = function()
{
	function checkCountdown( vArgs )
	{
		var obj = vArgs[ 0 ];
		var iPulsesPassed = vArgs[ 1 ];
		if( !obj ) return;
		
		if( obj.getPlayers().length == 0 ) {
			obj.iPulsesToStart = 0;
			obj.mode = obj.modes["SNOOZE"];
			return;
		}
		
		obj.iPulsesToStart -= iPulsesPassed;
		
		if( obj.iPulsesToStart <= 0 ) {
			obj.iPulsesToStart = 0;
			obj.reboot();
			return;
		}
		
		obj.room.echo("The game will begin in " + (obj.iPulsesToStart / constants.PULSES_PER_SEC) + " seconds.");
		setTimeout(constants.PULSES_PER_SEC*15, checkCountdown, [obj, constants.PULSES_PER_SEC*15]);
	}
	self.mode = self.modes[ "SIGNUP" ];
	self.iPulsesToStart = constants.PULSES_PER_SEC*15;
	checkCountdown( [self, 0 ] );//Initialize. 0 pulses have spanned.
}
self.startingRoom = function( actor )
{
	if( actor.level >= 100 ) return getRoom(1);
	if( actor.race == constants.RACE_HUMAN ) return getRoom(500);
	if( actor.race == constants.RACE_TROLLOC ) return getRoom(502);
	return getRoom(1);
}
self.reboot = function()
{
	if( !self.room ) return;
	var vPlayers = self.getPlayers();
	
	if( vPlayers.length == 1 ) {
		vPlayers[0].send("Game over! You win!");
		mudLog(constants.BRF, Math.max(vPlayers[0].invis,100), vPlayers[0].name + " has won a round of ter'angreal roulette!");
		gecho(vPlayers[0].name + " has won a round of ter'angreal roulette!");
		self.cleanup();
		return;
	}
	else if( vPlayers.length == 0 ) {
		self.cleanup();
		return;
	}
	self.mode = self.modes[ "REBOOT" ];
	self.vPlayerCycle = [];
	for(var i = 0;i < vPlayers.length;++i)
	{
		self.vPlayerCycle.push( vPlayers[ i ] );
	}
	
	self.switchTurn( false );
}
self.cleanup = function()
{
	self.iPulsesToStart = 0;
	self.mode = self.modes["SNOOZE"];
	self.vPlayers = [];
	self.vPlayerCycle = [];
}
self.switchTurn = function( bIterate )
{
	if( bIterate != false && self.vPlayerCycle.length > 0 )
		self.vPlayerCycle.splice( 0, 1 );
	if( self.vPlayerCycle.length == 0 || self.vPlayers.length == 1 ) {
		self.reboot();
		return;
	}
	var player = self.vPlayerCycle[ 0 ];
	self.room.echo("It is " + player.name + "'s turn.");
	self.mode = self.modes[ "CYCLE" ];
}
self.touch = function( actor )
{
	if( self.mode != self.modes["CYCLE"] || self.vPlayerCycle[ 0 ] != actor ) {
		actor.send("It is not your turn.");
		return;
	}
	function phase4( vArgs ) {
		var actor = vArgs[0];
		var self = vArgs[1];
		var live = vArgs[2];
		if( !live ) {
			self.removePlayer( actor );
			actor.hps = 1;
			actor.send("You are dead! Sorry...");
			actor.moveToRoom( getRoom(1) );
			actor.comm("look");
			self.room.echo(actor.name + " is dead! R.I.P.");
			actor.room.echoaround(actor,actor.name + " arrives suddenly looking dazed and confused.");
			mudLog(constants.CMPL, Math.max(102,actor.invis), capFirstLetter(actor.name) + " just got wrecked in ter'angreal roulette.");
			gecho(capFirstLetter(actor.name) + " just got wrecked in ter'angreal roulette.");
		}
		self.switchTurn();
	}
	function phase3( vArgs ) {
		var actor = vArgs[0];
		var self = vArgs[1];
		var people = self.room.people;
		var live = true;
		if( random(1,100) <= 20 )
			live = false;
		for(var i = 0;i < people.length;++i)
		{
			getCharCols(people[i]);
			if( live )
				people[i].send( cyn + capFirstLetter(self.name) + " glows blue." + nrm );
			else {
				if( actor == people[i] )
					actor.send( red + bld + "Your hand begins to burn..." + nrm );
				else
					people[i].send( red + bld + "The smell of burning flesh fills the room as " + actor.name + " turns red." + nrm );
			}
		}
		setTimeout(constants.PULSES_PER_SEC*1, phase4, [actor,self,live]);
	}
	function phase2( vArgs ) {
		var actor = vArgs[0];
		var self = vArgs[1];
		self.room.echo("Will " + actor.name + " perish?");
		setTimeout(constants.PULSES_PER_SEC*random(3,5), phase3, [actor,self]);
	}
	function phase1( vArgs ) {
		var actor = vArgs[0];
		var self = vArgs[1];
		if( self.vPlayerCycle[ 0 ] != actor ) {
			actor.send("But it's not your turn!");
			return;
		}
		actor.send("You touch " + self.name + ".");
		act("$n touches $p.", false, actor, self, null, constants.TO_ROOM);
		setTimeout(constants.PULSES_PER_SEC*3,phase2,[actor,self]);
	}
	phase1( [actor,self] );
}



};

