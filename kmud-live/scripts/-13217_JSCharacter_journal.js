//Alder January 2010
//Do not alter    
	/********************************************** 
	 * Retrieve a player's virtual journal object *
	 **********************************************/
	JSCharacter.prototype.__defineGetter__( "journal", function() {
		for each( var journal in globJournals ) {
			if ( journal.ownerID == this.id )
				return ( journal );
		}
		return false;
	} )
