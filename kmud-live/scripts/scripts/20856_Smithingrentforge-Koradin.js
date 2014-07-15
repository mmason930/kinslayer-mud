var script20856 = function(self, actor, here, args, extra) {
	//Bribe apprentice for renting forge hourly
	//Koradin - April 2011
	var payment = extra.amount;
	if (self.race != actor.race) {
		self.say("Thanks for the tip!");
		return;
	}
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
	for each (var room in self.room.neighbors)
	{
		if (room)
		{
			if (self.room.doorName(self.room.firstStep(room)) == door)
				var newRoom = room;
		}
	}
	if (!newRoom)
		return;
	for each (var peep in newRoom.people)
	{
		var peepName = peep.namelist.split(" ");
		if (peep.maxHit > 30000 && peepName[peepName.length-1] == trader)
			var blackSmith = peep;
	}
	if (!blackSmith)
	{
		self.say("Sorry, " +actor.name+", but the "+place+" is currently closed. Check back soon!");
		actor.send(capFirstLetter(self.name) + " hands you your coins back.");
		act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
		actor.gold += payment;
		return;
	}
	if (actor.affectedBy(constants.AFF_NOQUIT)) {
		self.say("It takes a steady hand for "+tradeType.toLowerCase()+", "+actor.name+". Calm down a bit first.");
		actor.send(capFirstLetter(self.name) + " hands you your coins back.");
		act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
		actor.gold += payment;
		return;
	}
	var renterName = blackSmith.getPval("smither");
	if (renterName)
	{
		wait 1;
		if (actor.room != self.room)
			return;
		if (renterName == actor.name)
		{
			self.say("The "+place+" is already yours, "+actor.name+"!");
			actor.send(capFirstLetter(self.name) + " hands you your coins back and leads you into the "+place+".");
			act("$n hands $N some coins and leads $M into the "+place+".", true, self, null, actor, constants.TO_NOTVICT);
			actor.gold += payment;
			actor.moveToRoom(newRoom);
			actor.comm("look");
			return;
		}
		else
		{
			self.say("The "+place+" is currently rented by "+renterName+", "+actor.name+"!");
			actor.send(capFirstLetter(self.name) + " hands you your coins back.");
			act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
			actor.gold += payment;
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
	if (payment < reqPayment) {
		self.say("Sorry, that's not enough.");
		self.say("For "+rank +", the cost to rent the "+trader+" is"+actor.numCopperToText(reqPayment)+" per hour.")
		actor.send(capFirstLetter(self.name) + " hands you your coins back.");
		act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
		actor.gold += payment;
		return;
	}
	var timeRented = Math.floor((payment / reqPayment) * 60);
	var hours = timeRented / 60;
	hours = hours.toString();
	hours = hours.split(".");
	if (hours[1]) {
		var partial = parseInt(hours[1].charAt(0));
	}
	else {
		var timeMessage = hours[0] + " hour";
		if (hours[0] != "1")
			timeMessage += "s";
	}
	hours = parseInt(hours[0]);
	if (partial < 2) {
		var timeMessage = "just over " + hours + " hour";
		if (hours > 1)
			timeMessage += "s";
	}
	else if (partial > 1 && partial < 5) {
		var timeMessage = "a little over " + hours + " hour";
		if (hours > 1)
			timeMessage += "s";
	}
	else if (partial == 5)
		var timeMessage = hours + " and a half hours";
	else if (partial == 6)
		var timeMessage = "just over " + hours + " and a half hours";
	else if (partial > 6 && partial < 9)
		var timeMessage = "almost " + (hours+1) + " hours";	
	else if (partial == 9)
		var timeMessage = "just shy of " + (hours+1) + " hours";
	//
	// if (actor.name != "Koradin") {
	// self.say("The forge is closed for repairs, "+actor.name+"!  Check back soon.");
	// actor.send(capFirstLetter(self.name) + " hands you your coins back.");
	// act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
	// actor.gold += payment;
	// return;
	// }
	//
	wait 1;
	if (!actor || !self)
		return;
	else if (actor.room != self.room)
		return;
	else if (actor.affectedBy(constants.AFF_NOQUIT)) {
		self.say("You look too busy for "+tradeType.toLowerCase()+" right now, "+actor.name+"!");
		actor.send(capFirstLetter(self.name) + " hands you your coins back.");
		act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
		actor.gold += payment;
		return;
	}
	self.say("Right away, "+actor.name+"!");
	actor.send(capFirstLetter(self.name) + " leads you into the "+place+".");
	act("$n shows $N into the "+place+", closing the door behind $M.", true, self, null, actor, constants.TO_NOTVICT);
	actor.moveToRoom(newRoom);
	actor.comm("look");
	actor.send(capFirstLetter(self.name) + " says '"+actor.name + " has rented the "+place+" for "+timeMessage+", Master!'");
	self.gold -= payment;
	wait 1;
	blackSmith.comm("emote nods absently, hardly looking up from his work.");
	blackSmith.say("Well, get on with it! I'll boot you out myself if you stay longer than is paid for.");
	var pvalTime = time() + timeRented;
	blackSmith.setPval("smithingTime",pvalTime,true);
	blackSmith.setPval("smither",actor.name,true);
	
}