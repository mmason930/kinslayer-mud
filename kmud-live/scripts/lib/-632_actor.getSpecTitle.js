//Alder
//May 2010
/*****************************************************************
 * Returns: title object with properties: title, type, if actor  *
 * has a special title set, if not this returns false            *
 *****************************************************************/
JSCharacter.prototype.getSpecTitle = function() {
	var str = sqlQuery("SELECT * FROM userTitle WHERE user_id="+this.id+" AND selected='"+sqlEsc(1)+"';");
	if ( str.hasNextRow ) {
		var row = str.getRow;
		var tObj = new Object();
		tObj.title = row.get("title");
		tObj.type = row.get("type");;
		tObj.category = row.get("category");
		tObj.achVnum = row.get("achievement_vnum");
		tObj.selected = row.get("selected");
		return ( tObj );
	}
	return ( false );
}

