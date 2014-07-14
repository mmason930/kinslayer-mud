JSRoom.prototype.getObjects = function(objectVnum)
{
	var objectVnumArray = typeof(objectVnum) == "number" ? [objectVnum] : objectVnum;
	var objectVnumMap = {};
	var objectResultArray = [];
	
	//Convert to map with mob virtual number as the index to increase lookup speed below.
	for each(var tempObjectVnum in objectVnumArray)
	{
		objectVnumMap[ tempObjectVnum ] = true;
	}
	
	for each(var object in this.items)
	{
		if(objectVnumMap[ object.vnum ] === true)
			objectResultArray.push(object);
	}
	
	return objectResultArray;
}

