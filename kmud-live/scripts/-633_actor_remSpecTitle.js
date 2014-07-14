//Alder
//May 2010
JSCharacter.prototype.remSpecTitle = function(bShowMsg) {
	var title = this.getSpecTitle();
	if ( title ) {
		getCharCols(this,constants.CL_OFF);
		if ( bShowMsg != false )
			this.send(cyn+bld+"You have deselected your special title."+nrm);
		sqlQuery("UPDATE userTitle SET selected="+sqlEsc(0)+" WHERE user_id="+this.id+" && title=\""+sqlEsc(title.title)+"\" && type='"+sqlEsc(title.type)+"' && category='"+sqlEsc(title.category)+"' && achievement_vnum='"+sqlEsc(title.achVnum)+"' && selected='"+sqlEsc(1)+"';");
	}
}

