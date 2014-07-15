//Alder
//February 2011
//Find every item on a person (fully recursive)
JSCharacter.prototype.getAllItems = function() {
	var objArr = [];
	var cInv = this.inventory;
	
	for(var i in cInv) {
		objArr.push( cInv[i] );
		if( cInv[i].type == constants.ITEM_CONTAINER) {
			objArr = objArr.concat( cInv[i].getAllItems() );
		}
	}
	for(var i = 0;i < 22;++i) {
		var eObj = this.eq(i);
		if( !eObj ) {
			continue;
		}
		objArr.push( eObj );
		if( eObj.type == constants.ITEM_CONTAINER ) {
			objArr = objArr.concat( eObj.getAllItems() );
		}
	}
    return objArr;
}
//Find every item in an object (fully recursive)
JSObject.prototype.getAllItems = function()
{
        var objArr = [];
        var cContents = this.contents;
        for(var i in cContents) {
			objArr.push( cContents[i] );
			if( cContents[i].type == constants.ITEM_CONTAINER ) {
				objArr = objArr.concat( cContents[i].getAllItems() );
			} 
        }
    return objArr;
}
