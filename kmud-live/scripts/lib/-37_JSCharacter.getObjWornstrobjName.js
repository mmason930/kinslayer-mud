//Searches a characters list of worn items to find an item matching the objName provided
//Rhollor - December 2009
JSCharacter.prototype.getObjWorn = function(objName) {
	for(var pos = 0; pos < constants.NUM_WEARS; pos++) {
		var currentItem = this.eq(pos);
		if(currentItem) {
			if(isName(objName,currentItem.namelist)){
				return currentItem;
			}
		}
	}
	return null;
}
