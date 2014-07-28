function Global2014Util()
{
	this.pedestalObjectId = 23204;
	this.pedestalMaxHitPoints = 500;
	this.pedestalRoomIds = {};
	this.pedestalRoomIds[constants.RACE_HUMAN] = [1,2,3];
	this.pedestalRoomIds[constants.RACE_TROLLOC] = [4,7,6];

	this.objectIdToPedestalMap = {};
	
	
	this.dsAdam = null;
	this.lsAdam = null;
	this.eventIsActive = false;
	this.dsPlayers = [];
	this.lsPlayers = [];
	this.damaneVnums = [1122,1123,1124,1125,1126,1127];
	this.damaneZones = {};
	this.damaneZones[1122] = 0;
	this.damaneZones[1123] = 0;
	this.damaneZones[1124] = 0;
	this.damaneZones[1125] = 0;
	this.damaneZones[1126] = 0;
	this.damaneZones[1127] = 0;
}

Global2014Util.prototype.setupStage3 = function()
{
	var self = this;
	this.objectIdToPedestalMap = {};

	for(var race in this.pedestalRoomIds)
	{
		this.pedestalRoomIds[race].forEach(function(roomId) {

			getRoom(roomId).getObjects(self.pedestalObjectId).forEach(function(oldPedestal) {

				mudLog(constants.BRF, 100, "Extracting old pedestal in room #" + roomId + ".");
				oldPedestal.extract();
			});

			self.setupPedestal(race, roomId);
		});
	}
};

Global2014Util.prototype.setupPedestal = function(race, roomId)
{
	var object = getRoom(roomId).loadObj(this.pedestalObjectId);

	var pedestal = new Global2014Pedestal(object, this.pedestalMaxHitPoints, race);

	this.objectIdToPedestalMap[object.id] = pedestal;
	mudLog(constants.BRF, 100, "Pedestal loaded in room #" + roomId + " for race " + raceNumToText(race) + ".");
};

global.global2014Util = new Global2014Util();

/** TURN GLOBAL EVENT ON OR OFF **/
var script20986 = function(self, actor, here, args, extra) {
	if(actor.level < 104){
		return;
	}
	_block;
	var vArgs = args.split(" ");
	if(!vArgs[1]){
		actor.send("ON or OFF?");
		return;
	}
	if(strn_cmp(vArgs[1], "on", 2)){
		global.Global2014Util = new Global2014Util();
		global.Global2014Util.eventIsActive = true;
		here.echo("The global event has been turned ON.");
		here.echo("A'dams and player arrays have been wiped.");
	}else if(strn_cmp(vArgs[1], "off", 3)){
		if(global.Global2014Util){
			global.Global2014Util.eventIsActive = false;
			global.Global2014Util.dsAdam = null;
			global.Global2014Util.lsAdam = null;
			global.Global2014Util.dsPlayers = [];
			global.Global2014Util.lsPlayers = [];
			global.Global2014Util = null;
		}
		here.echo("The global event has been turned OFF.");
		here.echo("global.Global2014Util set to NULL.");
	}else{
		actor.send("The global event can be turned ON or OFF.");
		return;
	}
}

