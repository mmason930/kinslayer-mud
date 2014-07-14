//	June 2010 - Fogel
//	Returns if a character is clanned
JSCharacter.prototype.isClanned = function() 
{
	for( var i = 0; i < constants.CLANS_MAX; i++ )
		if( this.inClan(i) )
			return true;
	return false;
}
