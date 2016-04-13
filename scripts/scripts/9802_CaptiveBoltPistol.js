var script9802 = function(self, actor, here, args, extra) {
	if ( str_cmp("Alder",actor.name) ) {
		_noblock;
		return;
	}
	_block;
	var vArgs = getArgList(args);
	if ( strn_cmp(vArgs[0],"shoot",2) == true ) {
		var target = getCharInListVis(actor,vArgs[1],here.people);
		if ( target ) {
			
			target.send(actor.name+" presses "+self.name+" to your forehead and toggles a lever, ejecting a small metal rod which obliterates your brain.");
			act("$n slowly raises $p and presses it to $N's forehead.", true, actor, self, target, constants.TO_NOTVICT);
			act("You slowly raise $p and press it to $N's forehead.", true, actor, self, target, constants.TO_CHAR );
			act("$n slowly raises $p and presses it to your forehead.",true,actor,self,target,constants.TO_VICT );
			waitpulse 2;
			if ( actor.room == target.room ) {
				act("PFTHH! $n releases the pressure on $p, ejecting the bolt and oblitering $N's brain.",true,actor,self,target,constants.TO_NOTVICT);
				getCharCols(actor);
				act(grn+"\nPFTHH! You release the pressure on $p, ejecting the bolt and obliterating $N's brain."+nrm, true, actor, self, target, constants.TO_CHAR );
				getCharCols(target);
				act(red+"PFTHH! $n releases the pressure on $p, ejecting the bolt and obliterating your brain."+nrm,true,actor,self,target,constants.TO_VICT );
				target.damage(5000,actor);
				// target.hps = 0;
				actor.experience += (target.level)*10000;
				return;
			}
			else {
				act("$n lowers $p.", true, actor, self, target, constants.TO_NOTVICT);
				act("You lower $p.", true, actor, self, target, constants.TO_CHAR );
				act("$n lowers $p.",true,actor,self,target,constants.TO_VICT );
				return;
			}
		}
		actor.send("Who do you intend to kill?");
		return;
	}
	else if ( strn_cmp(vArgs[0],"breach",2) == true ) {
		_block;
		var doorDir = here.doorName_cmp(vArgs[1]);
		if ( doorDir == -1 ) {
			actor.send("Breach what?");
			return;
		}
		if ( here.doorPick(doorDir) == 0 ) {
			actor.send("Why don't you just open it?");
			return;
		}
		if ( here.doorIsClosed(doorDir) == false ) {
			actor.send("It's open you idiot.");
			return;
		}
		var startRoom = here;
		var doorName = here.doorName(doorDir);
		act("$n slowly raises $p and presses it to the lock on the "+doorName+".", true, actor, self, null, constants.TO_ROOM);
		act("You slowly raise $p and press it to the lock on the "+doorName+".", true, actor, self, null, constants.TO_CHAR );
		waitpulse 2;
		if ( actor.room == startRoom ) {
			act("PFTHH! $n releases the pressure on $p, blowing out the lock on the "+doorName+".",true,actor,self,null,constants.TO_ROOM);
			getCharCols(actor);
			act(grn+"\nPFTHH! You release the pressure on $p, blowing out the lock on the "+doorName+"."+nrm, true, actor, self, null, constants.TO_CHAR );
			here.unlockDoor(doorDir);
			var vicRoom = here.direction(doorDir);
			var vic = getRandomPersonInRoom(vicRoom);
			if ( vic != null ) {
				getCharCols(vic,constants.CL_OFF);
				vic.send(red+"A piece of metal slams into you. Blood and ashes!"+nrm);
				vicRoom.echoaround(vic,vic.name+" is hit with a piece of shrapnel. That looks like it hurt.");
				vic.damage(random(20,150));
			}
			return;
		}
		else {
			act("$n lowers $p.", true, actor, self, null, constants.TO_ROOM);
			act("You lower $p.", true, actor, self, null, constants.TO_CHAR );
			return;
		}	
	}
}