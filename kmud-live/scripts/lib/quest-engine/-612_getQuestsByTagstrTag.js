//Alder
//April 2010
/***************************************************************
 * strTag: string which is compared with all quests' tags      *
 * actor: character to test quest availability with (optional) *
 * Returns: array of quest objects which are tagged with       *
 *          strTag 										       *
 ***************************************************************/
function getQuestsByTag( strTag, actor ) {
	var strTag = strTag+"";
	var viewer = undefined;
	if ( getObjectClass(actor) == "Array" ) {
		viewer = actor[0];
		actor = undefined;
	}
	var tQuests = [];
	for (var _autoKey in global.vQuests) {
		var quest = global.vQuests[_autoKey];
		if ( viewer != undefined && ( !arrContains(global.questMasters,viewer.name) || viewer.quest("QUEDIT_DISP_TOG") != 0 ) && !arrContains(quest.editors,viewer.name) ) {
			continue;
		}
		var qName = quest.name
		if ( checkTag(qName,strTag) == true ) {
			if ( actor && getObjectClass(actor) == 'JSCharacter' ) {
				var push = actor.isQuestAvailable(qName)[0];
				if ( push == true ) {
					tQuests.push(quest);
					continue;
				}
			}
			tQuests.push(quest);
		}
	}
	return tQuests;
}
