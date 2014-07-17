//Alder December 2009
//NOTE: this is CASE INSENSITIVE
function strn_cmp(userWord,realWord,minNumLetters) {
	if ( !userWord || !realWord || !minNumLetters ) {
		//mudLog(3, 103, "FLAGGED: invalid parameters used with 'strn_cmp(userWord,realWord,minNumLetters)'");
		return null;
	}
	userWord = userWord.toLowerCase();
	realWord = realWord.toLowerCase();
	if ( userWord == realWord )
		return true;
	if ( userWord.length > realWord.length || userWord.length < minNumLetters )
		return false;
	if ( minNumLetters > realWord.length )
		return false;
	for ( var i = minNumLetters; i <= realWord.length; i++ ) {
		var checkArray = [];
		for ( var n = 0; n < i; n++ )//n is current letter of realWord
			checkArray.push(realWord[n]);
		var check = checkArray.join("");
		if ( minNumLetters <= i && userWord == check )
			return true;
	}
	return false;
}
	

