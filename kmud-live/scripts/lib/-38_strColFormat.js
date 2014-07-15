/**********
 *
 * Galnor 12-23-2009 - Format a string into multiple lines of text given
 *    a certain column width. This works similarly to the /f option in
 *    many of the OLC text editors.
 **********/
function strColFormat( /*String*/sBuffer, /*Integer*/colWidth )
{
	if(colWidth == undefined || colWidth <= 0)
		var colWidth = 40;//Default.
	var nBuffer = "";
	var bufferLength = sBuffer.length;
	var currentLineLength = 0;
	var unprintable = {'\n' : '\n', '\t' : '\t', '\r' : '\r', ' ' : ' '};
	
	for(var index = 0;index < bufferLength;)
	{
		//Find the start of the next word.
		for(;index < bufferLength && unprintable[ sBuffer[index] ];++index)
		{
			if(sBuffer[index] == '\n')
			{
				nBuffer += "\n";
				currentLineLength = 0;
			}
		}
		
		if(index >= bufferLength)
			break;
		
		//Read the next word.
		var word = "";
		while(index < bufferLength && !unprintable[sBuffer[index]])
		{
			word += sBuffer[index++];
		}
		
		var wordLength = strlenDisplay(word);
		
		if(currentLineLength + wordLength > colWidth)
		{
			nBuffer += "\n";
			currentLineLength = 0;
		}
		
		currentLineLength += wordLength;
		nBuffer += word;
		
		if(index < bufferLength)
			nBuffer += " ";
	}
	
	return nBuffer;
}
