JSRoom.prototype.getMobs = function( mobVnum )
{
	var mobVnumArray = typeof(mobVnum) == "number" ? [mobVnum] : mobVnum;
	var mobVnumMap = {};
	var mobResultArray = [];
	
	//Convert to map with mob virtual number as the index to increase lookup speed below.
	for each(var tempMobVnum in mobVnumArray)
	{
		mobVnumMap[ tempMobVnum ] = true;
	}
	
	for each(var character in this.people)
	{
		if(mobVnumMap[ character.vnum ] === true)
			mobResultArray.push(character);
	}
	
	return mobResultArray;
}
