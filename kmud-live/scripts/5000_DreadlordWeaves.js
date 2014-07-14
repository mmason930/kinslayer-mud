var script5000 = function(self, actor, here, args, extra)
{
	self.comm("cackle");
waitpulse 2;
self.say("The Shadow will destroy you.");
wait 2;
self.comm("emote gestures and begins to weave.");
wait 2;
if( !actor || actor.isValid == false ) {
	return;//They died.
}
var rand = random(1,5);
if( rand == 1 ) {
	act("$n smirks as $s fireball burns your skin.", false, self, null, actor, constants.TO_VICT);
	act("You smirk as your fireball burns $N's skin.", false, self, null, actor, constants.TO_CHAR);
	act("$n smirks as $s fireballs burns $N's skin.", false, self, null, actor, constants.TO_NOTVICT);
	actor.damage(35,self);
}
else if( rand == 2 ) {
	act("You are struck by the lightning called down by $n.", false, self, null, actor, constants.TO_VICT);
	act("$N is struck by your lightning.", false, self, null, actor, constants.TO_CHAR);
	act("$N is struck by the lightning called down by $n.", false, self, null, actor, constants.TO_NOTVICT);
	actor.damage(28,self);
}
else if( rand == 3 ) {
	self.comm("yell Rolling waves of Earth!");
	actor.send("You are struck by the waves.");
	act("$N is struck by the waves.", false, self, null, actor, constants.TO_NOTVICT);
	actor.damage(15,self);
}
else if( rand == 4 ) {
	act("A wave of boiling water leaps out of the air and hits $N.", false, self, null, actor, constants.TO_NOTVICT);
	act("A wave of boiling water leaps out of the air and hits you.", false, self, null, actor, constants.TO_VICT);
	actor.damage(8,self);
}
else if( rand == 5 ) {
	self.comm("emote summons a twister out of the air.");
	var nrHit = 0;
	var people = here.people;
	for(var i = 0;i < people.length && nrHit < 2;++i) {
		if( people[ i ].fighting == self ) {
			people[ i ].damage(15, self);
			++nrHit;
		}
	}
}

};

