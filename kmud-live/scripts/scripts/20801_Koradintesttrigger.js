var script20801 = function(self, actor, here, args, extra) {
	//Test Trigger for Koradin
	var vArgs = getArgList(args);
	if (actor.level < 100 && actor.name != "Kortest") {
	self.say("yeah right " +actor.name);
	return;
	}
	/** RECEDIT LOADER
	
	waitpulse 1;
	actor.comm("recedit");
	waitpulse 1;
	actor.comm("3");
	waitpulse 1;
	var slots = getTailoringArmorSlots(actor);
	var slot = slots[8];
	var numstart = 208;
	var pracs = [90,90,90,90,90,90,90,99,90,90,90,90,90,90,90,99];
	var vnum = [4951,4953,4955,4957,4959,4961,4963,4965,4967,4969,4971,4973,4975,4977,4979,4981];
	var num = 0;
	var tVnum = [4944,4940,4941,4941,4942,4943,4946,4946,4944,4940,4941,4941,4942,4945,4947,4947];
	var mats = ["skin","hide","cloth","linen","wool","silk","velvet","fur","light leather","leather","quilted leather","padded leather","studded leather","chainmail","scale","brigandine"];
	while (num < 16) {
	if (num < 8)
		var gold = (num+1);
	else
		var gold = (num-7);
	actor.comm("a");
	waitpulse 1;
	actor.comm(num+numstart);
	waitpulse 1;
	actor.comm("1");
	waitpulse 1;
	actor.comm((slot[5]+" "+mats[num]+" "+slot[0]).toLowerCase());
	waitpulse 1;
	actor.comm("2");
	waitpulse 1;
	actor.comm(pracs[num]);
	waitpulse 1;
	actor.comm("7");
	waitpulse 1;
	actor.comm(gold*12500);
	waitpulse 1;
	actor.comm("a");
	waitpulse 1;
	actor.comm("i");
	waitpulse 1;
	actor.comm("1");
	waitpulse 1;
	actor.comm(vnum[num]);
	waitpulse 1;
	actor.comm("2");
	waitpulse 1;
	actor.comm(slot[2]);
	waitpulse 1;
	actor.comm("q");
	waitpulse 1;
	actor.comm("a");
	waitpulse 1;
	actor.comm("i");
	waitpulse 1;
	actor.comm("1");
	waitpulse 1;
	actor.comm(tVnum[num]);
	waitpulse 1;
	actor.comm("2");
	waitpulse 1;
	actor.comm(slot[6]);
	waitpulse 1;
	actor.comm("q");
	waitpulse 1;
	actor.comm("q");
	num += 1;
	waitpulse 1;
	
	}
	
	RECEDIT LOADER **/ 
	// var mat = ["skin","skin","skin","skin","skin","skin","skin","skin","skin","hide","hide","hide","hide","hide","hide","hide","hide","hide","cloth","cloth","cloth","cloth","cloth","cloth","cloth","cloth","cloth","linen","linen","linen","linen","linen","linen","linen","linen","linen","wool","wool","wool","wool","wool","wool","wool","wool","wool","silk","silk","silk","silk","silk","silk","silk","silk","silk","velvet","velvet","velvet","velvet","velvet","velvet","velvet","velvet","velvet","fur","fur","fur","fur","fur","fur","fur","fur","fur"];
	// var numstart = 31080;
	// var gold = 110;
	// var weightArr = [2.8, 2.6, 2.4, 2.2, 2, 1.8, 1.6, 1.4];
	// var parryArr = [-6,-5,-4,-3,-2,-1,0,1];
	// var dodgeArr = [-7,-6,-5,-4,-3,-2,-1,0];
	// var num = 54;
	// while (num < 72) {
		// var pdSval = getSval(actor,20801,"pd");
		// if (pdSval == 1) {
			// parry += 1;
			// setSval(actor,20801,"pd",2);
		// }
		// else if (pdSval == 2) {
			// dodge += 1;
			// setSval(actor,20801,"pd",1);
		// }
		// weight -= 0.1;
		// if (num == 0 || num == 9 || num == 18 || num == 27 || num == 36 || num == 45 || num == 54 || num == 63 || num == 72) {
			// var parry = parryArr[(num/9)];
			// var dodge = dodgeArr[(num/9)];
			// setSval(actor,20801,"pd",1);
			// var weight = weightArr[(num/9)];
		// }
			// if (num == 8 || num == 26 || num == 44 || num == 62)
				// parry += 1;
			// else
				// dodge += 1;
		// }
	// sendKoradin("Item:   "+mat[num]);
	// sendKoradin("Vnum:   "+(numstart+num));
	// sendKoradin("Weight: "+weight);
	// sendKoradin("Gold:   "+(gold+(gold*num)));
	// sendKoradin("Parry:  "+(parry>=0?parry:0));
	// sendKoradin("Dodge:  "+(dodge>=0?dodge:0));
	// actor.comm("oedit "+(numstart+num));
	// actor.comm("2");
	// actor.comm("dodge boots "+mat[num]);
	// actor.comm("4");
	// actor.comm("TAILORING ONLY, DO NOT USE");
	// actor.comm("/s");
	// actor.comm("5");
	// actor.comm("4");
	// actor.comm("6");
	// actor.comm("13");
	// actor.comm("0");
	// actor.comm("7");
	// actor.comm("7"); // WORN ON
	// actor.comm("0");
	// actor.comm("8");
	// actor.comm(weight);
	// actor.comm("9");
	// actor.comm(gold+(110*num));
	// actor.comm("e");
	// actor.comm((parry>=0?parry:0));
	// actor.comm("f");
	// actor.comm((dodge>=0?dodge:0));
	// actor.comm("j");
	// actor.comm("n");
	// actor.comm("20849");
	// actor.comm("x");
	// actor.comm("v");
	// actor.comm("1");
	// actor.comm("2");
	// actor.comm("2");
	// actor.comm("1");
	// actor.comm("3");
	// actor.comm("6");
	// actor.comm("0");
	// actor.comm("q");
	// actor.comm("y");
	// num += 1;
	// wait 5;
	// }
	
	/**
	 MANOR INITIATE
	
	var manor = new Object();
		
		manor.id = 3;
		manor.ownerUserId = 0;
		manor.race = 0;
		manor.name = "manor";
		manor.area = "the Tar Valon Road";
		manor.zoneId = 334;
		manor.hasStorage = false;
		manor.outsideGateRoomVnum = 33400;
		manor.insideGateRoomVnum = 33401;
		manor.innRoomVnum = 33410;
		manor.portalRoomVnum = 33411;
		manor.storageRoomVnum = 33409;
		manor.barRoomVnum = 33412;
		manor.flagRoomVnum = 33404;
		manor.createdDatetime = new Date();
		manor.createdByUserId = actor.id;
		manor.lastCapturedDatetime = new Date();
		manor.expireDatetime = new Date();
		manor.hasGate = false;
		manor.hasInn = false;
		manor.hasTrainer = false;
		manor.hasBar = false;
		manor.allowedUsers = [];
		var manor = getManor(here);
		manor.guardRooms = [33400,33401,33404,33407,33408];
		manor.shotRooms = [33400,33401,33403,33404,33405,33406];
		manor.rangeRooms = [33407,33408];
		manor.guardsPurchased = [];
		global.manors.push(manor); 
		saveManor(manor); **/
	
	
	/**for (var _autoKey in global.manors) {	var manor = global.manors[_autoKey];
		here.echo(manor.allowedUsers)
		//saveManor(manor);
	}
	**/
	/**
	sendKoradin(self.room.people[0].getPval("rakingTimeOfLastReward"));
	sendKoradin(getPval(self.room.people[0], 22339, "rakingTimeOfLastReward"))
	sendKoradin(time());
	sendKoradin((time()-parseInt(getPval(self.room.people[0], 22339, "rakingTimeOfLastReward")))/60/60);
	self.room.people[0].deletePval("rakingTimeOfLastReward",true);
	**/
	
	var date = new Date();
	here.echo(date.getDate());
	here.echo(date.getMonth());
	here.echo(date.getYear());
}