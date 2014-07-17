//Alder
//April 2010
/*****************************************************************************
 * Returns: array of quest objects available at quest master with masterVnum *
 *****************************************************************************/
JSCharacter.prototype.getAvailableQuests = function ( masterVnum ) {
	if ( isNumber(masterVnum) == false ) {
		mudLog(3,103,"JS -616: NaN passed as masterVnum");
		return;
	}
	var mProto = getMobProto(masterVnum);
	var tempQuests = getQuestsByMob( masterVnum );
	var finalQuests = [];
	if ( masterVnum == -1 || this.quest(masterVnum+'_BLOCK_QUESTS') == 1 )
		return finalQuests;
	var push;
	for ( var i = 0; i < tempQuests.length; i++ ) {
		var quest = tempQuests[i];
		var aData = this.isQuestAvailable(quest.questName);
		push = aData[0];
		if ( push == true && arrContains(aData[1],masterVnum) ) {
			finalQuests.push(quest);
		}
	}
	return finalQuests;
}
