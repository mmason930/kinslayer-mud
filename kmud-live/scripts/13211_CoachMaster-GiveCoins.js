var script13211 = function(self, actor, here, args, extra)
{
	var amount = extra.amount;
var coachId = getSval(self, -13200, "coachId");
var coach = null;

if(actor.race != self.race)
{
	wait 1;
	self.say("I won't give a ride to your kind!");
	wait 1;
	self.comm("spit");
	return;
}

if(coachId != null)
	coach = global.coachManager.getCoachById(coachId);

if(coachId == null || coach == null)
{
	self.say("I seem to have lost track of my coach.");
	act("$n returns the money to you.", true, self, null, actor, constants.TO_VICT);
	act("$n returns the money to $N.", true, self, null, actor, constants.TO_NOTVICT);
	act("You return the money to $N.", true, self, null, actor, constants.TO_CHAR);
	self.gold -= amount;
	actor.gold += amount;
	
	mudLog(constants.BRF, 102, "Coach master(vnum " + self.vnum + " in room #" + self.room.vnum + ") has no coach.");
	return;
}

var departureHour24Format = coach.getNextCoachRide().getDepartureTime();
var departureHour = departureHour24Format % 12;
var departureHourToDisplay = departureHour % 12 == 0 ? 12 : departureHour % 12;

if(coach.getPassengersWhoHavePaid().indexOf(actor) != -1)
{
	self.say("You have already paid for your ride. Be sure to meet back here before " + (departureHourToDisplay) + " " + (departureHour24Format < 12 ? "AM" : "PM") + ".");
	act("$n returns the money to you.", true, self, null, actor, constants.TO_VICT);
	act("$n returns the money to $N.", true, self, null, actor, constants.TO_NOTVICT);
	act("You return the money to $N.", true, self, null, actor, constants.TO_CHAR);
	self.gold -= amount;
	actor.gold += amount;
	
	return;
}

var cost = coach.getNextCoachRide().getCost();

if(amount < cost)
{
	self.say("Actually, the cost of the ride is " + cost + " coppers.");
	act("$n returns the money to you.", true, self, null, actor, constants.TO_VICT);
	act("$n returns the money to $N.", true, self, null, actor, constants.TO_NOTVICT);
	act("You return the money to $N.", true, self, null, actor, constants.TO_CHAR);
	self.gold -= amount;
	actor.gold += amount;
	
	return;
}
else if(amount > cost)
{
	self.say("You gave me a bit too much.");
	act("$n returns some money to you.", true, self, null, actor, constants.TO_VICT);
	act("$n returns some money to $N.", true, self, null, actor, constants.TO_NOTVICT);
	act("You return some money to $N.", true, self, null, actor, constants.TO_CHAR);
	self.gold -= (amount - cost);
	actor.gold += (amount - cost);
	
	//Allow to drop through since user paid for the ride.
}

wait 1;

self.say("Excellent. We'll be departing at " + (departureHourToDisplay) + " " + (departureHour24Format < 12 ? "AM" : "PM") + ". Be sure to meet back here before then or you'll miss your ride!");
coach.getPassengersWhoHavePaid().push(actor);

};

