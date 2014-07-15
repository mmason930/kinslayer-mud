function getAscii(c)
{
	// restrict input to a single character
	c = c . charAt (0);
	// loop through all possible ASCII values
	var i;
	for (i = 0; i < 256; ++ i)
	{
		// convert i into a 2-digit hex string
		var h = i . toString (16);
		if (h . length == 1)
			h = "0" + h;
		// insert a % character into the string
		h = "%" + h;
		// determine the character represented by the escape code
		h = unescape (h);
		// if the characters match, we've found the ASCII value
		if (h == c)
			break;
	}
	return i;
}
