JSCharacter.prototype.isMount = function()
{
	return this.mobFlagged(constants.MOB_MOUNT) || this.mobFlagged(constants.MOB_SHADOW_MOUNT) || this.mobFlagged(constants.MOB_OGIER_MOUNT);
}

JSCharacter.prototype.isHorse = JSCharacter.prototype.isMount;
