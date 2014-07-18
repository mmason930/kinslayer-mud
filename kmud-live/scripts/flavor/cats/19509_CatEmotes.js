var script19509 = function(self, actor, here, args, extra) {

	//Give priority to killing rats.
	//Galnor 2014-07-18

	var rats = here.getCharacters(function(character) {

		return character.vnum == 79;
	});

	if(rats.length != 0)
	{
		var rat = rats[0];

		act("$n sniffs at something on the ground...", true, self, null, null, constants.TO_ROOM);

		wait 2;

		if(rat.isValid && rat.room.vnum == self.room.vnum)
			self.hit(rat);

		return;
	}

	var rand = random(1,4);
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
}