JSObject.prototype.__defineGetter__("weaponType", function() {
	if( this.type != constants.ITEM_WEAPON )
		return -1;
	return this.value(constants.VALUE_WEAPON_TYPE);
});
