function liquidNumToText(liqNum) {
	var liquid = "undefined";
	if (liqNum == constants.LIQ_WATER) {
		liquid = "water";
	}
	else if (liqNum == constants.LIQ_BEER) {
		liquid = "beer";
	}
	else if (liqNum == constants.LIQ_WINE) {
		liquid = "wine";
	}
	else if (liqNum == constants.LIQ_ALE) {
		liquid = "ale";
	}
	else if (liqNum == constants.LIQ_DARKALE) {
		liquid = "dark ale";
	}
	else if (liqNum == constants.LIQ_WHISKY) {
		liquid = "whisky";
	}
	else if (liqNum == constants.LIQ_LEMONADE) {
		liquid = "lemonade";
	}
	else if (liqNum == constants.LIQ_FIREBRT) {
		liquid = "firebreather";
	}
	else if (liqNum == constants.LIQ_LOCALSPC) {
		liquid = "local specialty";
	}
	else if (liqNum == constants.LIQ_SLIME) {
		liquid = "slime";
	}
	else if (liqNum == constants.LIQ_MILK) {
		liquid = "milk";
	}
	else if (liqNum == constants.LIQ_TEA) {
		liquid = "tea";
	}
	else if (liqNum == constants.LIQ_COFFEE) {
		liquid = "coffee";
	}
	else if (liqNum == constants.LIQ_BLOOD) {
		liquid = "blood";
	}
	else if (liqNum == constants.LIQ_SALTWATER) {
		liquid = "salt water";
	}
	else if (liqNum == constants.LIQ_CLEARWATER) {
		liquid = "clear water";
	}
	else if (liqNum == constants.LIQ_OIL) {
		liquid = "oil";
	}
	return liquid;
}
