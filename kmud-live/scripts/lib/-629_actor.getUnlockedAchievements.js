//Alder
//May 2010
/*******************************************************************************
 * Returns: objects for achievements unlocked by player                        *
 *******************************************************************************/
JSCharacter.prototype.getUnlockedAchievements = function () {
	var uAchs = [];
	for (var _autoKey in global.vAchievements) {
		var ach = global.vAchievements[_autoKey];
		if ( this.isAchUnlocked(ach.achVnum) ) {
			uAchs.push(ach);
		}
	}
	return ( uAchs );
}
