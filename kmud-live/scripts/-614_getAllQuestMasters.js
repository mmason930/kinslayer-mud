//Alder
//April 2010
/*********************************************************
 * Returns: array of all quest master names (no repeats) *
 *********************************************************/
function getAllQuestMasters( viewer ) {
	var vMasters = [];
	for each ( var quest in global.vQuests ) {
		if ( viewer && ( !arrContains(global.questMasters,viewer.name) || viewer.quest("QUEDIT_DISP_TOG") != 0 ) && !arrContains(quest.editors,viewer.name) ) {
			continue;
		}
		if ( getObjectClass(quest.ownerVnum) != "Array" ) {
			var vnum = quest.ownerVnum;
			var name = getMobName(vnum);
			if ( vnum > -1 && arrContains(vMasters,name) == false ) {
				vMasters.push(name);
				continue;
			}
		}
		for each ( var vnum in quest.ownerVnum ) {
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
