//Alder
//May 2010
JSCharacter.prototype.isAchUnlocked = function ( achVnum ) {
	return ( sqlQuery("SELECT * FROM userAchievement WHERE player_name='"+sqlEsc(this.name)+"' && achievement_vnum="+sqlEsc(achVnum)+";").hasNextRow );
}	

