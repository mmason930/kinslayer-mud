//Alder January 2010
/**************************************************
 * actor.findClans()                              *
 * Args: if returnArray is true, the function     *
 * will always return an array.                   *
 * Returns: if actor is in one clan, returns that *
 * clan number. If they are in multiple, returns  *
 * array of clan numbers.                         *
 * If actor is unclanned, returns -1              *
 *                                                *
 **************************************************/
JSCharacter.prototype.findClans = function(returnArray) {
	var clanNumArray = [];
	for ( var i = 1; i <= 30; i++ ) {
		if ( this.inClan(i) == true ) {
			clanNumArray.push(i);
		}
	}
	if ( !clanNumArray.length ) {
		return -1;
	}
	if( returnArray ) {
		return clanNumArray;
	}
	return ( clanNumArray.length > 1 && this.vnum == -1 ? clanNumArray : clanNumArray[0] );
}	
