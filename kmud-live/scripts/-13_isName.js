/*Returns true if str is one of the words(aliases) in namelist(which is seperated by a " "*/
function isName(str, namelist) {
	return namelist.toLowerCase().split(" ").indexOf(str.toLowerCase()) != -1;
}

