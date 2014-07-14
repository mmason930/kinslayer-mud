// Returns an array of all the connected players within a zone with the specified vnum
function getPlayersInZone(zone) {
	var players = new Array();
	for each(var person in getConnectedPlayers()) {
		if( person.room.zoneVnum == zone ) {
			players.push(person);
		}
	}
	return players;
}
