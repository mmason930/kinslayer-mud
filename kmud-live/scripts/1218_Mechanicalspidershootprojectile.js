var script1218 = function(self, actor, here, args, extra)
{
	var ch = self;
var vict = ch.fighting;
waitpulse random(2, 5);
if( vict.vnum < 0 ) {
	act("$n fires a sharp metal projectile at you! Ouch!", true, ch, null, vict, constants.TO_VICT);
	act("$n fires a sharp metal projectile at $N, who yelps in pain!", true, ch, null, vict, constants.TO_NOTVICT);
	vict.comm("");
	vict.damage(random(5, 10));
}
else {
	act("$n fires a sharp metal projectile at $N's neck, severing $S head from $S body!", true, ch, null, vict, constants.TO_NOTVICT);
	vict.damage(10000);
}

};

