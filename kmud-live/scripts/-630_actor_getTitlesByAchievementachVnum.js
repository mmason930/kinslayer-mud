//Alder
//May 2010
/*******************************************************************************
 * Returns: titles earned through achievement with achVnum                     *
 *******************************************************************************/
JSCharacter.prototype.getTitlesByAchievement = function(achVnum) {
	var titles = this.titles;
	var achTitles = [];
	for ( var i = 0; i < titles.length; i++ ) {
		if ( achVnum == titles[i].achVnum ) {
			achTitles.push(titles[i]);
		}
	}
	return ( achTitles );
}
