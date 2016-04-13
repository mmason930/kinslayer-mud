//Alder
//March 2010
/***
Indents each line with length lineLength of str after the first, indentDist number of spaces.
***/
function hangIndentStr(str,lineLength,indentDist) {
	var lines = str.split("\n");
	var totLines = [];
	for (var _autoKey in lines) {
		var line = lines[_autoKey];
		var text = strColFormat(line,lineLength);
		var newLines = text.split("\n");
	}
	var blockStr = "";
	var indent = "";
	for ( var p = 1; p <= indentDist; p++ ) {
		indent += " ";
	}
	for ( var i = 0; i < newLines.length; i++ ) {
		blockStr += (i==0 ? "" : indent)+newLines[i]+"\n";
	}
	return ( blockStr );
}
