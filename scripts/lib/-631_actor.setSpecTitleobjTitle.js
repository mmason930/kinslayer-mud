//Alder
//May 2010
//Sends a selection message and sets player's title to supplied title
JSCharacter.prototype.setSpecTitle = function(objTitle) {
	getCharCols(this);
	this.remSpecTitle(false);
	this.send(cyn+bld+"You've selected the "+objTitle.type+" title, "+objTitle.title+"."+nrm);
	var sql = "UPDATE userTitle SET selected=" + sqlEsc(1) + " WHERE user_id=" + this.id + " AND title='" + sqlEsc(objTitle.title) + "' AND type='" + sqlEsc(objTitle.type) + "' AND category='" + sqlEsc(objTitle.category) + "' AND achievement_vnum='" + sqlEsc(objTitle.achVnum) + "';";
	//global.alder.send(sql);
	var query = sqlQuery(sql);
	//Monitoring (Alder)

	/***
	var alder = global.alder;
	if ( alder ) {
		getCharCols(alder);
		alder.send(grn+"[ "+this.name+" selected "+objTitle.type+" title "+objTitle.title+". ]"+nrm);
	}
	 ***/
	//******************
	if ( query )
		return true;
	return false;
}

