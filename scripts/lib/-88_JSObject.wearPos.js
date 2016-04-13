// Alder
// May 2011
/*********************************************************
 * Returns the position constant at which the item can   *
 * be worn. Returns -1 if item is not armor or cannot    *
 * be worn.                                              *
 *********************************************************/
 
JSObject.prototype.__defineGetter__("wearPos", function() {
	if( this.type != constants.ITEM_ARMOR )
		return -1;
	if ( this.canWear(constants.ITEM_WEAR_FINGER) )
		return constants.ITEM_WEAR_FINGER;
	if ( this.canWear(constants.ITEM_WEAR_NECK) )
		return constants.ITEM_WEAR_NECK;
	if ( this.canWear(constants.ITEM_WEAR_BODY) )
		return constants.ITEM_WEAR_BODY;
	if ( this.canWear(constants.ITEM_WEAR_HEAD) )
		return constants.ITEM_WEAR_HEAD;
	if ( this.canWear(constants.ITEM_WEAR_LEGS) )
		return constants.ITEM_WEAR_LEGS;
	if ( this.canWear(constants.ITEM_WEAR_FEET) )
		return constants.ITEM_WEAR_FEET;
	if ( this.canWear(constants.ITEM_WEAR_HANDS) )
		return constants.ITEM_WEAR_HANDS;
	if ( this.canWear(constants.ITEM_WEAR_ARMS) )
		return constants.ITEM_WEAR_ARMS;
	if ( this.canWear(constants.ITEM_WEAR_SHIELD) )
		return constants.ITEM_WEAR_SHIELD;
	if ( this.canWear(constants.ITEM_WEAR_ABOUT) )
		return constants.ITEM_WEAR_ABOUT;
	if ( this.canWear(constants.ITEM_WEAR_WAIST) )
		return constants.ITEM_WEAR_WAIST;
	if ( this.canWear(constants.ITEM_WEAR_WRIST) )
		return constants.ITEM_WEAR_WRIST;
	if ( this.canWear(constants.ITEM_WEAR_WIELD) )
		return constants.ITEM_WEAR_WIELD;
	if ( this.canWear(constants.ITEM_WEAR_BACK) )
		return constants.ITEM_WEAR_BACK;
	if ( this.canWear(constants.ITEM_WEAR_SHOULDERS) )
		return constants.ITEM_WEAR_SHOULDERS;
	if ( this.canWear(constants.ITEM_WEAR_EAR) )
		return constants.ITEM_WEAR_EAR;
	if ( this.canWear(constants.ITEM_WEAR_HOLD) )
		return constants.ITEM_WEAR_HOLD;
	if ( this.canWear(constants.ITEM_WEAR_TAKE) )
		return constants.ITEM_WEAR_TAKE;
	return -1;
	
});
