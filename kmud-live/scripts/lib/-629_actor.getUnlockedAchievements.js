//Alder
//May 2010
/*******************************************************************************
 * Returns: objects for achievements unlocked by player                        *
 *******************************************************************************/
JSCharacter.prototype.getUnlockedAchievements = function () {
	var uAchs = [];
	for each ( var ach in global.vAchievements ) {
		if ( this.isAchUnlocked(ach.achVnum) ) {
			uAchs.push(ach);
		}
	}
	return ( uAchs );
}
