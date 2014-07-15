//Alder
//April 2010
/**************************************************************************
 * strMaster: vnum which is compared with all quests' ownerVnum           *
 * Returns: array of quest objects which are owned by mob named strMaster *
 **************************************************************************/
function getQuestsByMasterName( strMaster ) {
	var tQuests = [];
	for each ( var quest in global.vQuests ) {
		for each ( var vnum in quest.ownerVnum ) {
			if ( vnum > -1 && getMobName(vnum) == strMaster && !arrContains(tQuests,quest) ) {
				tQuests.push(quest);
			}
		}
	}
	return tQuests;
}
