//Alder
//May 2010
/**********************************************************
 * sOperation                                             *
 *   "clear" - removes player's title array from game     *
 * Not supplying sOperation will return a writable        *
 * version of player's title array if it exists           *
 * NOTE: if you are just trying to see a player's titles, *
 *       use 'actor.titles', not this function.           *
 **********************************************************/
accessTitles = function ( playerID, sOperation ) {
	var clear = strn_cmp(sOperation,"clear",1);
	var id = playerID;
	var gTitles = global.playerTitleArrays;
	var len = gTitles.length;
	for ( var i = 0; i < len; i++ ) {
		var array = gTitles[i];
		if ( array[0] == id ) {
			if ( clear ) {
				getRoom(98).echo("Titles cleared: ID "+id);
				global.playerTitleArrays.splice(i,1);
				return true;
			}
			else {
				// getRoom(98).echo("Found: "+array);
				return array;
			}
		}
	}
	return false;
}
