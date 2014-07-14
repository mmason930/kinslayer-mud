/*******************************************************************
 *                                                                 *
 * JSCharacter.getItems()                                          *
 * Arguments: itemList - an array of items being searched.         *
 *                        element is either vnum of object sdesc   *
 *		   checkInv - If true, checks char's inventory             *
 *         checkEQ  - If true, checks char's equipment             *
 *		   recursive- If true, checks CONTENTS of eq/inv           *
 *                                                                 *
 * Returns: An array of matching items                             *
 * Purpose: Searches a character for certain items, returning all  *
 *          items that match the specified criteria                *
 *                                                                 *
 * ~~~ By: Galnor 01/13/2010                                       *
 *                                                                 *
 *******************************************************************/
JSCharacter.prototype.getItems = function(itemList, checkInv, checkEQ, recursive)
{
	if( getObjectClass(itemList) != "Array" )
		itemList = [itemList];
	var objArr = [];
	var cInv = this.inventory;
	
	if( checkInv )
	{
		for(var i in cInv)
		{
			for each(var tObj in itemList)
			{
				if( (getObjectClass(tObj) == "Number" && cInv[i].vnum == tObj) )
					objArr.push( cInv[i] );
				else if( cInv[i].name == tObj ) 
					objArr.push( cInv[i] );
			}
			if( recursive && cInv[i].type == constants.ITEM_CONTAINER)
				objArr = objArr.concat( cInv[i].getItems(itemList, true) );
		}
	}
	if( checkEQ )
	{
		for(var i = 0;i < 22;++i)
		{
			var eObj = this.eq(i);
			if( !eObj )
				continue;
			for each(var tObj in itemList)
			{
				if( (getObjectClass(tObj) == "Number" && eObj.vnum == tObj) )
					objArr.push( eObj );
				else if( eObj.name == tObj )
					objArr.push( eObj );
			}
			if( recursive && eObj.type == constants.ITEM_CONTAINER )
				objArr = objArr.concat( eObj.getItems(itemList, true) );
		}
	}
    return objArr;
}
