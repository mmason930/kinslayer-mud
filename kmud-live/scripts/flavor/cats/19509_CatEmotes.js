var script19509 = function(self, actor, here, args, extra) {

	if(self.position != constants.POS_STANDING)
		return;
	//Give priority to killing rats.
	//Galnor 2014-07-18
	var svalName = "rat";
	var svalVnum = 19509;

	var ratHunting = getSval(self, svalVnum, svalName);

	if(ratHunting && ratHunting.room && ratHunting.room.vnum != self.room.vnum)
	{//Try to chase the rat...

		if(ratHunting.isValid && ratHunting.room.zoneVnum == here.zoneVnum)
		{
			var direction = here.firstStep(ratHunting.room);
			if(direction >= 0)
			{
				act("$n sniffs at something on the ground...", true, self, null, null, constants.TO_ROOM);

				wait 1;
				self.comm(dirToText(direction));
				return;
			}
		}

		//We can't get here if the cat chased. If we get here the rat could not be tracked. Forget about it.
		setSval(self, svalVnum, svalName, null);
		ratHunting = null;
	}

	var rat = null;

	//If we are in the room of the rat we are hunting, attack it.
	if(ratHunting != null && ratHunting.isValid && ratHunting.room.vnum == self.room.vnum)
		rat = ratHunting;
	else
	{//Otherwise, try to find another one.
		var rats = here.getCharacters(function(character) {

			return character.vnum == 79;
		});

		if(rats.length != 0)
			rat = rats[0];
	}

	if(rat)
	{//If there is a rat in the room, attack it!
		act("$n sniffs at something on the ground...", true, self, null, null, constants.TO_ROOM);

		wait 2;

		if(rat.isValid && rat.room.vnum == self.room.vnum) {
			self.setFighting(rat);
			setSval(self, svalVnum, svalName, rat);
		}

		return;
	}

	var rand = random(1,16);
	if( rand == 1 )
	    here.echo(self.name + " sniffs at something on the ground...");
	else if( rand == 2 )
	    here.echo(self.name + " purrs softly.");
	else if( rand == 3 )
	{
	    target = getRandomPersonInRoom(here);
	    if( target != self && self.canSee(target) )
	    {
	        here.echoaround(target, self.name + " growls at " + target.name + ".");
	        target.send(self.name + " growls at you.");
	    }
	}
};