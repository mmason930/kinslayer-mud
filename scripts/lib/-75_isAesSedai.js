JSCharacter.prototype.isAesSedai =
function()
{
	var isAesSedai = false;
	//Clans 17-23 are the seven Ajahs. Membership in any one of them makes the character
	//Aes Sedai, so stop at the first match.
	for(var clanNum = 17;clanNum <= 23;++clanNum) {
		if( this.inClan( clanNum ) ) {
			isAesSedai = true;
			break;
		}
	}
	return isAesSedai;
}
