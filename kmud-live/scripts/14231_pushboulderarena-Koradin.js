var script14231 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
getCharCols(actor);
_block;
if (!vArgs[1] || vArgs[2]) {
	actor.send("Try to "+mag+"push"+nrm+" the"+mag+" boulder"+nrm+" instead.");
	return;
}
if (strn_cmp(vArgs[1],"boulder",7) || strn_cmp(vArgs[1],"rock",4)) {
	if (actor.position != constants.POS_STANDING) {
		actor.send("You're not in the position to do that right now!");
		return;
	}
	if (getSval(self,14231,"boulder") == 1) {
		actor.send("It's already in motion!");
		return;
	}
	if (actor.class == constants.CLASS_CHANNELER) {
		if (!actor.hasSource()) {
			if (actor.sex == constants.SEX_MALE)
				actor.comm("seize");
			else
				actor.comm("embrace");
		}
		var startMsgYou = "You channel flows of Air, directing them toward the boulder...";
		var startMsgAll = "$n takes on a look of concentration, focusing on a boulder...";
		var endMsgYou = "\nYour thick weave of Air pushes the boulder over the brink, sending it crashing down the path below!";
		var endMsgAll = "$n channels flows of Air to push a boulder over the edge, sending it crashing down the path below!";
	}
	else {
		var startMsgYou = "You position yourself behind the boulder and push with all your might...";
		var startMsgAll = "$n steps behind a boulder and begins to shove, throwing $s shoulder into it...";
		var endMsgYou = "\nYour efforts push the boulder over the brink, sending it crashing down the path below!";
		var endMsgAll = "$n gives one last heave, sending the boulder crashing down the path below!";
	} 
	actor.send(startMsgYou);
	act(startMsgAll, true, actor, null, null, constants.TO_ROOM);
	actor.startTimer(2.5);
	var success = runTimer(actor);
	if (!success) {
		actor.send("You give up the attempt.");
		return;
	}
	actor.send(endMsgYou);
	act(endMsgAll, true, actor, null, null, constants.TO_ROOM);
	self.setRetoolDesc("An enormous boulder races downhill, crashing out of sight.");
	setSval(self,14231,"boulder",1);
	if (self.room.vnum == 14519)
		var rooms = [14520,14521,14522,14523,14524,14525,14526,14527,14500];
	else
		var rooms = [14513,14512,14511,14510,14509,14508,14507,14506,14505];
	var roomCounter = 0;
	while (roomCounter < 9) {
		var newRoom = getRoom(rooms[roomCounter]);
		wait 2;
		self.moveToRoom(newRoom);
		newRoom.echo("An enormous boulder crashes into view from up the path!");
		for each (var player in newRoom.people) {
			if (player.vnum == -1 && random(1,5) != 1) {
				var dodge = 33;
			} 
			else {
				var dodge = 100;
			}
			player.stopFighting();
			var dmg = random(80,100);
			if (random(1,100) < dodge) {
				player.send("You dive to the side, letting the boulder roll past.");
				act("$n dives out of the way, letting the boulder roll past.",true,player,null,null,constants.TO_ROOM);
			}
			else {
				getCharCols(player);
				if (player.hps - dmg < 1) {
					player.send(red+"You barely have time to hear your bones snap before the boulder flattens you like a pancake!"+nrm);
					act("$n gets caught under the crashing boulder with the sickening sound of snapping bones.",true,player,null,null,constants.TO_ROOM);
				}
				else {
					player.send(red+"You're knocked to the side, landing in a heap."+nrm);
					act("$n is knocked to the side as the boulder rushes past.",true,player,null,null,constants.TO_ROOM);
				}
				player.damage(dmg, actor);
				player.position = constants.POS_SITTING;
			}
		}
		roomCounter += 1;
	}
	self.room.echo("The boulder collides with a rocky wall, breaking into small chunks.");
	self.moveToRoom(getRoom(20899));
	self.extract();
}
else {
 actor.send("Try to "+mag+"push"+nrm+" the"+mag+" boulder"+nrm+" instead.");
 return;
}

};