/** DAMANE PORTING / UPDATING TRACKING ON MAP / AGGROING / CALL PATROL **/
var script20980 = function(self, actor, here, args, extra) {
	var roomArr = [81, 10, 333, 80, 85, 20, 332, 18, 66, 181, 206, 193, 64, 17, 93, 19, 334, 103, 70, 100, 3, 1, 70, 214, 217, 4, 31, 201, 9, 230, 235, 100, 186, 53, 101, 102, 45, 134, 132];
	var newRoom = getRandomRoom();
	var neck1 = self.eq(constants.WEAR_NECK_1);
	if(neck1){ neck1 = neck1.vnum; }
	var neck2 = self.eq(constants.WEAR_NECK_2);
	if(neck2){ neck2 = neck2.vnum; }
	var randomRoll = 20;
	if(self.fighting){ randomRoll = 40; }
	if(newRoom && !self.isBashed && random(1, randomRoll) == 1 && neck1 != 1120 && neck2 != 1120){
		if(arrContains(roomArr, newRoom.zoneVnum)){
			act(self.name+" disappears in a flash of bright light.",true,self,null,null,constants.TO_ROOM);
			self.moveToRoom(newRoom);
			act(self.name+" arrives in a flash of bright light!",true,self,null,null,constants.TO_ROOM);
			if(global.Global2014Util){
				global.Global2014Util.damaneZones[self.vnum] = newRoom.zoneVnum;
			}
		}
	}else{
		if(global.Global2014Util){
			global.Global2014Util.damaneZones[self.vnum] = self.room.zoneVnum;
		}
	}
	if(neck1 != 1120 && neck2 != 1120){ // aggro/call mob if not collared
		if(self.fighting && random(0,100) < 5){ // call patrol
			var room = here;
			room.echo("The screaming cry of a beast can be heard from somewhere above you!");
			wait 1;
			if(self.room == room){
				room.echo("A Seanchan scouting party has arrived!");
				var loadRoom = getRoom(20900);
				loadRoom.loadMob(1135);
				var leader = loadRoom.people[0];
				loadRoom.loadMob(10259);
				var raken = loadRoom.people[0];
				loadRoom.people[0].comm("follow leader");
				loadRoom.loadMob(1136);
				var scout1 = loadRoom.people[0];
				loadRoom.people[0].comm("follow leader");
				loadRoom.loadMob(1136);
				var scout2 = loadRoom.people[0];
				loadRoom.people[0].comm("follow leader");
				leader.comm("group all");
				leader.moveToRoom(room);
				raken.moveToRoom(room);
				scout1.moveToRoom(room);
				scout2.moveToRoom(room);
				leader.say("There she is! Go and collect her!");
			}
			mudLog(2, 102, self.name+" generated a seanchan patrol in room "+self.room.vnum);
		}
		function attackPCs(self, here){ // aggro
			if(self && here){
				var attackArr = here.getMobs(-1);
				if(attackArr.length > 0){
					var target = attackArr[random(0, attackArr.length-1)];
					if(!self.fighting){
						self.say("Get away from me!");
						self.comm("k "+target.name);
					}
				}
			}
		}
		attackPCs(self, here);
		wait 3;
		attackPCs(self, here);
	}else{ // collared
		function followRandomPC(self, here){
			if(self && here){
				if(!self.leader){
					var followArr = here.getMobs(-1);
					if(followArr.length > 0){
						self.comm("follow "+followArr[random(0,followArr.length-1)].name);
					}
				}else{
					if(self.leader.room != self.room){
						var followArr = here.getMobs(-1);
						if(followArr.length > 0){
							self.comm("follow "+followArr[random(0,followArr.length-1)].name);
						}
					}
				}
			}
		}
		followRandomPC(self, here);
		wait 6;
		followRandomPC(self, here);
	}
}

/** PATROL AUTO RESCUE DAMANE **/
var script20987 = function(self, actor, here, args, extra) {
	if(global.Global2014Util){
		function rescueDamane(self, here){
			if(self && here){
				var people = here.people;
				for(var i=0;i<people.length;i++){
					if(people[i].fighting){
						if(arrContains(global.Global2014Util.damaneVnums, people[i].fighting.vnum)){
							if(people[i].vnum == 1135 || people[i].vnum == 1136){
								people[i].stopFighting();
							}else{
								self.say("The girl must survive to be collared!");
								self.comm("rescue damane");
							}
						}
					}
				}
			}
		}
		rescueDamane(self, here);
		wait 6;
		rescueDamane(self, here);
	}
}

/** LEASH DAMANE command for a'dam object **/
var script20983 = function(self, actor, here, args, extra) {
	if(global.Global2014Util){
		_block;
		var vArgs = args.split(" ");
		var damane = getCharInListVis(actor, vArgs[1], actor.room.people, false);
		if (!damane) {
			actor.send("Leash who?");
			return;
		}
		if(!arrContains(global.Global2014Util.damaneVnums, damane.vnum)){
			actor.send(damane.heShe()+" would probably not appreciate that.");
			return;
		}
		if((damane.fighting && !damane.isBashed) || actor.fighting){
			actor.send("You can't get close enough!");
			return;
		}
		actor.send("You start sneaking up on "+damane.name+", a'dam in hand...");
		act(actor.name+" sneaks up behind "+damane.name+", a'dam in hand...",true, actor,null,null,constants.TO_ROOM);
		actor.startTimer(random(5,8));
		var success = runTimer(actor);
		if(!success || !actor || !self){
			return;
		}
		if(!damane || damane.room != actor.room){
			actor.send("Drats! You were only a couple steps away!");
			return;
		}
		if((damane.fighting && !damane.isBashed) || actor.fighting){
			actor.send("You can't get close enough!");
			return;
		}
		for(var i=0; i<actor.room.people.length; i++){
			var peep = actor.room.people[i];
			if(peep == damane || peep.fighting == damane){
				peep.stopFighting();
			}
		}
		actor.send("You snap the a'dam around "+damane.name+"'s neck!");
		act("A look of horror crosses "+damane.name+"'s face as "+actor.name+" snaps an a'dam around her neck!",true, actor,null,null,constants.TO_ROOM);
		self.moveToChar(damane);
		damane.comm("wear adam");
		damane.comm("follow "+actor.name);
		mudLog(2, 102, actor.name+" just leashed "+damane.name+" for the "+(actor.race==constants.RACE_TROLLOC?"DARK":"LIGHT"));	
	}
}

/** LOGIN NOTIFICATION **/
var script20981 = function(self, actor, here, args, extra) { 
	//sendKoradin("working1");
	if(global.Global2014Util){
		if(global.Global2014Util.eventIsActive){
			//sendKoradin("working");
			waitpulse 1;
			getCharCols(actor);
			actor.send(cyn+bld+"Welcome to the global Kinslayer event! Type JOIN EVENT to get started."+nrm);
		}
	}
}

