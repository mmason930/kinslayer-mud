/*Returns true if str is one of the words(aliases) in namelist(which is seperated by a " "*/
function isName(str, namelist) {
	return str != null && namelist != null && namelist.toLowerCase().split(" ").indexOf(str.toLowerCase()) != -1;
}


