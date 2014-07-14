// Alder
// May 2011
/*********************************************************
 * Converts the number constant associated with a given  *
 * equipment position to a string representation.        *
 * Returns -1 if parameter is not an equipment position. *
 *********************************************************/
 
function wearPosToString( constant ) {
	if ( constant == constants.ITEM_WEAR_FINGER )
		return "FINGER";
	if ( constant == constants.ITEM_WEAR_NECK )
		return "NECK";
	if ( constant == constants.ITEM_WEAR_BODY )
		return "BODY";
	if ( constant == constants.ITEM_WEAR_HEAD )
		return "HEAD";
	if ( constant == constants.ITEM_WEAR_LEGS )
		return "LEGS";
	if ( constant == constants.ITEM_WEAR_FEET )
		return "FEET";
	if ( constant == constants.ITEM_WEAR_HANDS )
		return "HANDS";
	if ( constant == constants.ITEM_WEAR_ARMS )
		return "ARMS";
	if ( constant == constants.ITEM_WEAR_SHIELD )
		return "SHIELD";
	if ( constant == constants.ITEM_WEAR_ABOUT )
		return "ABOUT";
	if ( constant == constants.ITEM_WEAR_WAIST )
		return "WAIST";
	if ( constant == constants.ITEM_WEAR_WRIST )
		return "WRIST";
	if ( constant == constants.ITEM_WEAR_WIELD )
		return "WIELD";
	if ( constant == constants.ITEM_WEAR_BACK )
		return "BACK";
	if ( constant == constants.ITEM_WEAR_SHOULDERS )
		return "SHOULDERS"
	if ( constant == constants.ITEM_WEAR_EAR )
		return "EAR";
	if ( constant == constants.ITEM_WEAR_HOLD )
		return "HOLD";
	if ( constant == constants.ITEM_WEAR_TAKE )
		return "TAKE";
	return -1;
	
}
