function dirFromText( dText ) {
    if( !str_cmp(dText, "north") )
        return constants.NORTH;
    if( !str_cmp(dText, "east") )
        return constants.EAST;
    if( !str_cmp(dText, "south") )
        return constants.SOUTH;
    if( !str_cmp(dText, "west") )
        return constants.WEST;
    if( !str_cmp(dText, "up") )
        return constants.UP;
    if( !str_cmp(dText, "down") )
        return constants.DOWN;
    return -1;
}

function dirFromTextAbbreviated(dText)
{
	var letter = dText[0].toLowerCase();
	if(letter == "n")   return constants.NORTH;
	if(letter == "e")   return constants.EAST;
	if(letter == "s")   return constants.SOUTH;
	if(letter == "w")   return constants.WEST;
	if(letter == "u")   return constants.UP;
	if(letter == "d")   return constants.DOWN;
	return -1;
}