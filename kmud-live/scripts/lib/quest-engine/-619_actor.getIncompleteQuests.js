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
	for (var _autoKey in this.journal.entries) {
		var entry = this.journal.entries[_autoKey];
		var quest = entry.quest;
		if ( this.quest(quest.questName) > 0 ) {
			currQuests.push(getQuestByName(qName));
		}
	}
	return currQuests;
}

