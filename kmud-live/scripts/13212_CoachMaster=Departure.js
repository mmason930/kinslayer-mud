var script13212 = function(self, actor, here, args, extra)
{
	var coachId = getSval(self, -13200, "coachId");
var coach = null;

if(coachId != null)
	coach = global.coachManager.getCoachById(coachId);
	
if(coach == null)
	return;

var currentHour = getHour();
var departureHour = coach.getNextCoachRide().getDepartureTime();

if( (currentHour + 1) % 24 == departureHour )
	coach.sendDepartureWarningToPayingPassengers();
else if(currentHour == departureHour)
	coach.beginCoachRide();

};

