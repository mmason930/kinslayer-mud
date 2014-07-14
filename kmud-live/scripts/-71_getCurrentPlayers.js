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
