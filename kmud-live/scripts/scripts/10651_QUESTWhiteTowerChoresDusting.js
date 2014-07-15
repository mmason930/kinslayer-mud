var script10651 = function(self, actor, here, args, extra) {
	///Jenkin 
	//October 2010
	//Quest: White Tower Chores
	if( getSval( actor, 10651, "dusting" ) == 1 ) {
	   _block;
	   actor.send("You work too quickly and get nothing done.");     
	   return;
	}
	if( actor.mvs < 35) {
	   _block;
	   actor.send("You try to dust, but are just too tired!");
	   return;
	}
	setSval( actor, 10651, "dusting", 1 );
	var argv = args.split(" ");
	var rank = actor.getRank(16);
	var vict = actor;
	if( actor.inClan(16) && rank < 5 ) {
	   _block;
	   var timeOfLastReward = getPval(actor, 10651, "dustingTimeOfLastReward");
	   if( !timeOfLastReward ) { 
	   timeOfLastReward = 0;
	   }
	   var timeNow = time();
	   var timeDiff = timeNow - timeOfLastReward;
	   var hourDiff = timeDiff / 60 / 60 ;
	   if( hourDiff >= 24 && random(1, 100) < 10 )  {
	   actor.send("You receive a notation of merit for your hard work!");
	   //waitpulse 7;
	   self.comm("award " + actor.name + " 16 1 White Tower chores");
	   setPval(actor, 10651, "dustingTimeOfLastReward", timeNow);   
	   }
	
	   else {
	   act("You begin dusting the bookshelves...", false, self, null, vict, constants.TO_VICT);
	   act("$N begins dusting the bookshelves...", false, self, null, vict, constants.TO_NOTVICT);
	   var mv_dmg = random(3,6);
	   actor.mvs -= mv_dmg;
	   waitpulse 21;
	   }   
	}
	setSval ( actor, 10651 , "dusting", 0 );
	
	
	
}