/****************************************************************************
 *               Coach, CoachRide, CoachMessage, CoachManager               *
 *                                                                          *
 * Contains all utilities for coaches in the game.                          *
 *                                                                          *
 * Originally By: Galnor, 2014-01-26                                        *
 ****************************************************************************/

 //Coach Class
function Coach()
{
	this.id = null;
	this.name = null;
	this.coachMasterMobVnum = null;
	this.coachMaster = null;
	this.nextCoachRide = null;
	this.coachRides = [];
	this.isMoving = false;
	
	this.passengersWhoHavePaid = [];
};

Coach.prototype.getId = function()
{
	return this.id;
};

Coach.prototype.setId = function(id)
{
	this.id = id;
};

Coach.prototype.getName = function()
{
	return this.name;
};

Coach.prototype.setName = function(name)
{
	this.name = name;
};

Coach.prototype.getCoachMasterMobVnum = function()
{
	return this.coachMasterMobVnum;
};

Coach.prototype.setCoachMasterMobVnum = function(coachMasterMobVnum)
{
	this.coachMasterMobVnum = coachMasterMobVnum;
};

Coach.prototype.getCoachMaster = function()
{
	return this.coachMaster;
};

Coach.prototype.setCoachMaster = function(coachMaster)
{
	this.coachMaster = coachMaster;
};

Coach.prototype.getCoachRides = function()
{
	return this.coachRides;
};

Coach.prototype.setCoachRides = function(coachRides)
{
	this.coachRides = coachRides;
};

Coach.prototype.getNextCoachRide = function()
{
	return this.nextCoachRide;
};

Coach.prototype.setNextCoachRide = function(nextCoachRide)
{
	this.nextCoachRide = nextCoachRide;
};

Coach.prototype.getIsMoving = function()
{
	return this.isMoving;
};

Coach.prototype.setIsMoving = function(isMoving)
{
	this.isMoving = isMoving;
};

Coach.prototype.getPassengersWhoHavePaid = function()
{
	return this.passengersWhoHavePaid;
}

Coach.prototype.setPassengersWhoHavePaid = function(passengersWhoHavePaid)
{
	this.passengersWhoHavePaid = passengersWhoHavePaid;
}

Coach.prototype.queueNextMessage = function(messageIndex)
{
	var nextRide = this.getNextCoachRide();
	var messages = nextRide.getMessages();
	if(messageIndex >= messages.length)
	{
		this.arriveAtDestination(nextRide);
		return;
	}

	var message = messages[ messageIndex ];
	var self = this;
	
	setTimeout(message.getSecondsDelayBefore(), function() {
	
		self.getCoachMaster().room.echo(message.getMessage());
		self.queueNextMessage(messageIndex + 1);
	});
}

Coach.prototype.sendDepartureWarningToPayingPassengers = function()
{
	var roomNumber = this.getCoachMaster().room.vnum;
	var self = this;
	
	this.getPassengersWhoHavePaid().forEach(function(passenger) {
		if(passenger.room.vnum != roomNumber)
			self.getCoachMaster().tell(passenger, "We will be departing in 1 minute!");
	});
};

Coach.prototype.beginCoachRide = function()
{
	this.getCoachMaster().say("All aboard!");
	var self = this;
	
	setTimeout(3, function() {
	
		var roomNumber = self.getCoachMaster().room.vnum;
		self.getPassengersWhoHavePaid().forEach(function(passenger) {
		
			if(passenger.room.vnum == roomNumber)
			{
				act("$n is ushered into the coach.", false, passenger, null, null, constants.TO_ROOM);
				passenger.send("You are ushered into the coach.");
				passenger.moveToRoom(getRoom(self.getNextCoachRide().getIntermediateRoomVnum()));
				passenger.comm("look");
				act("$n is ushered into the coach.", false, passenger, null, null, constants.TO_ROOM);
			}
		});
		
		self.getCoachMaster().moveToRoom(getRoom(self.getNextCoachRide().getIntermediateRoomVnum()));
		act("$n enters the coach and moves to the front, preparing for departure.", false, self.getCoachMaster(), null, null, constants.TO_ROOM);
		
		setTimeout(7, function() {
		
			getRoom(roomNumber).echo("The coach slowly picks up speed and departs from the area.");
			self.setIsMoving(true);
			self.queueNextMessage(0);
		});
	});
};


