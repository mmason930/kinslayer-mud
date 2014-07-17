JSCharacter.prototype.isAesSedai =
function()
{
	var isAesSedai = false;
	for(var clanNum = 17;clanNum <= 23;++clanNum) {
		if( !this.inClan( clanNum ) ) {
			isAesSedai = true;
			break;
		}
	}
	return isAesSedai;
}
