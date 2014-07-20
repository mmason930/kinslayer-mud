//Alder
//May 2010
/*******************************************************
 * Returns: array of all achievement tags (no repeats) *
 *******************************************************/
function getAllAchTags() {
	var vTags = [];
	for (var _autoKey in global.vAchievements) {
		var ach = global.vAchievements[_autoKey];
		for (var _autoKey in ach.achTags) {
			var tag = ach.achTags[_autoKey];
			if ( arrContains(vTags,tag) == false ) {
				vTags.push(tag);
			}
		}
	}
	return vTags;
}
