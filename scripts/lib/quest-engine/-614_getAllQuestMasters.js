//Alder
//April 2010
/*********************************************************
 * Returns: array of all quest master names (no repeats) *
 *********************************************************/
function getAllQuestMasters( viewer ) {
	var vMasters = [];
	for (var _autoKey in global.vQuests) {
		var quest = global.vQuests[_autoKey];
		if (viewer && viewer.quest("QUEDIT_DISP_TOG") != 0 && !arrContains(quest.editors, viewer.name)) {
			continue;
		}
		if ( getObjectClass(quest.ownerVnums) != "Array" ) {
			var vnum = quest.ownerVnums;
			var name = getMobName(vnum);
			if ( vnum > -1 && arrContains(vMasters,name) == false ) {
				vMasters.push(name);
				continue;
			}
		}
		for (var _autoKey in quest.ownerVnums) {
			var vnum = quest.ownerVnums[_autoKey];
			var name = getMobName(vnum);
			if ( arrContains(vMasters,name) == false && vnum > -1 ) {
				vMasters.push(name);
				continue;
			}
		}
	}
	vMasters.sort();
	return vMasters;
}
