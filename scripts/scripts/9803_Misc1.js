var script9803 = function(self, actor, here, args, extra) {
	var weapon = actor.eq( constants.WEAR_WIELD );
	if( !weapon || weapon.name !== "a 12 gauge double barreled remington" )
		return;
	_block;
	var vArgs = getArgList( args );
	if( vArgs.length < 2 ) {
		actor.send("Shoot whom?");
		return;
	}
	var target = getCharInListVis( actor, vArgs[1], here.people );
	if( !target ) {
		actor.send("There is nobody by that name here.");
		return;
	}
	if( target == actor ) {
		actor.send("Shoot yourself? Are you crazy???");
		return;
	}
	act("BANG! You fire your boomstick at $N!", false, actor, null, target, constants.TO_CHAR);
	act("BANG! $n fires his boomstick at $N!", false, actor, null, target, constants.TO_NOTVICT);
	act("BANG! $n fires his boomstick directly at you!", false, actor, null, target, constants.TO_VICT);
	var chance = 80;
	var roll = random(1,100);
	var timer = random(0,3);
	if( timer != 0 ) {
	//	waitpulse timer;
	}
	if( roll >= chance ) {
		act("$n narrowly avoids being hit.", true, target, null, null, constants.TO_ROOM);
		act("You narrowly avoid being hit.", false, target, null, null, constants.TO_CHAR);
	}
	else {
		act("$n is struck by the gunshot.", true, target, null, actor, constants.TO_NOTVICT);
		getCharCols( target );
		act(red + "You are struck by the gunshot." + nrm, true, target, null, null, constants.TO_CHAR);
		getCharCols( actor );
		act(grn + "$N is struck by your gunshot." + nrm, true, actor, null, target, constants.TO_CHAR);
		
		var dmg = random(100,800);
		if( target.hps - dmg < -100 )
			actor.extract();
		else
			target.hps -= dmg;
	}
}