//Alder
//May 2010
/****************************************************************
 * Returns: array of all players that are on (linkless included)*
 ****************************************************************/
function getCurrentPlayers() {
	function filterValid(element, index, array) {
		return ( element.isValid );
	}
	global.players = global.players.filter(filterValid);
	return ( global.players );
}

/****************************************************************
 * Returns: player whose name is sName (or null if none found)	*
 ****************************************************************/
function getPlayerByName(sName) {
	var players = getCurrentPlayers();
	var regex = new RegExp(sName, "i");
	for each (var p in players) {
		if (regex.test(p.name)) {
			return p;
		}
	}
	return null;
}

