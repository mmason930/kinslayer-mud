function trimExcessSpacing(argument) {
	
	var buffer = "";
	
	for(var i = 0;i < argument.length;++i) {
	
		if(i > 0 && argument[i] == ' ' && argument[i]-1 == ' ') {
		
			continue;
		}
		buffer += argument[i];
	}
	return buffer;
}
function phraseMatch(args, phraseToMatch) {
	return trimExcessSpacing(args).toLowerCase().match(phraseToMatch.toLowerCase());
}
