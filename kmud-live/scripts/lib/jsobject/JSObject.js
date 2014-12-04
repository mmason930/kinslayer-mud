JSObject.prototype.__defineGetter__("weaponType", function() {
	if( this.type != constants.ITEM_WEAPON )
		return -1;
	return this.value(constants.VALUE_WEAPON_TYPE);
});

JSObject.prototype.hasItem = function(item_vnum, recursive)
{
	for(var i in this.contents) {
		if( (getObjectClass(item_vnum) == "Number" && this.contents[i].vnum == item_vnum) )
			return this.contents[i];
		else if( this.contents[i].name == item_vnum )
			return this.contents[i];//Scanning by name.
		if( recursive ) {
			var obj = this.contents[i].hasItem(item_vnum, true);
			if( obj )
				return obj;
		}
	}
	return null;
};

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
		for (var _autoKey in itemList) {
			var tObj = itemList[_autoKey];
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
};

JSObject.prototype.__defineGetter__("room", function()
	{
		if( this.inRoom )
			return this.inRoom;
		var holder = this.findHolder;
		if( holder ) {
			if( holder.room )
				return holder.room;
			if( holder.inRoom )
				return holder.inRoom;
		}
		return undefined;
	}
);

//	June 2010 - Fogel
//      Moved this method to JavaScript to avoid having crafting items show up as retooled.
JSObject.prototype.isRetooled = function()
{
	//	Ignore crafting base items.
	if( this.vnum >= 4800 && this.vnum <= 4999 )
		return false;

	var name = this.name;
	var namelist = this.namelist;
	var ldesc = this.ldesc;
	this.setRetoolName("");
	this.setRetoolDesc("");
	this.setRetoolSDesc("");
	if( name == this.name && namelist == this.namelist && ldesc == this.ldesc )
		return false;
	else
	{
		this.setRetoolSDesc(name);
		this.setRetoolName(namelist);
		this.setRetoolDesc(ldesc);
		return true;
	}
};

JSObject.prototype.numExists = function()
{
    var sql = "SELECT COUNT(vnum) as total FROM objects WHERE vnum="+this.vnum;
    var result = sqlQuery(sql);
    var row = result.getRow;
    return row.get("total");
}