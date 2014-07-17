var script22339 = function(self, actor, here, args, extra) {
	//Koradin
	//February 2012
	//Quest: White Tower Chores
	if (actor.room.vnum < 22339 || actor.room.vnum > 22342) {
		_block;
		actor.send("There's nothing to rake here!");
		return;
	}
	var roomRaked = getSval(actor.room, 22339, "raked");
	if (!roomRaked) {
		roomRaked = 0;
	}
	if (roomRaked > 10) {
		_block;
		actor.send("This area of the garden has been raked clean!");
		return;
	}
	if (actor.position != constants.POS_STANDING) {
		_block;
		actor.send("You're not in the position to do that!");
		return;
	}
	if( getSval( actor, 22339, "raking" ) > time() ) {
	   _block;
	   actor.send("You work too quickly and get nothing done.");     
	   return;
	}
	if( actor.mvs < 35) {
	   _block;
	   actor.send("You try to rake, but are just too tired!");
	   return;
	}
	setSval( actor, 22339, "raking", time()+2 );
	var argv = args.split(" ");
	var rank = actor.getRank(16);
	var qps = actor.qp(16);
	if( actor.inClan(16) && qps < 150 ) {
	   _block;
	   var timeOfLastReward = actor.getPval("rakingTimeOfLastReward");
	   var today = new Date();
	   var month = today.getMonth();
	   var day = today.getDate();
	   if (parseInt(today.getMonth()) < 10)
			month = "0"+parseInt(today.getMonth());
	   if (parseInt(today.getDate()) < 10)
			day = "0"+parseInt(today.getDate());
	   var today = parseInt(today.getYear()+""+month+""+day);
	   sendKoradin("today: "+today+", "+typeof(today));
	   if( !timeOfLastReward ) { 
	   timeOfLastReward = 0;
	   }
	   timeOfLastReward = parseInt(timeOfLastReward);
	   if (timeOfLastReward > 9999999)
		timeOfLastReward = 0;
	   sendKoradin(actor.name+" raking, last received: "+timeOfLastReward);
	   //return;
	   if( today > timeOfLastReward && random(1, 100) < 10 )  {
		//sendKoradin()
		//return;
		   var sheriamRoom = getRoom(22344);
		   for (i=0;i<sheriamRoom.people.length;i++) {
			if (sheriamRoom.people[i].vnum == 22303)
				var sheriam = sheriamRoom.people[i];
		   }
		   if (sheriam) {
		   actor.send("You receive a notation of merit for your hard work!");
		   sheriam.comm("award " + actor.name + " 16 1 White Tower Chores");
		   actor.setPval("rakingTimeOfLastReward", today, true); 
			}
	   }
	   actor.send("You begin raking the garden...");
	   act("$n begins raking the garden...", false, actor, null, null, constants.TO_ROOM);
	   var mv_dmg = random(3,6);
	   actor.mvs -= mv_dmg;
	   setSval(actor.room, 22339, "raked", roomRaked+1);
	   if (random(1,25) == 1) {
			var randHerb = getObjProto(random(20985,20998));
			actor.send("You uncovered "+randHerb.name+"!");
			actor.room.loadObj(randHerb.vnum);
	   }
	   if (random(1,5) == 1) {
			if (actor.level < 50) {
				actor.send("You receive some experience points!");
				actor.experience += random(1,1000000);
			}
		}
	   waitpulse 21;  
	}
	else if( actor.inClan(16) && rank > 4 ) {
		actor.send("Leave the chores for the Novices and Accepted!");
	}
	else
		actor.send("Leave the chores for the less experienced!");
	setSval ( actor, 22339 , "raking", 0 );
	
	
	
	
	
	
	
	
	
	
	
	
}