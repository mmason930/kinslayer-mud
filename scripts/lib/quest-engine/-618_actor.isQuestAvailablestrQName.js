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
	if (quest.ownerVnums.length > 0) {
		/**
		 * Find masters where this quest is available
		 */
		for (var i = 0; i < quest.ownerVnums.length; i++) {
			var vnum = quest.ownerVnums[i];
			if (this.quest(vnum + '_BLOCK_QUESTS') == 1) {
				continue;				// Player has been blocked from this quest master
			}
			var mProto = getMobProto(vnum);
			var mobClans = mProto.findClans();
			var actorClans = this.findClans();
			if (quest.unlockRank > 0) {
				if (!this.clanCheck(mobClans)) {
					continue;
				}
				if (mobClans.length && this.getRank(mobClans[0]) < quest.unlockRank) {
					continue;			// Player is not correct clan or rank
				}
			}
			var self = mProto;
			if (quest.extraChecks.length > 0 && quest.extraChecks.every(checks) == false) {
				continue					// Player doesn't pass all extra checks with this master
			}
			availableMasters.push(vnum);
		}
	}
	else if (!quest.extraChecks.every(checks)) {
		available = false;			// Player doesn't pass all extra checks
	}
	/**
	 * If priorQuests.length == 0 || priorQuests[j] is complete, include this quest
	 **/
	for (var j = 0; j < quest.priorQuests.length; j++) {
		if (this.quest(quest.priorQuests[j]) != -1) {
			available = false;		// Quest is not unlocked, unavailable
		}
	}
	if (!arrContains(quest.allowedRaces, this.race)) {
		available = false;			// Player is not of a valid race
	}
	if (available == false) {
		availableMasters = [];		// If it isn't available for any reason, then there are no masters offering it
	}
	if (availableMasters.length == 0 && quest.ownerVnums.length > 0) {
		available = false;			// If no masters are offering, it isn't available
	}
	return [available || availableMasters.length > 0, availableMasters];
}
