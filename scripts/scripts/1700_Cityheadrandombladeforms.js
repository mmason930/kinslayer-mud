var script1700 = function(self, actor, here, args, extra) {
	var vict = self.fighting;
	var form = random(1, 2);
	if( !vict )
		return;
	waitpulse (random(1,5));
	if (!vict)
		return;
	else if (vict.room !== self.room)
		return;
	else if( form === 1 )
	{
		act("$n swirls about with $s sword, slicing across your body!", false, self, null, vict, constants.TO_VICT);
		act("$n swirls about with $s sword, slicing across $N's body!", false, self, null, vict, constants.TO_NOTVICT);
		vict.damage( random(15, 25), self );
		return;
	}
	else if( form === 2)
	{
		act("$n drops to one knee while sliding your way and stabs you in the chest!", false, self, null, vict, constants.TO_VICT);
		act("$n slides on one knee to dodge a blow and stabs $N in the chest!", false, self, null, vict, constants.TO_NOTVICT);
		vict.damage( random(30, 40), self );
		return;
	}
}