/** JOIN EVENT global command for event **/
var script20982 = function(self, actor, here, args, extra) { 
	getCharCols(actor);
	_block;
	return;
	var vArgs = args.split(" ");
	if(!strn_cmp(vArgs[1], "event", 5)){
		actor.send("Type JOIN EVENT to join the global event.");
		return;
	}
	if(global.Global2014Util){
		if(!global.Global2014Util.eventIsActive){
			actor.send("The global event is not currently running! Log in on Thursday, July 31 at 8pm EST.");
			return;
		}
		if(actor.race == constants.RACE_HUMAN){ // humans
			var adamMsg = "We've been sent items that will help you catch the damane. Come see me and I'll entrust one to you.";
			var adam = global.Global2014Util.lsAdam;
			if(adam){
				if(adam.isValid){
					var adamHolder = adam.findHolder;
					if(adamHolder){
						if(adamHolder.race == constants.RACE_HUMAN){
							adamMsg = "We've been sent items that will help you catch the damane, but I've already entrusted one to "+adamHolder.name+".  Find "+adamHolder.himHer()+" and assist us in this quest.";
						}
					}
				}
			}
			var players = global.Global2014Util.lsPlayers;
			var head = "Agelmar, Lord of Fal Dara";
			var script = ["I am looking for brave warriors to round these women up and bring them to a rendezvous point in Tarwin's Gap for transport to the White Tower.  The Amyrlin wishes to study them.", "Lord Marshall Uno is waiting in Tarwin's Gap. He will give you further instructions when you bring the damane to him.", adamMsg];
		}else{ // trolls
			var adamMsg = "The Dreadlord has leashes for the pink ones. Come see me to get one.";
			var adam = global.Global2014Util.dsAdam;
			if(adam){
				if(adam.isValid){
					var adamHolder = adam.findHolder;
					if(adamHolder){
						if(adamHolder.race == constants.RACE_TROLLOC){
							adamMsg = "The Dreadlord has leashes for the pink ones, but I've already give one to "+adamHolder.name+".  Find "+adamHolder.himHer()+".";
						}
					}
				}
			}
			var players = global.Global2014Util.dsPlayers;
			var head = "Murash";
			var script = ["The Dreadlord wants these scum for his own pleasure. Round them up and bring them to Tarwin's Gap for transport.", "Syyggar is waiting there. He will give you further instructions when you bring the damane to him.", adamMsg];; 
		}
		if(arrContains(players, actor)){
			actor.send("You've already joined the global event!");
			return;
		}
		players.push(actor);
		actor.send(cyn+bld+"You join the global event!"+nrm);
		actor.send(" ");
		actor.send(red+head+" tells you, 'We have had a report of six damane that escaped and are traveling around the world.'"+nrm);
		actor.send(" ");
		actor.send(red+head+" tells you, '"+script[0]+"'"+nrm);
		actor.send(" ");
		actor.send(red+head+" tells you, '"+script[1]+"'"+nrm);
		actor.send(" ");
		actor.send(red+head+" tells you, '"+script[2]+"'"+nrm);
		actor.send(" ");
		actor.send(red+head+" tells you, 'Reports of the damane whereabouts will show up in green on your MAP as they come in.'"+nrm);
	}else{
		actor.send("The global event is not currently running! Log in on Thursday, July 31 at 8pm EST.");
		return;
	}
}

/** MURASH / AGELMAR ISSUE ANOTHER A'DAM ON ENTER **/
var script20985 = function(self, actor, here, args, extra) { 
	if(global.Global2014Util){
		waitpulse 1;
		if(actor && self){
			if(self.vnum == 1700){ // murash
				var players = global.Global2014Util.dsPlayers;
				var adam = global.Global2014Util.dsAdam;
			}else{
				var players = global.Global2014Util.lsPlayers;
				var adam = global.Global2014Util.lsAdam;
			}
			if(actor.race == self.race && arrContains(players, actor)){
				if(adam){
					if(adam.isValid){
						var adamHolder = adam.findHolder;
						if(adamHolder){
							if(adamHolder.race == actor.race){
								self.tell(actor, "I've given an a'dam to "+adamHolder.name);
								return;
							}
						}else{
							adam.extract();
						}
					}
				}
				var newAdam = self.loadObj(1120);
				if(self.race == constants.RACE_TROLLOC){
					global.Global2014Util.dsAdam = newAdam;
				}else{
					global.Global2014Util.lsAdam = newAdam;
				}
				newAdam.moveToChar(actor);
				self.tell(actor, "Use it wisely.");
				actor.send(self.name+" gives you "+newAdam.name+".");
				act(self.name+" gives "+actor.name+" "+newAdam.name+".", true, actor, null, null, constants.TO_ROOM);
			}
		}
	}
}
