//Alder January 2010
/***************************************************
 * Returns true if actor belongs to any clans in   *
 * arrClanNums (-1 is unclanned). False otherwise. *
 ***************************************************/
JSCharacter.prototype.clanCheck = function( arrClanNums ) {
	if ( getObjectClass(arrClanNums) != "Array" ) {
		arrClanNums = [arrClanNums];
	}
	if ( !arrClanNums.length || arrClanNums[0] == -1 ) {
		return true;
	}
	var clanCheck = false
	var clans = this.findClans(true);
	for ( var i = 0; i < clans.length; i++ ) {
		var num = clans[i];
		if ( arrContains(arrClanNums,num) ) {
			clanCheck = true;
			break;
		}
	}
	return ( clanCheck );
}
