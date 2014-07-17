//Alder
//February 2011
/**************************************************************
 * JSCharacter.findPlayerScalps ( scalpRace, minLevel )       *
 * scalpRace: integer or string representing race of scalp    *
 * minLevel: integer representing minimum level of scalp      *
 * Returns: array of scalp objects matching given criteria    *
 **************************************************************/
 
 JSCharacter.prototype.findPlayerScalps = function( scalpRace, minLevel ) {
	// If scalpRace is given as String, convert to number
	if ( getObjectClass(scalpRace) == "String" ) {
		scalpRace = parseRace(scalpRace,true);
	}
	// If minLevel isNaN, display error
	if ( !isNumber(minLevel ) ) {
		mudLog(constants.CMP,102,"Value passed as minLevel isNaN: JSCharacter.findPlayerScalps() :: JS -9800");
		return null;
	}
	minLevel = parseInt(minLevel);
	
	var matchingScalps = [];				// Holds scalps that match parameters
	var possessions = this.getAllItems();	// Every item on player
	
	for each ( var item in possessions ) {
		if ( item.isPlayerScalp && item.scalpRace == scalpRace && item.scalpLevel >= minLevel ) {
			matchingScalps.push(item);		// This item is a scalp matching specified criteria
		}
	}
	return matchingScalps;
}

