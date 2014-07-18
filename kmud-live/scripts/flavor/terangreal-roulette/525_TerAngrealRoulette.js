var script525 = function(self, actor, here, args, extra) {
	var vArgs = getArgList( args );
	_block;
	if( vArgs.length < 2 ) {
		actor.send("Touch what?");
		return;
	}
	var obj = getObjInListVis( actor, vArgs[1], here.items );
	if( !obj ) {
		actor.send("You don't see that here.");
		return;
	}
	if( obj != self ) {
		actor.send("Why would you want to touch that?");
		return;
	}
	function getStartingRoom( ch ) {
		if( ch.level >= 100 )
			return 1;
		if( ch.race == constants.RACE_HUMAN )
			return 500;
		if( ch.race == constants.RACE_TROLLOC )
			return 502;
		return 1;
	}
	self.touch( actor );
	/***
	   if (%(arg0%) == orb || %(arg0%) == ter || %(arg0%) == angreal || %(arg0%) == terangreal)
	   %(send%) %(actor%) You touch %(self.name%).
	   %(echoaround%) %(actor%) %(actor.name%) touches %(self.shortdesc%)
	   wait 2s
	   %(echo%) %(self.shortdesc%) begins to glow...
	   wait 2s
	   set rand %(random.3%)
	   if (%(rand%) == 0)
	   %(echo%) Does %(actor.name%) live, or die?
	   elseif (%(rand%) == 1)
	   %(echo%) Will %(actor.name%) bite the dust?
	   elseif (%(rand%) == 2)
	   %(echo%) Will %(actor.name%) be spared?
	   end
	   wait 4s
	   set chance %(random.4%)
	   if (%(chance%) == 1)
	   %(send%) %(actor%) Your hand begins to burn...
	   %(echoaround%) %(actor%) The smell of burning flesh fills the room as %(self.name%) turns red.
	   wait 6
	   %(echoaround%) %(actor%) %(actor.name%) is dead! R.I.P.
	   %(send%) %(actor%) Your flesh burns to bits! R.I.P.
	   %(teleport%) %(actor%) 502
	   %(force%) %(actor%) look
	   %(echoaround%) %(actor%) %(actor.name%) arrives suddenly looking dazed and confused.
	   else
	   %(echo%) %(self.shortdesc%) glows a bright, soothing blue.
	   end
	   else
	   %(send%) %(actor%) Touch what?
	   end
	***/
}