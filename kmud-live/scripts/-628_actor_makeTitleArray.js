//Alder
//May 2010
//Add to global array of player title arrays
JSCharacter.prototype.makeTitleArray = function() {
	for each ( var array in global.playerTitleArrays ) {
		if ( array[0] == this.id ) {
			// this.send("Your title array exists globally.");
			return ( array );
		}
	}
	// this.send("Loading your titles from database and adding to global array.");
	var titleArray = [this.id].concat(this.loadTitlesFromDB());
	global.playerTitleArrays.push(titleArray);
	function sortByID(a,b) {
		return a[0] - b[0];
	}
	global.playerTitleArrays.sort(sortByID);
	return ( titleArray );
}
