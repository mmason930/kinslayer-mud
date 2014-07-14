var script1327 = function(self, actor, here, args, extra)
{
	var ch = self.wornBy;
if(!ch) {
        return;
}
if( ch.fighting ) {
	if (ch.fighting.eq(18)) {
		act("You nimbly disarm $N with a flick of your sword.", false, ch, null, ch.fighting, constants.TO_CHAR);
        act("$n flicks $s sword, nimbly disarming you!.", false, ch, null, ch.fighting, constants.TO_VICT);
        act("$n flicks $s sword, nimbly disarming $N.", false, ch, null, ch.fighting, constants.TO_NOTVICT);
		ch.fighting.eq(18).moveToChar(ch.fighting);
	}
}

};

