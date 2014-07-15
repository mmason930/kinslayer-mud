var script10680 = function(self, actor, here, args, extra) {
	var rank = actor.getRank(7);
	var vict = self.fighting;
	var technique = random(0, 5);
	if( !vict )
	        return;
	else if( technique == 0 ) 
	{
	        waitpulse random(1, 5);
	        act("$n launches a counterattack at you, but misses by a hair!", false, self, null, vict, constants.TO_VICT);
	        act("$n launches a counterattack, missing $N's face by a hair!", false, self, null, vict, constants.TO_NOTVICT);
	        return;
	}
	else if( technique == 1 && rank >= 1 ) 
	{
	        waitpulse random(1, 5);
	        act("$n grabs your arm and reverses your stroke! Ouch!", false, self, null, vict, constants.TO_VICT);
	        act("$n counters $N's attack, slamming $s fist in $N's face!", false, self, null, vict, constants.TO_NOTVICT);
	        vict.damage( random(5, 15), self );
	        return;
	}
	else if( technique == 2 && rank >= 3 )
	{
	        waitpulse random(1, 5);
	        act("$n leaps into the air, kicking you in the solar plexus!", false, self, null, vict, constants.TO_VICT);
	        act("$n leaps into the air to dodge a blow and kicks $N in the chest!", false, self, null, vict, constants.TO_NOTVICT);
	        vict.damage( random(15, 25), self );
	        return;
	}
	else if( technique == 3 && rank >= 5 )
	{
	        waitpulse random(1, 5);
	        act("$n circles you, burying $s dagger in your vital organs!", false, self, null, vict, constants.TO_VICT);
	        act("$n circles $N, burying $s dagger in $N's vital organs!", false, self, null, vict, constants.TO_NOTVICT);
	        vict.damage( random(25, 35), self );
	        return;
	}
	else if( technique == 4 && rank >= 7 && !actor.affectedBy(constants.AFF_POISON) )
	{
	        waitpulse random(1, 5);
	        act("$n flicks his wrist fluidly, aiming a black-tipped dart at your neck!", false, self, null, vict, constants.TO_VICT);
	        act("$n flicks his wrist at $N, landing $s dart in $N's jugular vein!", false, self, null, vict, constants.TO_NOTVICT);
	        vict.affect(constants.AFF_POISON,2,0,0);
	        return;
	}
	else if( technique == 5 && rank >= 7 && !actor.affectedBy(constants.AFF_BLIND) )
	{
	        waitpulse random(1, 5);
	        act("$n slides his foot forward, kicking dirt into your eyes!", false, self, null, vict, constants.TO_VICT);
	        act("$n slides his foot toward $N, sending dirt into $N's eyes!", false, self, null, vict, constants.TO_NOTVICT);
	        vict.affect(constants.AFF_BLIND,2,0,0);
	        return;
	}
}