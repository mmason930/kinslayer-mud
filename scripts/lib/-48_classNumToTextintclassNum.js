//Takes constants.CLASS_XXX and converts to "xxx" (constants.CLASS_THIEF -> "thief"
// Rhollor Jan 2010
function classNumToText(classNum) {
	var str = "undefined";
	if(classNum == constants.CLASS_THIEF) {
		str = "thief";
	}
	else if(classNum == constants.CLASS_WARRIOR) {
		str = "warrior";
	}
	else if(classNum == constants.CLASS_RANGER) {
		str = "ranger";
	}
	else if(classNum == constants.CLASS_CHANNELER) {
		str = "channeler";
	}
	else if(classNum == constants.CLASS_FADE) {
		str = "fade";
	}
	else if(classNum == constants.CLASS_BLADEMASTER) {
		str = "blademaster";
	}
	else if(classNum == constants.CLASS_DREADLORD) {
		str = "dreadlord";
	}
	else if(classNum == constants.CLASS_GREYMAN) {
		str = "greyman";
	}
	else if(classNum == constants.CLASS_DRAGHKAR) {
		str = "draghkar";
	}
	else if(classNum == constants.CLASS_DREADGUARD) {
		str = "dreadguard";
	}
	else if(classNum == constants.CLASS_OGIER) {
		str = "ogier";
	}
	return str;
}
