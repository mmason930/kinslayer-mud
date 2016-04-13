//Alder
//May 2010
function getAchByVnum(achVnum) {
	if ( !isNumber(achVnum) ) {
		mudLog(3,103,"JScript -624: NaN passed as achVnum");
		return false;
	}
	achVnum = Math.floor(achVnum);
	for (var _autoKey in global.vAchievements) {
		var ach = global.vAchievements[_autoKey];
		if ( ach.achVnum == achVnum ) {
			return ach;
		}
	}
	return false;
}
