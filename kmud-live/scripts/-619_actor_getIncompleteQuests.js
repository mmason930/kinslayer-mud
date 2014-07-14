//Alder
//May 2010
/*********************************************************
 * Returns: array of quest objects which the actor is on *
 *********************************************************/
JSCharacter.prototype.getIncompleteQuests = function() {
	if ( !this.journal ) {
		this.recreateJournal();
	}
	var currQuests = [];
	for each ( var entry in this.journal.entries ) {
		var quest = entry.quest;
		if ( this.quest(quest.questName) > 0 ) {
			currQuests.push(getQuestByName(qName));
		}
	}
	return currQuests;
}

