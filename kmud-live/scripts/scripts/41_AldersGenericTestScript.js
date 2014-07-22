var alderTest = function(self, actor, here, args, extra) {
	// actor.lockAchievement(53);
	// actor.lockAchievement(54);
	// actor.lockTitle("Ghob'hlin","normal",53);
	// actor.lockTitle("Ghob'hlin Council","normal",54);
	// sqlQuery("UPDATE userTitle SET category='trades' WHERE achievement_vnum=0;");
	// sqlQuery("UPDATE userTitle SET category='clans' WHERE achievement_vnum=53 || achievement_vnum=54;");
	// sqlQuery("DELETE FROM pvals WHERE sKey='Special Title';");
	// var query = sqlQuery("UPDATE userTitle SET selected="+sqlEsc(0)+";");
	// if ( query )
		// here.echo(true);
	// // else
		// // here.echo(false);
	// global.playerTitleArrays = [];
	// here.echo(global.playerTitleArrays.length);
	// for (var _autoKey in getCurrentPlayers()) { 	var player = getCurrentPlayers()[_autoKey];
		// var title = player.getSpecTitle();
		// if ( title ) {
			// actor.send(player.name+": ");
			// actor.send(title.title+" "+title.type+" "+title.category+" "+title.achVnum);
		// }
	// }
	// var uAchs = sqlQuery("SELECT * FROM quests WHERE quest_name LIKE 'Achievement_%';");
	// while ( uAchs.hasNextRow ) {
		// var row = uAchs.getRow;
		// if ( parseInt(row.get("value")) == 1 ) {
			// var vnum = row.get("quest_name").split("_")[1];
			// var name = row.get("name");
			// sqlQuery("INSERT INTO userAchievement (`player_name`, `achievement_vnum`) VALUES('"+sqlEsc(name)+"', '"+sqlEsc(vnum)+"');");
		// }
	// }
	// var unlocked = sqlQuery("SELECT * FROM userAchievement WHERE player_name='"+sqlEsc(actor.name)+"' && achievement_vnum="+sqlEsc(73)+";").hasNextRow;
	// here.echo(unlocked);
	
	global.alder.send(this.name);
}