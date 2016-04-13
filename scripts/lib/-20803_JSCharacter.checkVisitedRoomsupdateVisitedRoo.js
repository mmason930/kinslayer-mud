//Koradin - May 2010
//checks an array of rooms that the character has already visited for a certain task
JSCharacter.prototype.checkVisitedRooms = function ( strQuestName_strTaskName )
{
	var roomString = this.room.vnum.toString();
	var zone = roomString.substr( 0, roomString.length-2);
	var room = roomString.substr(roomString.length - 2, 2);
	var pval = this.getPval(strQuestName_strTaskName);
	var result = [];
	if ( pval == null )
		return false;
	var pval_array = pval.split("~");
	var pval_zone = pval_array.shift();
	for (var _autoKey in pval_array) {
		var pval_room = pval_array[_autoKey];
		result.push( parseInt(pval_zone+pval_room) );

	}
	function fSort(a,b) {
		return ( a - b );
	}
	result.sort(fSort);
	if ( arrContains(result,parseInt(zone+room)) == true )
		return true;
	else
		return false;
}
//updates array of rooms that the character has visited for a task
JSCharacter.prototype.updateVisitedRooms = function ( strQuestName_strTaskName )
{
	var roomString = this.room.vnum.toString();
	var zone = roomString.substr( 0, roomString.length-2);
	var room = roomString.substr(roomString.length - 2, 2);
	if ( this.getPval(strQuestName_strTaskName) )
		this.setPval( strQuestName_strTaskName, this.getPval(strQuestName_strTaskName) + "~" + room, true );
	else
		this.setPval( strQuestName_strTaskName, zone + "~" + room, true );
}
