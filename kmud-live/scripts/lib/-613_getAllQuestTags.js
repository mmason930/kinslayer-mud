//Alder
//April 2010
/*************************************************
 * Returns: array of all quest tags (no repeats) *
 *************************************************/
function getAllQuestTags(viewer) {
	var vTags = [];
	for each ( var quest in global.vQuests ) {
		if ( viewer && ( !arrContains(global.questMasters,viewer.name) || viewer.quest("QUEDIT_DISP_TOG") != 0 ) && !arrContains(quest.editors,viewer.name) ) {
			continue;
		}
		for each ( var tag in quest.tags ) {
			if ( arrContains(vTags,tag) == false ) {
				vTags.push(tag);
			}
		}
	}
	vTags.sort();
	return vTags;
}
