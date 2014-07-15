var script552 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( vArgs.length < 2 )
		return;
	var target = getCharInListVis(self,vArgs[1],here.people);
	wait 1;
	if( !target ) {
		self.say("Who?");
		return;
	}
	act("$n takes off your clothes.", false, self, null, target, constants.TO_VICT);
	act("$n takes off $N's clothes.", false, self, null, target, constants.TO_NOTVICT);
	wait 1;
	var rand = random(1,4);
	if( rand == 1 ) {
		act("$n does you slowly.", false, self, null, target, constants.TO_VICT);
		act("$n does $N slowly.", false, self, null, target, constants.TO_NOTVICT);
	}
	else if( rand == 2 ) {
		act("$n makes you suck his thang.", false, self, null, target, constants.TO_VICT);
		act("$n makes $N suck his thang.", false, self, null, target, constants.TO_NOTVICT);
	}
	else if( rand == 3 ) {
		act("$n whips your ass and does you hard.", false, self, null, target, constants.TO_VICT);
		act("$n whips $N's ass and does $M hard.", false, self, null, target, constants.TO_NOTVICT);
	}
	else if( rand == 4 ) {
		act("$n licks $s finger and shoves it up far.", false, self, null, target, constants.TO_VICT);
		act("$n licks $s finger and shoves it up far.", false, self, null, target, constants.TO_NOTVICT);
	}
	wait 1;
	act("You love it.", false, self, null, target, constants.TO_VICT);
	act("$N loves it.", false, self, null, target, constants.TO_NOTVICT);
	/**
	%(send%) %(arg1%) %(self.name%) takes off your clothes.
	%(echoaround%) %(arg1%) %(self.name%) takes off %(arg1%)'s clothes.
	wait 1s
	set rand %(random.4%)
	if (%(rand%) == 0)
	%(send%) %(arg1%) %(self.name%) does you slowly.
	%(echoaround%) %(arg1%) %(self.name%) does %(arg1%) slowly.
	elseif (%(rand%) == 1)
	%(send%) %(arg1%) %(self.name%) makes you suck his thang.
	%(echoaround%) %(arg1%) %(self.name%) makes %(arg1%) suck his thang.
	elseif (%(rand%) == 2)
	%(send%) %(arg1%) %(self.name%) whips your ass and does you hard.
	%(echoaround%) %(arg1%) %(self.name%) whips %(arg1%) and does him hard.
	elseif (%(rand%) == 3)
	%(send%) %(arg1%) %(self.name%) licks his finger and shoves it up far.
	%(echoaround%) %(arg1%) %(self.name%) licks his finger and shoves it up far.
	end
	wait 1s
	%(send%) %(arg1%) You love it.
	%(echoaround%) %(arg1%) %(arg1%) loves it.
	**/
}