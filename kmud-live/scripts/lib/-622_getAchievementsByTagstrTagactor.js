//Alder
//May 2010
/*********************************************************************
 * strTag: string which is compared with all achievement's tags      *
 * actor: character to test achievement availability with (optional) *
 * Returns: array of achievement objects which are tagged with       *
 *          strTag 										             *
 *********************************************************************/
function getAchievementsByTag( strTag, actor ) {
	var strTag = strTag+"";
	var tAchs = [];
	for each ( var ach in global.vAchievements ) {
		var vnum = ach.achVnum;
		if ( arrContains(ach.achTags,strTag) == true ) {
			if ( actor && getObjectClass(actor) == 'JSCharacter' ) {
				if ( actor.isAchUnlocked(achVnum) ) {
					tAchs.push(ach);
					continue;
				}
			}
			tAchs.push(ach);
		}
	}
	return tAchs;
}
