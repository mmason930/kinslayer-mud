var script590 = function(self, actor, here, args, extra) {
	waitpulse random(1,32);
	var worksInManor = false;
	for each (var manor in global.manors) {
		if (here.zoneVnum == manor.zoneId)
			worksInManor = true;
	}
	if (worksInManor == true) {
		self.room.echo("The fireworks fizzle out without exploding. Must have been a dud!");
		self.extract();
		return;
	}
	here.zecho("A huge explosion sounds somewhere in the distance, followed by a blinding flash of light.");
	if( self.findHolder ) {
		here = self.findHolder.room;
	    here.echoaround(self.findHolder, self.findHolder.name + " lights up as " + self.name + " explode in " + "his" + " hand!");
	    self.findHolder.send(self.name + " explode right in your hand!");
	    self.findHolder.damage(random(50,500));
	}
	else here = self.inRoom;
	var movable = here.people;
	for(var i = 0;i < here.items.length;++i) {
	    if( here.items[i].canWear( constants.ITEM_WEAR_TAKE ) ) {
	        movable.push(here.items[i]);
	    }
	}
	for(var i = 0;i < movable.length;++i) {
	    var dist = random(1, Math.max(1, 20 - movable[i].weight / 20));//Maximum of 20 rooms, min of 1
	    var tRoom = here;
	    for(;dist;--dist) {
	        if( here.neighbors.length > 0 ) {
	            var toDir = random(0,6);
	            for(var p = 0;p < 5;++p) {
	                toDir = random(0,6);
	                if( tRoom.direction(toDir) && !tRoom.doorExists(toDir)) {
	                    tRoom = tRoom.neighbors[toDir];
	                    break;
	                }
	            }
	        }
	    }
		if (worksInManor == true)
			tRoom = here;
	    if( getObjectClass(movable[i]) == "JSCharacter" ) {
	        if( tRoom == here )
	            movable[i].send("You are thrown against something nearby.");
	        else
	            movable[i].send("You are thrown into the air and crash into the ground.");
	    }
	    if( getObjectClass([movable[i]]) == "JSCharacter" ) {//Only echo AROUND if this is a character
	        here.echoaround(movable[i], capFirstLetter(movable[i].name) + " is flung from the room by the enormous explosion!");
	    } else {//Otherwise, echo to the entire room...
	        here.echo( capFirstLetter(movable[i].name) + " is flung from the room by the enormous explosion!");
	    }
	    tRoom.echo(movable[i].name + " falls into the room from afar, slamming into something nearby!");
	    movable[i].moveToRoom(tRoom);
	    if( movable[i].comm ) {
	        movable[i].comm("look");
	    }
	}
	self.extract();
	
	
	
}