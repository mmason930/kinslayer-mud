//Alder
//May 2010
//Sends an unlock message and qvals achievement to 1 for actor
JSCharacter.prototype.lockAchievement = function(achVnum) {
	var ach = getAchByVnum(achVnum);
	if ( ach ) {
		getCharCols(this);
		if ( this.isAchUnlocked(achVnum) ) {
			var cVal = this.quest("HIDE_ACH_TITLE_MSG");
			if ( !cVal && this.name == "Alder" )
				this.send(cyn+"Achievement locked: "+nrm+bld+ach.achName+nrm+"\n");
			sqlQuery("DELETE FROM userAchievement WHERE player_name='"+sqlEsc(this.name)+"' && achievement_vnum="+sqlEsc(achVnum)+";");
			for (var _autoKey in this.getTitlesByAchievement(achVnum)) {
				var title = this.getTitlesByAchievement(achVnum)[_autoKey];
				// this.send("LOCKING A TITLE IN lockAchievement");
				// this.send("'"+title.title+"'");
				// this.send("'"+title.type+"'");
				// this.send("'"+title.achVnum+"'");
				this.lockTitle(title.title,title.type,title.achVnum);
			}
		}
	}
	return;
}

