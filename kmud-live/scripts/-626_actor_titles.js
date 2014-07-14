//Alder
//May 2010
	/********************************************** 
	 * Retrieve a player's array of title objects *
	 **********************************************/
	JSCharacter.prototype.__defineGetter__( "titles", function() {
		var titles = [];
		var tempTitles = this.makeTitleArray();
		for ( var i = 1; i < tempTitles.length; i++ )
			titles.push(tempTitles[i]);
		return ( titles );
	} )
