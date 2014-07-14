//Alder
//May 2010
//Sends a selection message and sets player's title to supplied title
JSCharacter.prototype.setSpecTitle = function(objTitle) {
	getCharCols(this);
	this.remSpecTitle(false);
	this.send(cyn+bld+"You've selected the "+objTitle.type+" title, "+objTitle.title+"."+nrm);
	var query = sqlQuery("UPDATE userTitle SET selected="+sqlEsc(1)+" WHERE user_id="+this.id+" AND title='"+sqlEsc(objTitle.title)+"' && type='"+sqlEsc(objTitle.type)+"' && category='"+sqlEsc(objTitle.category)+"' && achievement_vnum='"+sqlEsc(objTitle.achVnum)+"';");
	//Monitoring (Alder)
	var alder = global.alder;
	if ( alder ) {
		getCharCols(alder);
		alder.send(grn+"[ "+this.name+" selected "+objTitle.type+" title "+objTitle.title+". ]"+nrm);
	}
	//******************
	if ( query )
		return true;
	return false;
}

