function dmgRangeToText(dmg) {
	if (dmg < 6)
		return "Tiny";
	else if (dmg < 10)
		return "Small";
	else if (dmg < 16)
		return "Medium";
	else if (dmg < 22)
		return "Good";
	else if (dmg < 25)
		return "High";
	else if (dmg < 28)
		return "Very High";
	else
		return "Godlike";
}
