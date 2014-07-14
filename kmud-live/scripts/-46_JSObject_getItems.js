/*******************************************************************
 *                                                                 *
 * JSCharacter.getItems()                                          *
 * Arguments: itemList - an array of items being searched.         *
 *                        element is either vnum of object sdesc   *
 *		   recursive- If true, checks CONTENTS of eq/inv           *
 *                                                                 *
 * Returns: An array of matching items                             *
 * Purpose: Searches an object for certain items, returning all    *
 *          items that match the specified criteria                *
 *                                                                 *
 * ~~~ By: Galnor 01/13/2010                                       *
 *                                                                 *
 *******************************************************************/
JSObject.prototype.getItems = function(itemList, recursive)
{
	var objArr = [];
	if( getObjectClass(itemList) != "Array" )
		itemList = [itemList];
	var cContents = this.contents;
	for(var i in cContents)
	{
		for each(var tObj in itemList)
		{
			if( (getObjectClass(tObj) == "Number" && cContents[i].vnum == tObj) )
				objArr.push( cContents[i] );
			else if( cContents[i].name == tObj )
				objArr.push( cContents[i] );
		}
		if( recursive && cContents[i].type == constants.ITEM_CONTAINER ) {
			objArr = objArr.concat( cContents[i].getItems(itemList, true) );
		} 
	}
    return objArr;
}
 
