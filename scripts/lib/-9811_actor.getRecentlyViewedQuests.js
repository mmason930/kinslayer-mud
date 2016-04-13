//Alder
//August 2010
/** Returns: array of last five quest names actor has viewed **/
JSCharacter.prototype.getRecentlyViewedQuests = function () {
	var sval = getSval(getRoom(98),-9811,this.name+" RVQ");
	if ( sval ) {
		return sval;
	}
	else {
		return [];
	}
}
