//Alder
//May 2010
JSCharacter.prototype.lockTitle = function( strTitle, strType, achVnum ) {
	if ( this.titles.length ) {
		// getRoom(98).echo("Has titles");
		var tObj = new Object();
		tObj.title = strTitle;
		tObj.type = strType.toLowerCase();
		var type = tObj.type;
		if ( type != "prefix" && type != "suffix" ) {
			tObj.type = "normal";
		}
		tObj.achVnum = (achVnum != undefined ? achVnum : 0);
		var titles = accessTitles(this.id);
		var length = titles.length;
		for ( var i = 1; i < length; i++ ) {
			var title = titles[i];
			// getRoom(98).echo("'"+title.title+"' == '"+tObj.title+"'");
			// getRoom(98).echo("'"+title.type+"' == '"+tObj.type+"'");
			// getRoom(98).echo("'"+title.achVnum+"' == '"+tObj.achVnum+"'");
			if ( title.title == tObj.title && title.type == tObj.type && title.achVnum == tObj.achVnum ) {
				titles.splice(i,1);
				sqlQuery("DELETE FROM userTitle WHERE user_id=" + this.id + " && title = " + sqlEscapeQuoteString(title.title) + " AND type='"+sqlEsc(title.type)+"' AND achievement_vnum='"+title.achVnum+"';");
				mudLog(3, Math.max(103,this.invis),capFirstLetter(strType)+" title  <"+strTitle+"> locked for "+this.name+".");
				return;
			}
		}
	}
	return;
}


