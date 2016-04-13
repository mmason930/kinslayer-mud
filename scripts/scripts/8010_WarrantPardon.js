var script8010 = function(self, actor, here, args, extra) {
	mudLog(constants.BRF, 100, "Race: " + actor.race);
	if( actor.race == constants.RACE_TROLLOC )
		return;
	mudLog(constants.BRF, 100, "Test");
	for(var personIndex = 0;personIndex < actor.room.people.length;++personIndex)
	{
		if( actor.room.people[ personIndex ].mobFlagged( constants.MOB_AWARD ) ) {
			performPardonWarrantAction( actor.room.people[ personIndex ], actor, args, here );
			break;
		}
	}
	function performPardonWarrantAction( self, actor, args, here )
	{
		var vArgs = getArgList(args);
		var bPardon = !str_cmp(vArgs[0],"pardon");
		var bWarrant = !str_cmp(vArgs[0],"warrant");
		var sPlayerName = vArgs[1];
		if( !bWarrant && !bPardon )
			return;
		if( !sPlayerName || sPlayerName.length == 0 )
			return;
		var vSelfClans = self.findClans();
		var vActorClans = actor.findClans();
		var vSharedClans = [];
		if( !vSelfClans.length || !vActorClans.length )
			return;
		for(var iSelfClansIndex = 0;iSelfClansIndex < vSelfClans.length;++iSelfClansIndex)
		{
			for(var iActorClansIndex = 0;iActorClansIndex < vActorClans.length;++iActorClansIndex)
			{
				if( vActorClans[ iActorClansIndex ] == vSelfClans[ iSelfClansIndex ] && actor.isCouncil( vActorClans[ iActorClansIndex ] ) )
					vSharedClans.push( vActorClans[ iActorClansIndex ] );
			}
		}
		//vSharedClans now contains all clans shared by actor and self where actor is part of the council.
		if( vSharedClans.length == 0 )
			return;
		var iClanVnum = vSharedClans[ 0 ]; //We'll use the first shared clan. More sophisticated methods could replace this.
		var sCommand = (bWarrant ? "warrant " : "pardon ") + sPlayerName + " " + iClanVnum;
		self.comm(sCommand);
	}
}