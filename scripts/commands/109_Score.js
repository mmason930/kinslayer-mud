let script109 = function(self, actor, here, args, extra) {
	/**************************************************************************
	 *                                                                        *
	 * Command - SCORE                                                        *
	 * Purpose: Provides information about various character status points.   *
	 *                                                                        *
	 * ~~~ By: Galnor 05/16/2010                                              *
	 *                                                                        *
	 **************************************************************************/
	_block;
	getCharCols(actor, constants.CL_COMPLETE);
	let sBuffer = "";
	sBuffer += "You have " + cyn + actor.hps + nrm + "(" + cyn + actor.maxHit + nrm + ") hit";
	if( channelingAbility(actor ) )
		sBuffer += ", " + cyn + actor.sps + nrm + "(" + cyn + actor.maxSpell + nrm + ") spell,";
	if( actor.class === constants.CLASS_FADE )
		sBuffer += ", " + cyn + actor.shadowPoints  + nrm + "(" + cyn + actor.maxShadow + nrm + ") shadow points,";
	sBuffer += " and " + cyn + actor.mvs + nrm + "(" + cyn + actor.maxMove + nrm + ") movement points.\n";
	sBuffer += "You have scored " + cyn + actor.experience + nrm + " experience and " + cyn + actor.totalQuestPoints + nrm + " total quest points.\n";
	if( actor.level < 100 ) {
		sBuffer += "You need " + cyn + actor.expToLevel + nrm + " exp to reach your next level.\n";
	}
	sBuffer += "You have been playing for " + cyn + actor.playingDays + nrm + " days and " + cyn + actor.playingHours + nrm + " hours.\n";
	if( actor.weavePoints > 0 ) {
		sBuffer += "You have gathered " + cyn + actor.weavePoints + nrm + " Weave Points so far";
		if( actor.legend >= 1 && actor.legend <= 8 )
			sBuffer += ", ranking you Legend " + cyn + actor.legend + nrm;
		sBuffer += ".\n";
	}
	sBuffer += "This ranks you as " + cyn + actor.name + (actor.title?(" " + actor.title):"") + nrm + " (level " + cyn + actor.level + nrm + ")\n";
	//sBuffer += "You are not a member of any clans.";
	function channelingAbility( actor ) {
		return (actor.class === constants.CLASS_DREADLORD || actor.class === constants.CLASS_DREADGUARD || actor.class === constants.CLASS_CHANNELER);
	}
	function qpNeededToRank( iCurrentRank ) {

		let vRankRequirements =
		[
			0, 15, 30, 65, 150, 300, 550, 1000, 1500, 2100, 2700, 3600, 4600, 5600
		];
		//The index of the requirement is one below the rank it represents.
		//So the quest points to get to rank 5 is stored in vRankRequirements[ 4 ]
		//This means if you pass the player's current rank into the array, it will spit out the quest points
		//they need to obtain in order to move up a rank.
		
		if( iCurrentRank < 0 || iCurrentRank >= vRankRequirements.length )
			return 1000000000;
		return vRankRequirements[ iCurrentRank ];
	}
	let vClans = actor.findClans();
	if(typeof vClans == "number" && vClans != -1)
		vClans = [vClans];
	if( vClans.length ) {
		sBuffer += "You are in the following clans:\n";
	}
	for(let clanIndex = 0;clanIndex < vClans.length;++clanIndex)
	{//Display each clan.
		let sLine = "";
		sLine += clanNumToText( vClans[ clanIndex ] );
		sLine += " : " + cyn + actor.qp( vClans[ clanIndex ] ) + nrm + " Quest Points, rank " + cyn + actor.getRank( vClans[ clanIndex ] ) + nrm + ", ";
		sLine += (qpNeededToRank( actor.getRank( vClans[ clanIndex ] ) ) - actor.qp(vClans[ clanIndex ])) + " Quest Points to rank.";
		sBuffer += sLine + "\n";
	}
	let killStreak = actor.quest(actor.name+"_KILL_STREAK");
	if ( killStreak > 1 ) {
		sBuffer += ("You are currently on a "+cyn+killStreak+nrm+" kill streak.\n");
	}
	let featPoints = actor.featPoints;
	if ( featPoints ) {
		sBuffer += "You have acquired "+cyn+featPoints+nrm+" feat points.\n";
	}
	sBuffer += "Your current Arena ranking is " + cyn + getArenaRank(actor) + nrm + ".\n";
	if( actor.mount ) {
		let mount = actor.mount;
		sBuffer += "You are riding " + cyn + mount.name + nrm + ".\n";
		if( mount.mvs > mount.maxMove * 0.75 )
			sBuffer += "Your mount is full of energy.\n";
		else if( mount.mvs > mount.maxMove * 0.50 )
			sBuffer += "Your mount looks strong.\n";
		else if( mount.mvs > mount.maxMove * 0.25 )
			sBuffer += "Your mount looks weary.\n";
		else
			sBuffer += "Your mount looks haggard and beaten.\n";
	}
	if (getSval(actor,20850,"mining") === 1)
		sBuffer += "You are mining.";
	else if( actor.position === constants.POS_STANDING )
		sBuffer += "You are standing.";
	else if( actor.position === constants.POS_FIGHTING ) {
		sBuffer += "You are fighting ";
		sBuffer += cyn;
		if( actor.fighting )
			sBuffer += actor.fighting.name + nrm + ".";
		else
			sBuffer += " an invisible force" + nrm + "!";
	}
	else if( actor.position === constants.POS_SITTING )
		sBuffer += "You are sitting.";
	else if( actor.position === constants.POS_RESTING )
		sBuffer += "You are resting.";
	else if( actor.position === constants.POS_SLEEPING )
		sBuffer += "You are sleeping.";
	else if( actor.position === constants.POS_STUNNED )
		sBuffer += "You are stunned.";
	else if( actor.position === constants.POS_INCAP )
		sBuffer += "You are incapacitated.";
	else if( actor.position === constants.POS_MORTALLYW )
		sBuffer += "You are mortally wounded.";
	else if( actor.position === constants.POS_FLYING )
		sBuffer += "You are flying.";
	else
		sBuffer += "You are dead.";
		
	if( actor.hunger === 0 )
		sBuffer += "\nYou are hungry.";
	if( actor.thirst === 0 )
		sBuffer += "\nYou are thirsty.";
	if( actor.drunk >= 10 )
		sBuffer += "\nYou are intoxicated.";
	actor.send( sBuffer );
};