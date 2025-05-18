var script18605 = function(self, actor, here, args, extra) {
	//Commands for the mercenaries, they can be given by: order COMMAND, or: order MERC_ALIAS COMMAND
	//Rhollor - December 2009
	const cyn = actor.cyan(constants.CL_OFF); 
	const ylw = actor.yellow(constants.CL_OFF);
	const grn = actor.green(constants.CL_OFF);
	const red = actor.red(constants.CL_OFF);
	const mag = actor.magenta(constants.CL_OFF);
	const blu = actor.blue(constants.CL_OFF);
	const bld = actor.bold(constants.CL_OFF);
	const nrm = actor.normal(constants.CL_OFF);
		
	var vArgs = getArgList(args);
	var owner = getSval(self,18600,"owner");
	var specific = getCharInListVis(actor,vArgs[1],here.people);
	if(!owner.isValid) {
		var maxMercs = 30;
		var fileName = (self.id + ".mercEQ.");
		for(var i = 1; i <= maxMercs; i++) {
			//Overwrite file with ""
			fwrite(fileName + i, "");
		}
		setSval(self,18600,"owner",0);
		setSval(self,18600,"mercNum",-1);
		self.extract();
	}
	if(actor != owner) {
		return;
	}
	else {
		if(specific) {
			if( specific != self ) {
				_noblock;
				return;//Let other mob try.
			}
				//Attempting to give order to this specific merc
			_block;
		}
		else {
			if(strn_cmp(vArgs[1].toLowerCase(),"all",1)) {
				//Count number of Owner's mercs in room
				var ownerMercsInRoom = getSval(owner,18605,"ownerMercsInRoom");
				if(!ownerMercsInRoom) {	//This hasn't been checked before...
					echoaround(owner,owner.name + " issues an order.");
					// here.echo("!ownerMercsInRoom - hasnt been checked before new list");
					ownerMercsInRoom = [];
					for( var i = 0; i < self.room.people.length; i++) {
						if(getSval(self.room.people[i], 18600, "owner") == owner) {
							// here.echo(self.room.people[i].name + " has been pushed.");
							ownerMercsInRoom.push(self.room.people[i]); //Build an array of mercs in the room
							// here.echo(ownerMercsInRoom[0].name + " is the first element and " + ownerMercsInRoom[ownerMercsInRoom.length-1].name + " is the last"); 
						}
					}
					setSval(owner,18605,"ownerMercsInRoom",ownerMercsInRoom);
				}
				ownerMercsInRoom = [];
				ownerMercsInRoom = getSval(owner,18605,"ownerMercsInRoom");
				// here.echo(ownerMercsInRoom);
				if(ownerMercsInRoom.length > 0) {
					// here.echo("ownerMercsInRoom exists");
					// here.echo(ownerMercsInRoom[0].name);
					//read ownerMercsInRoom for "self"
					//for(var i = 0; i < ownerMercsInRoom.length; i++) {
						if(self == ownerMercsInRoom[0]) {
							// here.echo("[0] == self .. popping");
							var temp = [];
							for(var i = 0; i < (ownerMercsInRoom.length - 1); i++) {
								temp[i] = ownerMercsInRoom[i+1];
							}
							ownerMercsInRoom = temp;
							setSval(owner,18605,"ownerMercsInRoom",ownerMercsInRoom);
							if(ownerMercsInRoom.length == 0) {
								_block;
								setSval(owner,18605,"ownerMercsInRoom","");
							}
						}
					//							here.echo(ownerMercsInRoom[0].name + " is the first element and" + ownerMercsInRoom[ownerMercsInRoom.length-1].name + " is the last"); 
						//here.echo(ownerMercsInRoom[0].name + " " + ownerMercsInRoom[1].name + " " + ownerMercsInRoom[2].name); 
					//}
				}
				if(ownerMercsInRoom.length == 0) {
					_block;
					setSval(owner,18605,"ownerMercsInRoom","");
				}
			}
			else
				return;
		}
		if(specific) {
			owner.send("You issue an order to " + specific.name + ".");
			echoaround(owner,owner.name + " issues an order.");
		}
		else {
			owner.send("You issue an order.");
		}
	}
	/*** Set Up Constants ***/
	var myMercObj = getSval(self,-18600,"myMercObj");
	const THIEF = constants.CLASS_THIEF;
	const WARRIOR = constants.CLASS_WARRIOR;
	const RANGER = constants.CLASS_RANGER;
	const CHANNELER = constants.CLASS_CHANNELER;
	const DREADGUARD = constants.CLASS_DREADGUARD;
	if(vArgs[2]) {
		var arg1 = vArgs[2];
		arg1 = arg1.toLowerCase();
	}
	if(vArgs[3]) {
		var arg2 = vArgs[3];
		arg2 = arg2.toLowerCase();
	}
	if(vArgs[4]) {
		var arg3 = vArgs[4];
		arg3 = arg3.toLowerCase();
	}
	//waitpulse 5;
	if(strn_cmp(arg1,"crawl",2)) {
		if(arg2 != "out" && arg2 != "entrance") {
			self.say("Crawl? What am I crawling around in?!");
			return;
		}
		else {
			self.comm("crawl " + arg2);
		}
		return;
	}
	if(strn_cmp(arg1,"pick",1)) {
		if(myMercObj.className != THIEF && myMercObj.skillsList[myMercObj.skill] != "Pick") {
			self.say("I don't know how to do that...");
			return;
		}
		if(!arg2) {
			self.say("What am I trying to pick?");
			return;
		}
		var found = 0; //haven't found anything to pick yet...
		if(here.doorName_cmp(arg2) != -1){
			//there is a door by that name
			found = 1; //found it as a door
		}
		else {
			//maybe its a container???
			var posCont = getObjInListVis(self,arg2,here.items)
			if(posCont != null && posCont.type == constants.ITEM_CONTAINER) {
				found = 2; //found it as a container
			}
		}
		if(found) {
			if(actor.gold < 60) {
				//Not enough cash!!!
				self.say("I'll need a bit more incentive to use my skillz...");
				return;
			}
			else {
				if(found == 1) {
					here.echo(capFirstLetter(self.name) + " tries to pick " + arg2 + ".");
				}
				if(found == 2) {
					here.echo(capFirstLetter(self.name) + " tries to pick " + posCont.name + ".");
				}
				self.comm("pick " + arg2);
				owner.send(capFirstLetter(self.name) + " takes " + ylw + "60" + nrm + " copper for " + self.hisHer() + " services.");
				actor.gold -= 60;
				return;
			}
		}
		else {
			self.say("I dont see that...");
			return;
		}
		return;
	}
	//REMOVED FOR SPAM KICKING ABUSE
	if(strn_cmp(arg1,"sneak",2)) {
		 if(myMercObj.className != THIEF) {
			self.say("I don't know how to do that...");
			return;
		}
		self.say("I'll try to move quietly.");
		self.comm("sneak");
		return;
	}
	if(strn_cmp(arg1,"ride",2)) {
		if(!arg2) {
			self.say("What do you want me to ride?");
		}
		else {
			var horse = getCharInListVis(self,arg2,here.people);
			if(!horse.mobFlagged(constants.MOB_MOUNT)) {
				self.say("I can't ride " + horse.name + "!");
				return;
			}
			if(horse.riddenBy) {
				self.say(capFirstLetter(horse.name) + " is already being ridden.");
			}
			else {
				if(here.sector == constants.SECT_INSIDE) {
					self.say("I can't ride inside...");
				}
				else {
					self.comm("ride " + arg2);
				}
			}
		}
		return;
	}
	// When a mob tries to lead a horse it follows here.room.people[0]
	// if(strn_cmp(arg1,"lead",1)) {
		// if(!arg2) {
			// self.say("What horse do you want me to lead?");
		// }
		// else {
			// var horse = getCharInListVis(self,arg2,here.people);
			// if(!horse.mobFlagged(constants.MOB_MOUNT)) {
				// self.say("I can't lead " + horse.name + "!");
				// return;
			// }
			// if(horse.riddenBy) {
				// self.say(capFirstLetter(horse.name) + " is being ridden.");
			// }
			// else {
				// if(horse.master) {
					// self.say(capFirstLetter(horse.master.name) + " is already leading " + horse.name + ".");
				// }
				// else {
					// self.comm("lead " + arg2);
				// }
			// }
		// }
		// return;
	// }
	if(strn_cmp(arg1,"dismount",2)) {
		if(!self.mount) {
			self.say("I'm not riding anything...");
		}
		else {
			self.comm("dismount");
		}
		return;
	}
	if(strn_cmp(arg1,"rescue",3)) {
		if(arg2 == "me" || !arg2) {
			if(owner.position != constants.POS_FIGHTING) {
				self.say("And who am I rescuing you from?!");
				return;
			}
			else {
				self.comm("rescue " + owner.name);
			}
			return;
		}
		else {
			var target = getCharInListVis(self,arg2,here.people);
			if(target) {
				if(target.position != constants.POS_FIGHTING) {
					if(target == actor) {
						self.say("You aren't fighting anyone though.");
					}
					else {
						self.say(capFirstLetter(target.name) + " isn't fighting anyone though.");
						return;
					}
				}
				else {
					//echoaround(target,capFirstLetter(self.name) + " assists " + target.name + ".");
					self.comm("rescue " + target.namelist);
					return;
				}
			}
			else {
				self.say("Who am I rescuing?");
				return;
			}
		}
		return;
	}
	if(arg1 == "hide") {
		if(myMercObj.className != THIEF) {
			self.say("I don't know how to do that...");
			return;
		}
		if(self.position != constants.POS_STANDING) {
			self.say("I can't hide in this position.");
			return;
		}
		else {
			self.comm("nod");
			self.comm("hide");
		}
		return;
	}
	if(arg1 == "open") {
		if(!arg2) {
			self.say("What am I trying to open");
			return;
		}
		var found = 0; //haven't determined if it is hidden yet
		var found = 0; //haven't found anything to pick yet...
		if(here.doorName_cmp(arg2) != -1){
			//there is a door by that name
			found = here.doorName_cmp(arg2); //found it as a door
			if(here.doorHidden(found) > 0) {
				//The door is hidden restrict to Rangers only
	//			if(myMercObj && myMercObj.className != constants.CLASS_RANGER && myMercObj.skillsList[myMercObj.skill] != "Search") {
	//				found = 0;
	//			}
	//			else {
					found = 1;
	//			}
			}
		}
		else {
			//maybe its a container???
			var posCont = getObjInListVis(self,arg2,here.items)
			if(posCont != null && posCont.type == constants.ITEM_CONTAINER) {
				found = 10; //found it as a container
			}
		}
		if(found) {
			if(actor.gold < 40) {
				//Not enough cash!!!
				self.say("I'll need a bit more incentive to use my skillz...");
				return;
			}
			else {
				if(found != 10) {
					here.echo(capFirstLetter(self.name) + " tries to open " + arg2 + ".");
				}
				if(found == 10) {
					here.echo(capFirstLetter(self.name) + " tries to open " + posCont.name + ".");
				}
				self.comm("open " + arg2);
				owner.send(capFirstLetter(self.name) + " takes " + ylw + "40" + nrm + " copper for " + self.hisHer() + " services.");
				actor.gold -= 40;
			}
		}
		else {
			self.say("I dont see that...");
			return;
		}
		return;
	}
	if(arg1 == "follow") {
		if(owner == self.leader) {
			self.say("I'm already following you.");
		}
		else {
			self.comm("follow " + owner.name);
		}
		return;
	}
	if(strn_cmp(arg1,"north",1)) {
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.NORTH)) {
			if(here.doorIsClosed(constants.NORTH)) {
				self.say("The " + here.doorName(constants.NORTH) + " is closed.");
				return;
			}
			self.comm("north");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"south",1)){
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.SOUTH)) {
			if(here.doorIsClosed(constants.SOUTH)) {
				self.say("The " + here.doorName(constants.SOUTH) + " is closed.");
				return;
			}
			self.comm("south");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"west",1)){
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.WEST)) {
			if(here.doorIsClosed(constants.WEST)) {
				self.say("The " + here.doorName(constants.WEST) + " is closed.");
				return;
			}
			self.comm("west");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"east",1)){
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.EAST)) {
			if(here.doorIsClosed(constants.EAST)) {
				self.say("The " + here.doorName(constants.EAST) + " is closed.");
				return;
			}
			self.comm("east");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"up",1)){
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.UP)) {
			if(here.doorIsClosed(constants.UP)) {
				self.say("The " + here.doorName(constants.UP) + " is closed.");
				return;
			}
			self.comm("up");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"down",1)){
		if(self.position != constants.POS_STANDING) {
			self.say("I can't do that right now.");
			return;
		}
		if(here.direction(constants.DOWN)) {
			if(here.doorIsClosed(constants.DOWN)) {
				self.say("The " + here.doorName(constants.DOWN) + " is closed.");
				return;
			}
			self.comm("down");
		}
		else {
			self.say("I cannot go that way.");
			return;
		}
		return;
	}
	if(strn_cmp(arg1,"flee",1)) {
		self.comm("flee");
		return;
	}
	if(strn_cmp(arg1,"shield block",2)) {
		if(myMercObj.className != THIEF && myMercObj.skillsList[myMercObj.skill] != "Shield Block") {
			self.say("I don't know how to do that...");
			return;
		}
		var shield = self.eq(constants.WEAR_SHIELD);
		if(!shield) {
			self.say("I'm not even using a shield!");
			return;
		}
		if(self.position != constants.POS_FIGHTING) {
			self.say("I'm not fighting anyone though.");
		}
		else {
			self.say("I'll try to block!");
			self.comm("shield");
		}
		return;
	}
	if(strn_cmp(arg1,"precise strike",2)) {
		if(myMercObj.className != THIEF && myMercObj.skillsList[myMercObj.skill] != "Precise Strike") {
			self.say("I don't know how to do that...");
			return;
		}
		var weapon = self.eq(constants.WEAR_WIELD);
		if(!weapon) {
			self.say("I'm not even wielding a weapon!");
			return;
		}
		if(self.position != constants.POS_FIGHTING) {
			self.say("I'm not fighting anyone though.");
		}
		else {
			self.say("I'll try to strike them!");
			self.comm("prec");
		}
		return;
	}
	if(strn_cmp(arg1,"bash",2)) {
		if(myMercObj && myMercObj.className != WARRIOR && myMercObj.className != RANGER && myMercObj.skillsList[myMercObj.skill] != "Bash") {
			self.say("I don't know how to do that...");
			return;
		}
		var weapon = self.eq(constants.WEAR_WIELD);
		if(!weapon) {
			self.say("I'm not even wielding a weapon!");
			return;
		}
		if(self.position != constants.POS_FIGHTING) {
			self.say("I'm not fighting anyone though.");
		}
		else {
			if(!self.fighting.isBashed) {
				self.say("I'll try to sit 'em down!");
				self.comm("bash");
			}
			else {
				self.say(capFirstLetter(self.fighting.name) + " is already bashed!");
			}
		}
		return;
	}
	if(strn_cmp(arg1,"assist",2)) {
		if(!arg2) {
			if(owner.position != constants.POS_FIGHTING) {
				self.say("And who am I assisting you against?!");
				return;
			}
			else {
				self.comm("assist " + owner.name);
				return;
			}
		}
		else {
			if(arg2 == "me") {
				if(owner.position != constants.POS_FIGHTING) {
					self.say("And who am I assisting you against?!");
					return;
				}
				else {
					self.comm("assist " + owner.name);
				}
				return;
			}
			else {
				var target = getCharInListVis(self,arg2,here.people);
				if(target != null) {
					if(target.position != constants.POS_FIGHTING) {
						self.say(capFirstLetter(target.name) + " isn't fighting anyone though.");
						return;
					}
					else {
						echoaround(target,capFirstLetter(self.name) + " assists " + target.name + ".");
						self.comm("assist " + target.name);
					}
				}
				else {
					self.say("Who am I assisting?");
					return;
				}
			}
		}
		return;
	}
	if(strn_cmp(arg1,"rest",1)){
		if(self.position != constants.POS_RESTING) {
			self.comm("rest");
		}
		else {
			self.say("I am already resting.");
		}
		return;
	}
	if(strn_cmp(arg1,"sleep",2)){
		if(self.position != constants.POS_SLEEPING) {
			self.comm("sleep");
		}
		return;
	}
	if(strn_cmp(arg1,"stand",2)){
		if(self.position != constants.POS_STANDING) {
			self.comm("stand");
		}
		else {
			self.say("I am already standing.");
		}
		return;
	}
	if(strn_cmp(arg1,"sit",2)){
		if(self.position != constants.POS_SITTING) {
			self.comm("sit");
		}
		else {
			self.say("I am already sitting.");
		}
		return;
	}if(strn_cmp(arg1,"wake",2)){
		if(self.position == constants.POS_SLEEPING) {
			self.comm("wake");
		}
		else {
			self.say("I am already awake.");
		}
		return;
	}
	if(strn_cmp(arg1,"take",1) || strn_cmp(arg1,"get",1)) {
		if(!arg2) {
			self.say("What do you want me to take?");
			return;
		}
		else {
			if(self.room.items.length == 0 && !arg3) {
				self.say("There's nothing here to take.");
				return;
			}
			else {
				if(!arg3) {
					arg3 = "";
				}
				else {
					var cont = self.getObjWorn(arg3);
					if(cont == null) {
						//check inv
						cont = getObjInListVis(self,arg3,self.inventory);
					}
					if(cont == null) {
						//check ground
						cont = getObjInListVis(self,arg3,self.room.items);
					}
					if(cont == null) {
						//Didnt find cont
						self.say("I don't see a " + arg3 + " to take " + arg2 + " from.");
						return;
					}
					else {
						//did find cont
						if(cont.contents.length == 0) {
							self.say("There doesn't appear to be anything in " + cont.name + ".");
						}
					}
					arg3 = " " + arg3;
				}
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var target = getObjInListVis(self,arg2,self.room.items);
					if(target == null && arg2 != "all") {
						if(arg3) {
							target = getObjInListVis(self,arg2,cont.contents);
							if(target == null) {
								self.say("I don't see a " + arg2 + " in " + cont.name + ".");
								return;
							}
						}
						else {
							self.say("I don't see a " + arg2 + " that I can take.");
							return;
						}
					}
					if(target != null) {
						if(self.maxCarrItems <= self.inventory.length || self.maxCarryWeight <= self.carr_weight) {
							self.say("I can't carry any more.");
							return;
						}
						else {
							self.comm("take " + allPoint + arg2 + arg3);
						}
					}
				}
				else {
					if(self.maxCarrItems <= self.inventory.length || self.maxCarryWeight <= self.carr_weight) {
						self.say("I can't carry any more.");
						return;
					}
					else {
						self.comm("take all" + arg3);
					}
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"drop",2)) {
		if(!arg2) {
			self.say("What do you want me to drop?");
			return;
		}
		else {
			if(self.inventory.length == 0) {
				self.say("I'm not holding anything to drop.");
				return;
			}
			else {
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var target = getObjInListVis(self,arg2,self.inventory);
					if(target == null) {
						self.say("I don't see a " + arg2 + " that I can drop.");
					}
					else {
						self.comm("drop " + allPoint + arg2);
					}
				}
				else {
					self.comm("drop all");
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"give",2)) {
		if(!arg2) {
			self.say("What do you want me to give and to who?");
			return;
		}
		else {
			if(!arg3) {
				self.say("Who do you want me to give it to?");
				return;
			}
			if(self.inventory.length == 0) {
				self.say("I'm not holding anything to give.");
				return;
			}
			else {
				var rec = getCharInListVis(self,arg3,here.people);
				if(rec == null) {
					self.say(capFirstLetter(arg3) + " doesn't seem to be here.");
					return;
				}
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var target = getObjInListVis(self,arg2,self.inventory);
					if(target == null) {
						self.say("I don't see a " + arg2 + " that I can give.");
					}
					else {
						self.comm("give " + allPoint + arg2 + " " + arg3);
					}
				}
				else {
					self.comm("give all" + " " + arg3);
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"remove",3)) {
		if(!arg2) {
			self.say("What do you want me to remove?");
			return;
		}
		else {
			if(arg2 != "all") {
				var allPoint = "";
				if(arg2.split(".")[0] == "all") {
					arg2 = arg2.split(".")[1];
					allPoint = "all.";
				}
				var target = self.getObjWorn(arg2);
				if(target == null) {
					self.say("I don't see a " + arg2 + " that I can remove.");
				}
				else {
					if(self.maxCarrItems <= self.inventory.length || self.maxCarryWeight <= self.carr_weight) {
						self.say("I can't carry any more.");
						return;
					}
					else {
						self.comm("remove " + allPoint + arg2);
					}
				}
			}
			else {
				if(self.maxCarrItems <= self.inventory.length || self.maxCarryWeight <= self.carr_weight) {
					self.say("I can't carry any more.");
					return;
				}
				else {
					self.comm("remove all");
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"wear",3)) {
		if(!arg2) {
			self.say("What do you want me to wear?");
			return;
		}
		else {
			if(self.inventory.length == 0) {
				self.say("I'm not holding anything to wear.");
				return;
			}
			else {
				if(arg2 != "all") {
					var allPoint = "";
					if(arg2.split(".")[0] == "all") {
						arg2 = arg2.split(".")[1];
						allPoint = "all.";
					}
					var target = getObjInListVis(self,arg2,self.inventory);
					if(target == null) {
						self.say("I don't see a " + arg2 + " that I can wear.");
					}
					else {
						//maybe add a check to see if you are already wearing something there?
						self.comm("wear " + allPoint + arg2);
					}
				}
				else {
					self.comm("wear all");
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"wield",2)) {
		if(!arg2) {
			self.say("What do you want me to wield?");
			return;
		}
		else {
			if(self.inventory.length == 0) {
				self.say("I'm not holding anything to wield.");
				return;
			}
			else {
				if(arg2 != "all") {
					var target = getObjInListVis(self,arg2,self.inventory);
					if(target == null) {
						self.say("I don't see a " + arg2 + " that I can wield.");
					}
					else {
						var weapon = self.eq(constants.WEAR_WIELD);
						if(weapon) {
							self.say("I'm already wielding " + weapon.name + "."); 
						}
						else {
							if(target.canWear(constants.ITEM_WEAR_WIELD)) {
								self.comm("wield " + arg2);
							}
							else {
								self.say("I cannot wield " + target.name + ".");
							}
						}
					}
				}
				else {
					self.comm("wield all");
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"hold",1) || strn_cmp(arg1,"grab",2)) {
		if(!arg2) {
			self.say("What do you want me to hold?");
			return;
		}
		else {
			if(self.inventory.length == 0) {
				self.say("I'm not carrying anything to hold.");
				return;
			}
			else {
				if(arg2 != "all") {
					var target = getObjInListVis(self,arg2,self.inventory);
					if(target == null) {
						self.say("I don't see a " + arg2 + " that I can hold.");
					}
					else {
						var held = self.eq(constants.WEAR_HOLD);
						if(held) {
							self.say("I'm already holding " + held.name + "."); 
						}
						else {
							if(target.canWear(constants.ITEM_WEAR_HOLD)) {
								self.comm("hold " + arg2);
							}
							else {
								self.say("I cannot hold " + target.name + ".");
							}
						}
					}
				}
				else {
					self.comm("hold all");
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(strn_cmp(arg1,"put",2)) {
		if(!arg2) {
			self.say("What do you want me to put where?!");
			return;
		}
		else {
			var allPoint = "";
			if(arg2 != "all") {
				if(arg2.split(".")[0] == "all") {
					arg2 = arg2.split(".")[1];
					allPoint = "all.";
				}
			}
			else
			{
				var cont = self.getObjWorn(arg3); // check worn containers first
				if(cont == null) {
					cont = getObjInListVis(self,arg3,self.inventory); //Try in inv second
				}
				if(cont == null) {
					//Check ground for cont...
					cont = getObjInListVis(self,arg3,here.items);
				}
				if(cont == null) {
					self.say("I don't seem to have a " + arg3 + " to put " + target.name + " in.");
					return;
				}

				self.inventory.forEach(function(item) {
					self.comm("put " + item.namelist.split(" ")[0] + " " + arg3);
				});
				return;
			}
			var target = getObjInListVis(self,arg2,self.inventory);
			if(target == null) {
				self.say("I don't see a " + arg2 + " that I can put anywhere.");
			}
			else {
				//Found target item -> search for container to put into
				var cont = self.getObjWorn(arg3); // check worn containers first
				if(cont == null) {
					cont = getObjInListVis(self,arg3,self.inventory); //Try in inv second
				}
				if(cont == null) {
					//Check ground for cont...
					cont = getObjInListVis(self,arg3,here.items);
				}
				if(cont == null) {
					self.say("I don't seem to have a " + arg3 + " to put " + target.name + " in.");
					return;
				}
				else {
					//Have target obj and cont
					self.comm("put " + allPoint + arg2 + " " + arg3);
				}
			}
		}
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		return;
	}
	if(arg1 == "return" && arg2 == "home") {
		var homeRoom = getSval(self,18600,"homeRoom");
		var mercObj = getSval(self,-18600,"myMercObj");
		var mercArray = getSval(homeRoom,-18600,"mercArray");
		if (mercArray == "empty")
			mercArray = [];

		var price = getSval(self,18600,"price");
		self.say("OK, since you've seen me home alive I'll thank you with this bit of coin back.");
		owner.gold += parseInt(price*0.2);
	//	self.comm("mgold " + owner.name + " " + (price*0.2) );
		owner.send(self.name + " gives you" + owner.numCopperToText(Math.floor(price*0.2), true) + ".");
	//	waitpulse 7;
		self.say("I'll leave your things here for you.");
		fwrite(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"),"");
		self.comm("rem all");
		self.comm("give all " + owner.name);
		self.comm("rem all");
		self.comm("drop all");
		self.comm("follow self");
		setSval(self,18600,"owner",0);
		setSval(self,18600,"mercNum",-1);
		setSval(self,18605,"mercRendezvousRoom","");
		here.echo(capFirstLetter(self.name) + " sets off to return home.");
		self.extract();

		// Put the merc back into circulation
		mercArray.push(mercObj);
		setSval(homeRoom,-18600,"mercArray",mercArray);
	}
	if(strn_cmp(arg1,"inventory",1)) {
		owner.send(capFirstLetter(self.name) + "'s Inventory:");
		if(self.inventory.length == 0) {
			owner.send(" Nothing.");
		}
		for(var i = 0; i < self.inventory.length; i++) {
			owner.send(self.inventory[i].name);
		}
		return;
	}
	if(strn_cmp(arg1,"save",2)) {
		owner.send(grn + capFirstLetter(self.name) + " is saving..." + nrm);
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		owner.send(grn + capFirstLetter(self.name) + " has finished saving." + nrm);
		return;
	}
	if(owner.level > 100 && arg1 == "eq") {
		//Display saved eq vnums
		var fileName = (actor.id + ".mercEQ.");
		var empty = 0;
		owner.send("Saved eq:");
		for(var i = 1; i <= 30; i++) {
			owner.send("Merc " + i + ": " + fread(fileName + i));
			//Check for non empty file
			if(fread(fileName + i) == "") {
				empty++;
			}
		}
		if(empty == 30) {
			owner.send("No eq saved.");
		}
		return;
	}
	if(strn_cmp(arg1,"report",3) || strn_cmp(arg1,"status",4)) {
		actor.send("\n" + capFirstLetter(self.name) + "'s Status Report:");
		if(myMercObj.chan) {
			actor.send("I have " + cyn + self.hps + nrm + "(" + cyn + self.maxHit + nrm + ") hit, " + cyn + self.sps + nrm + "(" + cyn + self.maxSpell + nrm + ") mana and " + cyn + self.mvs + nrm + "(" + cyn + self.maxMove + nrm + ") movement points.");
		}
		else {
			actor.send("I have " + cyn + self.hps + nrm + "(" + cyn + self.maxHit + nrm + ") hit and " + cyn + self.mvs + nrm + "(" + cyn + self.maxMove + nrm + ") movement points.");
		}
		actor.send("Offensive Bonus: " + cyn + self.offense + nrm + " Dodge Bonus: " + cyn + self.dodge + nrm + ", Parry Bonus: " + cyn + self.parry + nrm);
		actor.send("My abilities are: Str: " + cyn + self.strength + nrm + ", Int: " + cyn + self.intelligence + nrm + ", Wis: " + cyn + self.wisdom + nrm + ", Dex: " + cyn + self.dexterity + nrm + ", Con: " + cyn + self.constitution + nrm);
		actor.send("My armor absorbs " + cyn + self.absorb + nrm + "%% damage on average.");
		var rendRoom = getSval(self,18605,"mercRendezvousRoom");
		if(rendRoom && rendRoom != "") {
			actor.send("My rendezvous point: " +  cyn + rendRoom.name + nrm + ".");
		}
		if(myMercObj.skill > -1) {
			actor.send("My skill: " + cyn + myMercObj.skillsList[myMercObj.skill] + nrm + ".");
		}
		return;
	}
	if(strn_cmp(arg1,"rendezvous",3)) {
		if(arg2 == "here") {
			//Set rendezvous room here
			setSval(self,18605,"mercRendezvousRoom",here);
			self.say("OK, we'll meet up here if we need to.");
		}
		else if(!arg2) {
			var rendRoom = getSval(self,18605,"mercRendezvousRoom");
			if(!rendRoom) {
				self.say("You never told me where to rendezvous! Do you want to meet HERE?");
			}
			else {
				self.say("OK I'll see you at the rendezvous!");
				here.echo(capFirstLetter(self.name) + " makes " + self.hisHer() + " way to the rendezvous point.");
				self.moveToRoom(rendRoom);
				rendRoom.echo(capFirstLetter(self.name) + " arrives at the rendezvous point.");
			}
		}
		return;
	}
	if(arg1 == "pwn") {
		actor.send("You asked for it!");
		actor.damage(actor.hps - 50,self);
		echoaround(actor,actor.name + " gets pwned by " + self.name + "!");
		actor.send("You are pwned!");
		return;
	}
	/****** Channel By Command ******/
	if(strn_cmp(arg1,"channel",2)) {
		if(myMercObj.className == CHANNELER || myMercObj.className == DREADGUARD) {
			if(!arg2) {
				self.say("What do you want me to weave?");
				return;
			}
			var heal, dmg, msgToActor, msgToRoom, length, cost, target, combat;
			health = 0;
			dmg = 0;
			cost = 0;
			length = 0;
			msgToActor = "";
			msgToRoom = "";
			target = "undefined";
			combat = false;
			/** Non-Combat Weaves **/
			switch(arg2) {
				case "healing":
					/*Healing*/
					if(myMercObj.skillsList[myMercObj.skill] != "Healing") {
						self.say("I don't have that talent.");
						return;
					}
					cost = Math.floor(0.85*myMercObj.maxSpell);
					msgToActor = capFirstLetter(self.name) + " makes a small gesture towards you and you feel better.";
					msgToRoom = capFirstLetter(self.name) + " makes a small gesture towards " + actor.name + " and " + actor.heShe() + " looks better.";
					length = 10;
					target = actor;
					health = 2*myMercObj.wis + myMercObj.intel;
					combat = false;
					break;
			/** Combat Weaves **/
				case "air":
					/*Air Scythe*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(15,19);
					length = 1.5;
					msgToActor = "You suddenly feel intense pain as deep cuts appear on your body.";
					msgToRoom = target.name + "'s flesh is sliced apart!";
					combat = true;
					cost = 40;
					break;
				case "fireball":
					/*Fireball*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(40,60);
					length = 4;
					msgToActor = "A large fireball blooms into existance and envelops you!";
					msgToRoom = "A large fireball blooms into existance and envelops " + target.name + ".";
					combat = true;
					cost = 80;
					break;
				case "ice":
					/*Ice Shards*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(26,36);
					length = 3.25;
					msgToActor = "Razor sharp ice shards seem to form from thin air before impaling you.";
					msgToRoom = target.name + " is struck by razor sharp ice shards!";
					combat = true;
					cost = 50;
					break;
				case "impact":
					/*Impact*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(27,39);
					length = 3.25;
					msgToActor = "A huge boulder rises from the ground and then crashes into you, causing a lot of pain.";
					msgToRoom = "A boulder floats up into the air and then races towards " + target.name + ", crashing right into " + target.himHer() + ".";
					combat = true;
					cost = 65;
					break;
				case "create":
					/*Create Fire*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(15,25);
					length = 1.5;
					msgToActor = "There is sudden burst of light as sparks ignite your clothing.";
					msgToRoom = target.name + " howls in pain as flames suddenly appear on " + target.himHer() + ".";
					combat = true;
					cost = 40;
					break;
				case "frostbite":
					/*Frostbite*/
					if(self.position == constants.POS_FIGHTING) {
						target = self.fighting;
					}
					else {
						target = "undefined";
					}
					dmg = random(14,22);
					length = 1.5;
					msgToActor = "Your limbs start to feel numb as frost accumulates over them.";
					msgToRoom = "Frost begins to grow on " + target.name + ".";
					combat = true;
					cost = 35;
					break;
			}
			if(msgToActor == "") {
				self.say("What do you want me to weave?");
				return;
			}
			if(self.position != constants.POS_STANDING && combat == false) {
				self.say("I can't form the weaves in this position.");
				return;
			}
			if(self.position == constants.POS_FIGHTING && combat == false) {
				self.say("I can't form those weaves while fighting!");
				return;
			}
			if(actor.position == constants.POS_FIGHTING && combat == false) {
				self.say("I can't get the weaves right with you fighting!");
				return;
			}
			/** Casting **/
			if(!self.isBashed) {
				if(myMercObj.sps < cost) {
					self.say("I don't have the energy to do that.");
					return;
				}
				if(target == "undefined" || !target || target == null) {
					self.say("Who am I doing this to?");
					return;
				}
				/* Echo Start of Weaving */
				here.echo(capFirstLetter(self.name) + " begins to form a weave.");
				self.startTimer(length);
				var success = runTimer(self);
				if(success && target.room == self.room) {
					if(self.affectedBy(constants.AFF_SHIELD)){
						here.echo(capFirstLetter(self.name) + " looks suprised as " + self.heShe() + " fails to reach the source.");
						return;
					}
					if(combat) {
						if(target.class == constants.CLASS_FADE || target.dodge > 100) {
							if(random(0,5) == 2) {
								target.send("You avoid " + self.name + "'s attempts at channeling!");
								here.echoaround(target,capFirstLetter(target.name) + " avoid's " + self.name + "'s attempt at channeling!");
								return;
							}
						}
					}
					else {
						if(self.position != constants.POS_STANDING) {
							self.say("I can't form the weaves in this position.");
							return;
						}
						if(self.position == constants.POS_FIGHTING) {
							self.say("I can't form those weaves while fighting!");
							return;
						}
						if(actor.position == constants.POS_FIGHTING) {
							self.say("I can't get the weaves right with you fighting!");
							return;
						}
					}
					/* Send Messages */
					target.send(msgToActor);
					var list = [self,target];
					echoaroundmany(list,msgToRoom);
					/* Do Damage */
					if(dmg) {
						dmg += Math.floor(myMercObj.skillBonus/2);
					}
					target.damage( dmg, self);
					/* Do Healing */
					if(health) {
						health += 3*myMercObj.skillBonus;
					}
					target.heal(health);
					/* Drain SPS */
					cost -=  myMercObj.skillBonus;
					self.sps -= cost;
					if(actor.level > 99) {
						here.echo("Cost: " + cost);
						here.echo("Dmg: " + Math.floor(myMercObj.skillBonus/2 + dmg) + ", Healing: " + health);
					}
				}
			}
		}
		else {
			//Not a channie or DG
			self.say("Are you crazy?! I can't channel!");
		}
	}
	if(strn_cmp(arg1,"commands",1)) {
		actor.send("ENABLED:");
		actor.send("Cmd Name             Arguments");
		actor.send("--------------------------------------------");
		actor.send("Assist          <Argument: NPC/PC to Assist>");
		actor.send("Bash            <No Argument>"); //Warrior + Ranger only
	//	actor.send("Cond            <No Argument>");
	//	actor.send("Condition       <No Argument>");
		actor.send("Crawl           <Argument: Entrance|Out>");
		actor.send("Dism            <No Argument>");
		actor.send("Dismount        <No Argument>");
		actor.send("Down            <No Argument>");
		actor.send("Drop            <Argument: itemName>");
		actor.send("East            <No Argument>");
		actor.send("Flee            <No Argument>");
		actor.send("Grab|Hold       <Argument: itemName>");
		actor.send("Hide            <No Argument>"); //Thief only
		actor.send("Inventory       <No Argument>");
		actor.send("Inv             <No Argument>");
	//	actor.send("Kick            <No Argument>");	//ABUSABLE - spam kick
	//	actor.send("Level           <No Argument>");
	//	actor.send("Mood            <Argument: Mood Name>");	//Dont think moods change anything for mobs could be wrong...
		actor.send("North           <No Argument>");
		actor.send("Open            <Argument: doorName>"); //Ranger opens hidden only
		actor.send(cyn + "Opening hidden doors costs fourty (" + ylw + "40" + cyn + ") copper." + nrm);
		actor.send("Pick            <Argument: doorName>"); //Thief only
		actor.send(cyn + "Picking locked doors costs sixty (" + ylw + "60" + cyn + ") copper." + nrm);
		actor.send("Put             <Arguments: itemName containerName>");
	//	actor.send("Pos             <Argument: Position Name>");
		actor.send("Rendezvous      <Argument: None|Here>");
		actor.send("Rest            <No Argument>");
		actor.send("Save            <No Argument>");
		actor.send("Sleep           <No Argument>");
		actor.send("South           <No Argument>");
		actor.send("Stand           <No Argument>");
		actor.send("Rem             <Argument: itemName>");
		actor.send("Remove          <Argument: itemName>");
		actor.send("Report          <No Argument>");
		actor.send("Rescue          <Argument: NPC/PC Being Rescued>"); //
		actor.send("Return          <Argument: Home>");
		actor.send("Ride            <Argument: horseName>");
		actor.send("Sneak           <No Argument>"); //Thief only
		actor.send("Status          <No Argument>");
	//	actor.send("Stats           <No Argument>");
		actor.send("Take|Get        <Argument: itemName [containerName]>");
		actor.send("Up              <No Argument>");
		actor.send("Wake            <No Argument>");
		actor.send("West            <No Argument>");
		actor.send("Wield           <Argument: itemName>");
		actor.send(cyn + "Address all complaints, suggestions, or bugs to Rhollor on the forums(http://kinslayermud.org/forums) or in game." + nrm);
		return;
	}
	return;
}
