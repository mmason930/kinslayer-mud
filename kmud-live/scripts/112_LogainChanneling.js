var script112 = function(self, actor, here, args, extra)
{
	var rand = random(1,5);
var fighting = self.fighting;
if( !fighting )
	return;
getCharCols(fighting);
if( rand == 1 ) {
	act("$n throws a huge fireball at $N.", false, self, null, fighting, constants.TO_NOTVICT);
	act(red + "$n throws a huge fireball at you." + nrm, false, self, null, fighting, constants.TO_VICT);
	act("You throw a huge fireball at $N.", false, self, null, fighting, constants.TO_CHAR);
	fighting.damage(70,self);
}
else if( rand == 2 ) {
	act("A huge bolt of lightning strikes $N.", false, self, null, fighting, constants.TO_NOTVICT);
	act(red + "You are struck by a huge bolt of lightning!" + nrm, false, self, null, fighting, constants.TO_VICT);
	act("You throw a huge bolt of lightning at $N!", false, self, null, fighting, constants.TO_CHAR);
	fighting.damage(60,self);
}
else if( rand == 3 ) {
	act("Cold gusts of wind slice $N's body!", false, self, null, fighting, constants.TO_NOTVICT);
	act(red + "Your body is cut by razor-sharp gusts of wind!" + nrm, false, self, null, fighting, constants.TO_VICT);
	act("You toss razor-sharp gusts of cold wind at $N, slicing $S body!", false, self, null, fighting, constants.TO_CHAR);
	fighting.damage(35,self);
}

};

