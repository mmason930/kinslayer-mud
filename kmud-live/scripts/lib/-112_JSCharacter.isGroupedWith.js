JSCharacter.prototype.isGroupedWith =
function( groupedCharacter ) {
	if( this.leader == groupedCharacter )
		return true;
	var followers = this.followers;
	for(var followerIndex = 0;followerIndex < followers.length;++followerIndex) {
		if( followers[ followerIndex ] == groupedCharacter )
			return true;
	}
	return false;
}
