	//Alder
//May 2010
/*******************************************************************
 * strQName: name of quest being examined for availability         *
 * Returns: [ boolQuestAvailability, [ vnumsMastersGivingQuest ] ] *
 *******************************************************************/
JSCharacter.prototype.isQuestAvailable = function(strQName) {
	var quest = getQuestByName(strQName);
	var available = true;
	var availableMasters = [];
	var actor = this;
	var here = this.room;
	function checks(elementValue, elementIndex, targetArray) {
		return (eval(elementValue) == true);
	}
	if ( quest.ownerVnum.length > 0 ) {
		/***Find masters where this quest is available***/
		for ( var i = 0; i < quest.ownerVnum.length; i++ ) {
			var vnum = quest.ownerVnum[i];
			if ( this.quest(vnum+'_BLOCK_QUESTS') == 1 ) {
				// here.echo("BLOCKED");
		
				continue;	// Player has been blocked from this quest master
			}
			var mProto = getMobProto(vnum);
			var mobClans = mProto.findClans();
			var actorClans = this.findClans();
			if ( quest.unlockRank > 0 ) {
				if( !this.clanCheck(mobClans) ) {
					// here.echo("clanCheck");
					continue;
				}
				if ( mobClans.length && this.getRank(mobClans[0]) < quest.unlockRank ) {
					// here.echo("Rank");
					continue;	// Player is not correct clan or rank
				}
			}
			var self = mProto;
			if ( quest.extras.length > 0 && quest.extras.every(checks) == false ) {
				// here.echo("Extra check");
				
				continue	// Player doesn't pass all extra checks with this master
			}
			availableMasters.push(vnum);
		}
	}
	else {
		if ( quest.extras.length > 0 && !quest.extras.every(checks) ) {
		
			available = false;		// Player doesn't pass all extra checks
		}
	}
	/**IF priorQuests.length == 0 || priorQuests[j] IS COMPLETE, INCLUDE THIS QUEST**/
	for ( var j = 0; j < quest.priorQuests.length; j++ ) {
		if ( this.quest(quest.priorQuests[j]) != -1 ) {
		
			available = false;		// Quest is not unlocked, unavailable
		}
	}
	if ( !arrContains(quest.open,this.race) ) {
		available = false;			// Player is not of a valid race
	}
	if ( available == false ) {
		availableMasters = [];		// If it isn't available for any reason, then there are no masters offering it
	}
	if ( availableMasters.length == 0 && quest.ownerVnum.length > 0 ) {	
		available = false;			// If no masters are offering, it isn't available
	}
	return [available||availableMasters.length>0,availableMasters];
}
