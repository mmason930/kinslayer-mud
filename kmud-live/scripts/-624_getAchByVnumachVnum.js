//Alder
//May 2010
function getAchByVnum(achVnum) {
	if ( !isNumber(achVnum) ) {
		mudLog(3,103,"JScript -624: NaN passed as achVnum");
		return false;
	}
	achVnum = Math.floor(achVnum);
	for each ( var ach in global.vAchievements ) {
		if ( ach.achVnum == achVnum ) {
			return ach;
		}
	}
	return false;
}
