var script1911 = function(self, actor, here, args, extra) {
	var ch = self.wornBy;
	if(!ch) {
		return;
	}
	if( ch.fighting ) {
		act("A flash of light is reflected off your shield and hits $N's eyes, causing $M to cry out in pain!", false, ch, null, ch.fighting, constants.TO_CHAR);
		act("AAAARGH, a bright flash of light reflects off $n's shield and into your eyes, causing a sudden burst of pain!", false, ch, null, ch.fighting, constants.TO_VICT);
		act("A flash of light is reflected off $n's shield and hits $N's eyes, causing $M to cry out in pain!", false, ch, null, ch.fighting, constants.TO_NOTVICT);
		ch.fighting.comm("");
	}
}