var script20981 = function(self, actor, here, args, extra) { // login notification
	//sendKoradin("working1");
	if(global.eventIsActive){
		//sendKoradin("working");
		waitpulse 1;
		getCharCols(actor);
		actor.send(cyn+bld+"Welcome to the global Kinslayer event! Type JOIN EVENT to get started."+nrm);
	}
}

var script20982 = function(self, actor, here, args, extra) { // login notification
	getCharCols(actor);
	_block;
	var vArgs = args.split(" ");
	if(!strn_cmp(vArgs[1], "event", 5)){
		actor.send("Type JOIN EVENT to join the global event.");
		return;
	}
	if(!global.eventIsActive){
		actor.send("The global event is not currently running! Log in on Thursday, July 31 at 8pm EST.");
		return;
	}
	if(actor.race == constants.RACE_HUMAN){ // humans
		var adamMsg = "We've been sent an item that will help you catch the damane. Come see me and I'll entrust it to you.";
		var adam = global.lsAdam;
		if(adam){
			if(adam.isValid){
				var adamHolder = adam.findHolder;
				if(adamHolder){
					if(adamHolder.race == constants.RACE_HUMAN){
						adamMsg = "We've been sent an item that will help you catch the damane, but I've already entrusted it to "+adamHolder.name+".  Find them and assist us in this quest.";
					}
				}
			}
		}
		var players = global.lsPlayers;
		var head = "Agelmar, Lord of Fal Dara";
		var script = ["I am looking for brave warriors to round these women up and bring them to a rendezvous point in Tarwin's Gap for transport to the White Tower.  The Amyrlin wishes to study them.", "Lord Marshall Uno is waiting in Tarwin's Gap. He will give you further instructions when you bring the damane to him.", adamMsg];
	}else{ // trolls
		var adamMsg = "The Dreadlord has leashes for the pink ones. Come see me to get one.";
		var adam = global.lsAdam;
		if(adam){
			if(adam.isValid){
				var adamHolder = adam.findHolder;
				if(adamHolder){
					if(adamHolder.race == constants.RACE_HUMAN){
						adamMsg = "The Dreadlord has leashes for the pink ones, but I've already give one to "+adamHolder.name+".  Find them.";
					}
				}
			}
		}
		var players = global.dsPlayers;
		var head = "Murash";
		var script = ["The Dreadlord wants these scum for his own pleasure. Round them up and bring them to Tarwin's Gap for transport.", "Syyggar is waiting there. He will give you further instructions when you bring the damane to him.", adamMsg];; 
	}
	if(arrContains(players, actor)){
		actor.send("You've already joined the global event!");
		return;
	}
	global.lsPlayers.push(actor);
	actor.send(cyn+bld+"You join the global event!"+nrm);
	actor.send(" ");
	actor.send("We have had a report of six damane that escaped and are traveling around the world.");
	actor.send(" ");
	actor.send(script[0]);
	actor.send(" ");
	actor.send(script[1]);
	actor.send(" ");
	actor.send(script[2]);
}