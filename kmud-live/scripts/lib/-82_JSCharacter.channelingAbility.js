JSCharacter.prototype.channelingAbility = function() {
	if( this.class == constants.CLASS_DREADLORD || this.class == constants.CLASS_CHANNELER || this.class == constants.CLASS_DREADGUARD )
		return true;
	return false;
}
