//Alder
//May 2010
/**************************************************************
 * Returns: true if actor is in void, false if not            *
 **************************************************************/
JSCharacter.prototype.isIdle = function() {
	if ( this.idle >= 5 )
		return true;
	return false;
}
