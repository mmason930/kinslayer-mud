// Alder
// June 2011
/***************************************************************
 * Parameter 1: String or Array of Strings representing the    *
 * names of scalps to look for                                 *
 * NOTE: use the mob's name, not the name of the scalp. EX:    *
 * If you want to check for Easar's scalp, you need to pass    *
 * "King Easar", NOT "a bloody scalp of King Easar"            *
 * Parameter 2: desired race constant of returned scalps       *
 * Returns: Array of scalp objects that match the parameter    *
 ***************************************************************/
JSCharacter.prototype.findScalpsByNameAndRace = function ( names, scalpRace ) {
	if ( !Array.isArray(names) ) {
		names = [names];
	}
	if ( isString(scalpRace) ) {
		scalpRace = parseRace(scalpRace);
	}
	var scalpNames = [];
	for each ( var name in names ) {
		scalpNames.push("a bloody scalp of "+name);
	}
	var matchScalps = [];
	var items = this.getAllItems();
	for each ( var item in items ) {
		if ( arrContains(scalpNames,item.name) && item.scalpRace == scalpRace ) {
			matchScalps.push(item);
		}
	}
	return matchScalps;
}

