//Alder
//April 2010
/*****************************************************************************
 * Returns: array of quest objects available at quest master with masterVnum *
 *****************************************************************************/
JSCharacter.prototype.getAvailableQuests = function (masterVnum) {
	var actor = this;
	var self = getMobProto(masterVnum);
	var masterQuests = Quest.getByMaster(masterVnum);
	return masterQuests.filter(function(quest) {
		return quest.canAccess(actor);
	});
}
