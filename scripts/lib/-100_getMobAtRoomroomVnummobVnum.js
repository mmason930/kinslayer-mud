//Alder December 2009
/**This returns JSCharacter with vnum mobVnum at room roomVnum. Null otherwise.**/
function getMobAtRoom(roomVnum,mobVnum) {
	var people = getRoom(roomVnum).people;
	for ( var i = 0; i < people.length; i++ ) {
		if ( people[i].vnum == mobVnum )
			return people[i];
	}
	return null;
}
