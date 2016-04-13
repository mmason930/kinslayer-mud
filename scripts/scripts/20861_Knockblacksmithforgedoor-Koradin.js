var script20861 = function(self, actor, here, args, extra) {
	var argArray = getArgList(args);
	if (!argArray[1])
		return;
	if (arrContains(self.namelist.split(" "), "blacksmith")) {
		var tradeType = "Smithing";
		var trader = "blacksmith";
		var place = "forge";
		var door = "forgedoor";
	}
	else if (arrContains(self.namelist.split(" "), "tailor")) {
		var tradeType = "Tailoring";
		var trader = "tailor";
		var place = "workshop";
		var door = "workshop";
	}
	if (str_cmp(argArray[1],door))
		return;
	for (var _autoKey in self.room.neighbors) {
		var room = self.room.neighbors[_autoKey];
		if (room)
		{
			if (self.room.doorName(self.room.firstStep(room)) == door)
				var newRoom = room;
		}
	}
	if (!newRoom)
		return;
	for (var _autoKey in newRoom.people) {
		var peep = newRoom.people[_autoKey];
		var peepName = peep.namelist.split(" ");
		if (peep.maxHit > 30000 && peepName[peepName.length-1] == trader)
			var blackSmith = peep;
	}
	if (!blackSmith)
	{
		self.say("Sorry, " +actor.name+", but the "+place+" is currently closed. Check back soon!");
		return;
	}
	if (actor.affectedBy(constants.AFF_NOQUIT)) {
		self.say("It takes a steady hand for "+tradeType.toLowerCase()+", "+actor.name+". Calm down a bit first.");
		return;
	}
	var renterName = blackSmith.getPval("smither");
	if (renterName)
	{
		if (renterName == actor.name)
		{
			wait 1;
			if (actor.room != self.room)
				return;
			self.say("Right away, "+actor.name+"!");
			actor.moveToRoom(newRoom);
			actor.comm("look");
			return;
		}
		else
		{
			self.say("The "+place+" is currently rented by "+renterName+", "+actor.name+"!");
			return;
		}
	}
	var skill = actor.getSkill(getSkillVnum(tradeType));
	// if (skill < 10) {
		// self.say("You have no need to rent the forge if you can't smith, "+actor.name+"!");
		// actor.send(capFirstLetter(self.name) + " hands you your coins back.");
		// act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
		// actor.gold += payment;
		// return;
	// }
	if (skill > 98) {
		var reqPayment = 240;
		var rank = "artisan "+trader+"s";
	}
	else if (skill > 74) {
		var reqPayment = 180;
		var rank = "master "+trader+"s";
	}
	else if (skill > 39) {
		var reqPayment = 120;
		var rank = "journeyman "+trader+"s";
	}
	else if (skill > 9) {
		var reqPayment = 60;
		var rank = "apprentice "+trader+"s";
	}
	else {
		var reqPayment = 30;
		var rank = "commoners";
	}
	wait 1;
	self.say("For "+rank +", the cost to rent the "+place+" is"+actor.numCopperToText(reqPayment)+" per hour.")
	wait 1;
	self.say("You can pay for as much time as you want, with a minimum of one hour!");
	wait 1;
	self.say("Just hand me the coin and I'll see if the "+place+" is available.");
	return;
	
}