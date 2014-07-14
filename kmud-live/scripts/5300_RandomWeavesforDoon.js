var script5300 = function(self, actor, here, args, extra)
{
	var vict = self.fighting;
if( !vict )
	return;
else
{
	waitpulse random(1, 5);
	act("$n grins evilly as he throws a black orb of fire at you, engulfing your body in tainted flames! OUCH!", false, self, null, vict, constants.TO_VICT);
	act("$n hurls a pulsing black fireball at $N!, who is engulfed in sable flames!", false, self, null, vict, constants.TO_NOTVICT);
	vict.damage( random(25, 35), self );
	return;
}

};

