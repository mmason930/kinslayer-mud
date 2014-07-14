var ClanUtil = new Object();
ClanUtil.onUserAddedToClan = function(user, clanId)
{

	var clanName = clanNumToText(clanId);
	
	user.setPval("CLAN_SCRIPT_NAME_SAVE", clanName);
	user.unlockAchievement(53, true);
	user.comm("save");
};

ClanUtil.onUserRemovedFromClan = function(user, clanId)
{
	var clanName = clanNumToText(clanId);
	
	user.lockTitle(clanName, "normal", 53);
	user.comm("save");
};

ClanUtil.onUserRemovedFromCouncil = function(user, clanId)
{
	var clanName = clanNumToText(clanId);
	getCharCols(user);
	
	user.send("You have been removed from the " + bld + clanName + nrm + " Council.");
	user.lockTitle(clanName + " Council", "normal", 54);
};

ClanUtil.onUserAddedToCouncil = function(user, clanId)
{
	var clanName = clanNumToText(clanId);
	getCharCols(user);
	
	user.send("You have been inducted into the " + bld + clanName + nrm + " Council.");
	user.setPval("CLAN_SCRIPT_NAME_SAVE", clanName + " Council");
	user.unlockAchievement(54, true);
};

ClanUtil.onUserAddedToCouncil = function(user, clanId)
{
	var clanName = clanNumToText(clanId);
	
	user.setPval("CLAN_SCRIPT_NAME_SAVE", clanName + " Council");
	user.unlockAchievement(54, true);
};

ClanUtil.onUserRemovedFromCouncil = function(user, clanId)
{
	var clanName = clanNumToText(clanId);
	
	user.lockTitle(clanName + " Council", "normal", 54);
};

