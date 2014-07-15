// Alder
// June 2011
// Personal Storage Utilities
/****************************************
 *            Get Stored Items          *
 ****************************************/
JSCharacter.prototype.getStoredItems = function () {
	return loadObjectsByHolderFromDatabase("S", this.id);
}
/****************************************
 *        Get Free Storage Slots        *
 ****************************************/
JSCharacter.prototype.countEmptyStorageSlots = function () {
	var pval = this.getPval("storageSlots");
	if ( pval == null ) {
		return 0;
	}
	return parseInt(pval);
}
/****************************************
 *              Store Item              *
 ****************************************/
JSCharacter.prototype.storeItem = function ( item ) {
	if ( getObjectClass(item) != "JSObject" ) {
		return false;
	}
	var emptySlots = this.countEmptyStorageSlots();
	if ( emptySlots ) {
		saveSingleObjectToDatabase(item,"S",this.id);	// Save object into storage holder in database
		item.extract();
		this.setPval("storageSlots", emptySlots-1, true);
		this.comm("save");
		return true;
	}
	return false;
}
/****************************************
 *           Get Storage Price          *
 ****************************************/
getStoragePrice = function ( item ) {
	if ( getObjectClass(item) != "JSObject" ) {
		return -1; //"That is not an item."
	}
	if ( item.contents.length ) {
		return -2; //"You must first remove the contents of "+item.name+" before I can give you a price."
	}
	if ( isName( "masterweapon", item.namelist ) ) {
		return -3; //"You cannot store "+item.name+"."
	}
	var cost = 250;				// Base cost
	const weapMult = 3;			// Cost multiplier for weapon
	const armorMult = 2;		// Cost multiplier for armor
	const absCost = 5;			// Each absorption point costs this amount of copper
	const dbCost = 50;			// Each dodge point costs this amount of copper (Negative db is not deducted)
	const pbCost = 20;			// Each parry point costs this amount of copper (Negative db is not deducted
	const retoolMult = 1.5;		// Cost multiplier for retooled item
	const weightThreshold = 6;	// Anything over this weight will cost extra
	const weightCost = 80;		// Each pound over weightThreshold costs this amount of copper
	const isRareMult = 4;		// Cost multiplier for RARE item
	const rarityThreshold = 65;	// Any item with less instances than this is considered rare
	const rarityMult = 1.5;		// Cost multiplier for an item within rarityThreshold
	
	//Weight
	if ( item.weight > weightThreshold ) {
		cost += ( (item.weight - weightThreshold)*weightCost );
	}
	//Abs
	if ( item.abs > 0 ) {
		cost += (item.abs*absCost);
	}
	//DB
	if ( item.dodge > 0 ) {
		cost += (item.dodge*dbCost);
	}
	//PB
	if ( item.parry > 0 ) {
		cost += (item.parry*pbCost);
	}
	//Weapon
	if ( item.type == constants.ITEM_WEAPON ) {
		cost *= weapMult;
	}
	//Armor
	if ( item.type == constants.ITEM_ARMOR ) {
		cost *= armorMult;
	}
	//Retooled
	if ( item.isRetooled() ) {
		cost *= retoolMult;
	}
	//Rare
	if ( item.extraFlags(constants.ITEM_RARE) ) {
		cost *= isRareMult;
	}
	//Rarity
	if ( item.count <= rarityThreshold ) {
		cost *= rarityMult;
	}
	return Math.ceil(cost);
}
