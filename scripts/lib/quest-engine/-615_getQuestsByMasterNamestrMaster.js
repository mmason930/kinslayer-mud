//Alder
//April 2010
/**************************************************************************
 * strMaster: vnum which is compared with all quests' ownerVnums           *
 * Returns: array of quest objects which are owned by mob named strMaster *
 **************************************************************************/
function getQuestsByMasterName( strMaster ) {
	var tQuests = [];
	for (var _autoKey in global.vQuests) {
		var quest = global.vQuests[_autoKey];
		for (var _autoKey in quest.ownerVnums) {
			var vnum = quest.ownerVnums[_autoKey];
			if ( vnum > -1 && getMobName(vnum) == strMaster && !arrContains(tQuests,quest) ) {
				tQuests.push(quest);
			}
		}
	}
	return tQuests;
}
