//Alder
//April 2010
/************************************************************************************************
 * strQName: name of quest to be compared with priorQuests of others                            *
 * Returns: array of quest objects unlocked by quest named strQName which the actor can receive *
 ************************************************************************************************/
JSCharacter.prototype.getUnlockedQuests = function ( strQName ) {
	var objQuest = getQuestByName(strQName);
	var tempQuests = [];
	var owners = objQuest.ownerVnum;
	// //Quests with same owner as this one
	// if ( owners != undefined && owners.length > 0 ) {
		// for (var _autoKey in owners) { 	var vnum = owners[_autoKey];
			// var theseQuests = this.getAvailableQuests(vnum);
			// for (var _autoKey in theseQuests) { 	var quest = theseQuests[_autoKey];
				// if ( arrContains(tempQuests,quest) == false )
					// tempQuests.push(quest);
			// }
		// }
	// }
	// var tags = objQuest.tags;
	// //Quests with same tag as this one
	// if ( tags != undefined && tags.length > 0 ) {
		// for (var _autoKey in tags) { 	var tag = tags[_autoKey];
			// var taggedQuests = getQuestsByTag(tag,this);
			// for (var _autoKey in taggedQuests) { 	var quest = taggedQuests[_autoKey];
				// if ( arrContains(tempQuests,quest) == false )
					// tempQuests.push(quest);
			// }
		// }
	// }
	tempQuests = global.vQuests;
	var unlockableQuests = [];
	var unlockedQuests = [];
	/***LOOP THROUGH QUESTS AND SEE IF COMPLETED QUEST IS A PRIOR QUEST OF ANYTHING***/
	for ( var i = 0; i < tempQuests.length; i++ ) {
		var priors = tempQuests[i].priorQuests;
		if ( priors.length == 0 ) {
			continue;
		}
		for ( var j = 0; j < priors.length; j++ ) {
			// This means completed quest is a requirement to unlock examined quest
			if ( priors[j] == objQuest.questName ) {
				unlockableQuests.push(tempQuests[i]);
			}
		}
	}
	/*********************************************************************************************************
	 * unlockedQuests: quests which have all prior quests completed           			                     *
	 * unlockQuest: quest from unlockableQuests array                                                        *
	 * unlockableQuests: quests in quest master's array which may be unlocked by completed quest             *
	 *********************************************************************************************************/
	if ( unlockableQuests.length > 0 ) {
		for ( var k = 0; k < unlockableQuests.length; k++ ) {
			unlockQuest = unlockableQuests[k];
			if ( this.isQuestAvailable(unlockQuest.questName)[0] == true ) {
				unlockedQuests.push(unlockQuest);
			}
		}
	}
	return unlockedQuests;
}
