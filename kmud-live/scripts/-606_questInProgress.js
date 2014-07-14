JSCharacter.prototype.questInProgress = function( qName )
{
	return ( this.quest(qName) > 0 );
}
