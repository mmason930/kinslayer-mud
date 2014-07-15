var script3189 = function(self, actor, here, args, extra) {
	_block;
	if( canDigInRoom(here) == false ) {
		actor.send("You can't dig here.");
		return;
	}
	var room = here;
	var shovelName = capFirstLetter(self.name);
	var digItem = null;
	actor.send("You begin chipping away at the ground...");
	act("$n begins chipping away at the ground...", false, actor, null, null, constants.TO_ROOM);
	var numberOfDigs=0;
	var foundSomething = false;
	while( true ) {
		++numberOfDigs;
		var timer = random(2,4);
		actor.startTimer( timer );
		var success = runTimer(actor);
		
		actor.send("");
		
		if( success ) {
		}
		else {
			actor.send("You stop digging.");
			act("$n stops digging.", false, actor, null, null, constants.TO_ROOM);
			return;
		}
		
		var roll = random(1,6);
		if( roll == 1 ) {//Message
		
			actor.send( shovelName + " hits a pile of small stones submerged in the ground.");
			act("$n hits a pile of small stones in the ground.", false, actor, null, null, constants.TO_ROOM);
		}
		else if( roll == 2 ) {//Message
		
			if( numberOfDigs >= 3 ) {
				actor.send( "The dirt to the side piles up as your hole gets deeper." );
				act("$n drops another scoop of dirt onto the pile by the side of the hole, which is now fairly deep.", false, actor, null, null, constants.TO_ROOM);
			}
			else {
				actor.send("You drop a scoop of dirt onto the ground, starting a small pile.");
				act("$n drops a scoop of dirt onto the ground, starting a small pile.", false, actor, null, null, constants.TO_ROOM);
			}
		}
		else if( roll >= 3 && roll <= 5 ) {//Message
			actor.send("You pick up another scoop of dirt and drop it beside the pile.");
			act("$n picks up another scoop of dirt and drops it beside the pile.", false, actor, null, null, constants.TO_ROOM);
		}
		else if( roll == 6 ) {//Success
		
			if( random(1,100) <= 25 ) {
				foundSomething=false;
			}
			else {
				digItem = getAndRemoveRandomDigItem( room );
				if( !digItem )
					foundSomething=false;
				else	
					foundSomething=true;
			}
			break;
		}
	}
	if( foundSomething ) {
		actor.send("You dig up " + digItem.name + ".");
		act("$n digs up $P.", true, actor, null, digItem, constants.TO_ROOM);
		digItem.moveToChar(actor);
	}
	else {
		actor.send("You dig and dig but find nothing.");
		act("$n digs and digs but finds nothing.", true, actor, null, null, constants.TO_ROOM);
	}
}