Coach.prototype.arriveAtDestination = function(coachRide)
{
	var destinationRoom = getRoom( coachRide.getDestinationRoomVnum() );
	if(this.getCoachMaster() == null)
	{
		this.setCoachMaster( destinationRoom.loadMob( this.getCoachMasterMobVnum() ) );
		setSval(this.getCoachMaster(), -13200, "coachId", this.getId());
	}
	else
		this.getCoachMaster().moveToRoom( destinationRoom );
		
	destinationRoom.echo(coachRide.getArrivalMessage());
	
	//Find next coach ride.
	this.setNextCoachRide( this.coachRides[ (this.coachRides.indexOf(coachRide) + 1) % this.coachRides.length ] );
	this.getPassengersWhoHavePaid().length = 0;
	
	setTimeout(7, function() {
		getRoom(coachRide.intermediateRoomVnum).people.forEach(function(person) {
	
			person.send("You are ushered out of the coach.");
			act("$n is ushered out of the coach.", false, person, null, null, constants.TO_ROOM);
			person.moveToRoom(destinationRoom);
			person.comm("look");
			act("$n is ushered out from the coach.", false, person, null, null, constants.TO_ROOM);
		});
	});
};


//CoachRide Class.
function CoachRide()
{
	this.id = null;
	this.departureTime = null;
	this.intermediateRoomVnum = null;
	this.messages = [];
	this.destinationRoomVnum = null;
	this.arrivalMessage = null;
	this.coachId = null;
	this.cost = null;
};

CoachRide.prototype.getId = function()
{
	return this.id;
}

CoachRide.prototype.setId = function(id)
{
	this.id = id;
}

CoachRide.prototype.getDepartureTime = function()
{
	return this.departureTime;
};

CoachRide.prototype.setDepartureTime = function(departureTime)
{
	this.departureTime = departureTime;
};

CoachRide.prototype.getIntermediateRoomVnum = function()
{
	return this.intermediateRoomVnum;
};

CoachRide.prototype.setIntermediateRoomVnum = function(intermediateRoomVnum)
{
	this.intermediateRoomVnum = intermediateRoomVnum;
};
CoachRide.prototype.getMessages = function()
{
	return this.messages;
};

CoachRide.prototype.setMessages = function(messages)
{
	this.messages = messages;
};

CoachRide.prototype.getDestinationRoomVnum = function()
{
	return this.destinationRoomVnum;
};

CoachRide.prototype.setDestinationRoomVnum = function(destinationRoomVnum)
{
	this.destinationRoomVnum = destinationRoomVnum;
};

CoachRide.prototype.getArrivalMessage = function()
{
	return this.arrivalMessage;
};

CoachRide.prototype.setArrivalMessage = function(arrivalMessage)
{
	this.arrivalMessage = arrivalMessage;
};

CoachRide.prototype.getCoachId = function()
{
	return this.coachId;
};

CoachRide.prototype.setCoachId = function(coachId)
{
	this.coachId = coachId;
};

CoachRide.prototype.getCost = function()
{
	return this.cost;
}

CoachRide.prototype.setCost = function(cost)
{
	this.cost = cost;
}


//CoachMessage Class
function CoachMessage()
{
	this.id = null;
	this.coachRideId = null;
	this.message = null;
	this.secondsDelayBefore = null;
};

CoachMessage.prototype.getId = function()
{
	return this.id;
};

CoachMessage.prototype.setId = function(id)
{
	this.id = id;
};

CoachMessage.prototype.getCoachRideId = function()
{
	this.coachRideId = coachRideId;
};

CoachMessage.prototype.setCoachRideId = function(coachRideId)
{
	this.coachRideId = coachRideId;
};

