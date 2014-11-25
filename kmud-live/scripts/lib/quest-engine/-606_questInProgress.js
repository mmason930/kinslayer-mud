JSCharacter.prototype.questInProgress = function( qName )
{
	return Quest.getByName(qName).hasBegun(this);
}
