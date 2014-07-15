/**********
 *
 * Galnor 12-23-2009 - Format a string into multiple lines of text given
 *    a certain column width. This works similarly to the /f option in
 *    many of the OLC text editors.*****ALDER VARIATION FOR FILES*****
                                   Delimits by ';' instead of newlines.
 **********/
 function strToFileColFormat( /*String*/sBuffer, /*Integer*/colWidth )
 {
	if(colWidth == undefined || colWidth <= 0)
		var colWidth = 40;//Default.
	var nBuffer = "";
	var vWords = getArgList( sBuffer );
	var iLineCount = 0;
	for(var i = 0;i < vWords.length;++i)
	{
		var word = vWords[i];
		if( iLineCount > 0 && iLineCount + word.length > colWidth ) {
			//We've exceeded this column's length. Move to next line.
			nBuffer += ";";
			iLineCount = 0;
		}
		iLineCount += word.length;
		nBuffer += word;
		if( (i+1) < vWords.length )
			nBuffer += " ";//There's still another word coming. We need a space separator.
	}
	return nBuffer;//Return the resulting string.
 }
 