CoachMessage.prototype.getMessage = function()
{
	return this.message;
};

CoachMessage.prototype.setMessage = function(message)
{
	return this.message = message;
};

CoachMessage.prototype.getSecondsDelayBefore = function()
{
	return this.secondsDelayBefore;
};

CoachMessage.prototype.setSecondsDelayBefore = function(secondsDelayBefore)
{
	this.secondsDelayBefore = secondsDelayBefore;
};


//CoachManager Class
function CoachManager()
{
	this.coaches = {}; //Coach ID to Coach Object
};

CoachManager.prototype.getCoaches = function()
{
	return this.coaches;
};

CoachManager.prototype.setCoaches = function(coaches)
{
	this.coaches = coaches;
};

CoachManager.prototype.getCoachById = function(coachId)
{
	return this.coaches[ coachId ];
};

CoachManager.prototype.addCoachInternally = function(coach)
{
	if(this.coaches[ coach.getId() ] != null)
	{
		mudLog(constants.BRF, 102, "Attempting to add coach that already exists in CoachManager. Coach ID: " + coach.getId());
		return;
	}
	
	this.coaches[ coach.getId() ] = coach;
};

CoachManager.prototype.removeCoachInternally = function(coachId)
{
	if( !this.coaches.hasOwnProperty(coachId) )
	{
		mudLog(constants.BRF, 102, "Attempting to delete coach that does not exist in CoachManager. CoachID: "+  coachId);
		return;
	}
	
	delete this.coaches[ coachId ];
};

CoachManager.prototype.loadCoaches = function()
{
	//TODO: Load from database.
	var coach = new Coach();
	coach.setId(1);
	coach.setName("The Two Rivers Coach");
	coach.setCoachMasterMobVnum(13243);
	
	/***
	this.id = null;
	this.departureTime = null;
	this.intermediateRoomVnum = null;
	this.messages = [];
	this.destinationRoomVnum = null;
	this.coachId = null;
	***/
	var coachRide = new CoachRide();
	coachRide.setId(1);
	coachRide.setCoachId(coach.getId());
	coachRide.setDepartureTime(19);
	coachRide.setIntermediateRoomVnum(2);
	coachRide.setDestinationRoomVnum(10241);
	coachRide.setCost(100);
	coachRide.setArrivalMessage("A coach arrives from the south before teetering to a stop.");
	
	coach.getCoachRides().push(coachRide);
	
	/***
	this.id = null;
	this.coachRideId = null;
	this.message = null;
	this.secondsDelayBefore = null;
	***/
	var coachMessage = new CoachMessage();
	coachMessage.setId(1);
	coachMessage.setCoachRideId(coachRide.getId());
	coachMessage.setMessage("The coach bounces along the road.");
	coachMessage.setSecondsDelayBefore(10);
	
	coachRide.getMessages().push(coachMessage);
	
	
	
	
	coachRide = new CoachRide();
	coachRide.setId(2);
	coachRide.setCoachId(coach.getId());
	coachRide.setDepartureTime(20);
	coachRide.setIntermediateRoomVnum(4);
	coachRide.setDestinationRoomVnum(13213);
	coachRide.setCost(100);
	coachRide.setArrivalMessage("A coach arrives from the north before teetering to a stop.");
	
	coach.getCoachRides().push(coachRide);
	
	coachMessage = new CoachMessage();
	coachMessage.setId(2);
	coachMessage.setCoachRideId(coachRide.getId());
	coachMessage.setMessage("The coach moves north towards Baerlon.");
	coachMessage.setSecondsDelayBefore(10);
	
	coachRide.getMessages().push(coachMessage);
	
	this.addCoachInternally(coach);
	
	
	//Everything below here should remain unchanged when we switch to database storage.
	for each(var coach in this.coaches)
	{
		var coachRides = coach.getCoachRides();
		
		if(coachRides.length == 0)
			continue;
		
		coach.arriveAtDestination(coachRides[ coachRides.length - 1 ]);
	}
};
