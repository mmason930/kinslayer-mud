//Alder
//May 2010
/**************************************************************
 * strTitle: the title string to look for (Case sensitive)    *
 * Returns: true if player has a title which matches strTitle *
 *          exactly                                           *
 **************************************************************/
JSCharacter.prototype.checkTitle = function ( strTitle ) {
	var strTitle = strTitle+"";
	var titles = this.titles;
	for each ( var title in titles ) {
		if ( title.title == strTitle ) {
			return true;
		}
	}
	return false;
}
