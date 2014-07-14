//Alder
//May 2010
//Load player's title from the database
JSCharacter.prototype.loadTitlesFromDB = function() {
	var result = sqlQuery("SELECT * FROM userTitle WHERE user_id = " + this.id);
	var titles = [];
	while ( result.hasNextRow ) {
		var row = result.getRow;
		var title = new Object();
		title.title = row.get("title");
		title.type = row.get("type");
		var type = title.type;
		if ( type != "prefix" && type != "suffix" ) {
			title.type = "normal";
		}
		title.category = row.get("category");
		var cat = title.category;
		if ( !strn_cmp(cat,"pk",1) && !strn_cmp(cat,"clans",1) && !strn_cmp(cat,"quests",1) && !strn_cmp(cat,"trades",1) && !strn_cmp(cat,"silly",1) && !strn_cmp(cat,"imm",1) ) {
			title.category = "quests";
		}
		title.achVnum = parseInt(row.get("achievement_vnum"));
		titles.push(title);
	}
	// this.send(titles.length+" titles loaded from database");
	return titles;
}

