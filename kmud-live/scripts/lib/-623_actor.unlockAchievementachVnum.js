//Alder
//May 2010
/********************************************************************
 * achVnum: vnum of achievement to unlock                           *
 *																	*
 * bAlwaysExecute: optional boolean, if true, the achievement       *
 * executions will be evaluated for an already unlocked achievement *
 *																	*
 * Returns: array of elements common between fArr and sArr          *
 ********************************************************************/
JSCharacter.prototype.unlockAchievement = function(achVnum,bAlwaysExecute) {
	var ach = getAchByVnum(achVnum);
	if ( ach ) {
		getCharCols(this);
		var unlocked = this.isAchUnlocked(achVnum);
		if ( !unlocked ) {
			var cVal = this.quest("HIDE_ACH_TITLE_MSG");
			if ( !cVal ) {
				this.send(cyn+"Achievement Unlocked: "+nrm+bld+ach.achName+nrm+"\n");
				var achiever = this;
				function gechoMsg(player) {
					getCharCols(player);
					return (bld+grn+"**"+achiever.name+" has unlocked an achievement: "+ach.achName+"!**"+nrm);
				}
				gecho(gechoMsg);
				mudLog(3, Math.max(103,this.invis),this.name+" has unlocked achievement "+achVnum+": "+ach.achName+".");
			}
			this.addFeatPoints(ach.achFeatPoints);
			sqlQuery("INSERT INTO userAchievement (`player_name`, `achievement_vnum`) VALUES('"+sqlEsc(this.name)+"', '"+sqlEsc(achVnum)+"');");
		}
		else if ( ach.alwaysAwardFeatPoints ) {
			this.addFeatPoints(ach.achFeatPoints);
		}
		if ( !unlocked || bAlwaysExecute == true ) {
			var actor = this;
			var here = this.room;
			for each ( var exec in ach.achExecutions ) {
				eval(exec);
			}
		}
	}
	return;
}


