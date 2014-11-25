//Alder
//April 2010
/*************************************************
 * Returns: array of all quest tags (no repeats) *
 *************************************************/
function getAllQuestTags(viewer) {
	var vTags = [];
	for (var _autoKey in global.vQuests) {
		var quest = global.vQuests[_autoKey];
		if ( viewer && viewer.quest("QUEDIT_DISP_TOG") != 0 && !arrContains(quest.editors, viewer.name)) {
			continue;
		}
		for (var _autoKey in quest.tags) {
			var tag = quest.tags[_autoKey];
			if ( arrContains(vTags,tag) == false ) {
				vTags.push(tag);
			}
		}
	}
	vTags.sort();
	return vTags;
}
