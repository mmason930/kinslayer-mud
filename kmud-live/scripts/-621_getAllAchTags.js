//Alder
//May 2010
/*******************************************************
 * Returns: array of all achievement tags (no repeats) *
 *******************************************************/
function getAllAchTags() {
	var vTags = [];
	for each ( var ach in global.vAchievements ) {
		for each ( var tag in ach.achTags ) {
			if ( arrContains(vTags,tag) == false ) {
				vTags.push(tag);
			}
		}
	}
	return vTags;
}
