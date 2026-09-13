var script20943 = function(self, actor, here, args, extra) {
	var raider = self.getCharVis("headraidmob");
	if (!raider)
	{
		self.say("My target has died. Purging self.");
		self.extract();
		return;
	}
	function canSend( actor )
	{
		if (actor.level < 106)
			return true;
		return false;
	}
	self.say(raider.name)
	var room = raider.room.vnum;
	var zone = Math.floor(room / 100);
	var zonemin = zone * 100;
	var zonemax = zonemin + 99;
	const caem = 1;
	const baer = 45;
	const lug = 201;
	const tv = 223;
	const fm = 103;
	const fd = 17;
	const keep = 80;
	const hold = 81;
	var chance = random(1,10);
	self.say(zone);
	if (chance !== 1)
	{
		self.say("Chance to ring bell failed.");
		return;
	}
	if (zone === caem)
		gecho("The gongs of the Caemlyn city alarm sound off in the distance.",canSend);
	else if (zone === baer)
		gecho("A long whining alarm horn sounds off, warning of danger in Baerlon.",canSend);
	else if (zone === lug)
		gecho("The Lugard distress bell echoes throughout the landscape.",canSend);
	else if (zone === tv)
		gecho("The loud tolling of the Tar Valon bell can be heard in the distance.",canSend);
	else if (zone === fm)
		gecho("The bold call of the Fal Moran trumpets can be heard sounding off in the distance.",canSend);
	else if (zone === fd)
		gecho("The long Fal Dara alarm horn echoes far in the distance.",canSend);
	else if (zone === keep)
		gecho("The terrifying beat of the trolloc war drums can be heard in the distance.",canSend);
	else if (zone === hold)
		gecho("A screaming alarm echoes in the distance, summoning trollocs to the Stronghold.",canSend);
	else
		self.say(raider.name + " is not in a city.");
		
}