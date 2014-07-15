var script6693 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var action = random(1, 2);
	var vict = actor;
	var target = self;
	var x = 0;
	if(!vict)
		return;
	else if( action == 1 )
	{
		_block;
		act("As you attempt a bash, you look into $n's eyes, and suddenly all thought of attack dissolves away.", false, self, null, vict, constants.TO_VICT);
		act("$N runs toward $n to attempt a bash, but suddenly stops, a look of adoration on $S face.", false, self, null, vict, constants.TO_NOTVICT);
		waitpulse 3;
		vict.lag(24);
		act("$N seems to flicker for a moment...", false, self, null, vict, constants.TO_NOTVICT);
		vict.moveToRoom(getRoom(6694));
		vict.comm("look");
		wait 2;
		var target = self;
		while( target == self || target.leader == self && x < 10 )
		{
			target = getRandomPersonInRoom(self.room);
			x++;
		}
		vict.moveToRoom(getRoom(6693));
		vict.comm("look");
		vict.comm("kill " + target.name);
		wait 2;
		vict.send("Suddenly, you feel more in control of yourself.");
		return;
	}
}