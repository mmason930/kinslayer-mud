var script4001 = function(self, actor, here, args, extra) {
	// March 2011, Fogel - Port of the Kick skill, with some enhancements
	_block;
	var ch = actor;
	var room = here;
	var vict;
	var preFighting = ch.fighting ? true : false;
	var skill = ch.getSkill( getSkillVnum("Kick") );
	var vArgs = getArgList(args);
    var now = time();
	var lastKickUnixTimestamp = getSval(ch, 4001, "lastKickUnixTimestamp") || 0;
    var secondsSinceLastKick = now - lastKickUnixTimestamp;
    var secondsToWaitPerKick = 12;
    var secondsRemainingToWait = Math.max(0, secondsToWaitPerKick - secondsSinceLastKick);

	if( skill == 0 ) {
		ch.send("You have no idea how.");
		return;
	}
	if( secondsRemainingToWait > 0 ) {
		ch.send("You still have another " + secondsRemainingToWait + " second" + (secondsRemainingToWait == 1 ? "" : "s") + " before you can do that again.");
		return;
	}
	if( room.isFlagged(constants.ROOM_PEACEFUL) ) {
		ch.send("This room just has such a peaceful, easy feeling...");
		return;
	}
	if( ch.fighting && vArgs.length == 1 ) {
		vict = ch.fighting;
	}
	else {
		vict = ch.getCharRoomVis(vArgs[1]);
	}
	if( !vict ) {
		if( vArgs.length == 1 )
			ch.send("Kick who?");
		else
			ch.send("No-one by that name here.");
		ch.lag(1);
		return;
	}
	if( vict == ch ) {
		ch.send("Aren't we funny today...");
		return;
	}
	if( vict.position >= constants.POS_FLYING ) {
		ch.send("They're too high off the ground to reach!");
		return;
	}
	if( ch.fighting && ch.fighting != vict && vict.fighting != ch ) {
		ch.send("You can't kick someone you aren't fighting!");
		return;
	}
	vict = ch.faceoff(vict);
	var offense = random(0, skill);
	var defense = vict.level/3;
	var damage = 10;
	if( defense > offense ) {
		act(ch.green(constants.CL_OFF)+"You miss your kick at $N's groin, much to $S relief..."+ch.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_CHAR);
		act("$n misses a kick at $N's groin, much to $N's relief...", true, ch, null, vict, constants.TO_NOTVICT);
		act(vict.red(constants.CL_OFF)+"$n misses a kick at your groin, you breathe lighter now..."+vict.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_VICT);
		vict.damage(0, ch);
	}
	else {
		if( vict.hps - damage > -3 ) {
			act(ch.green(constants.CL_OFF)+"Your kick hits $N in the solar plexus!"+ch.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_CHAR);
			act("$n kicks $N in solar plexus, $N is rendered breathless!", true, ch, null, vict, constants.TO_NOTVICT);
			act(vict.red(constants.CL_OFF)+"You're hit in solar plexus, wow, this is breathtaking!!"+vict.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_VICT);
		}
		else {
			act(ch.green(constants.CL_OFF)+"Your kick at $N's face splits $S head open -- yummy!"+ch.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_CHAR);
			act("$n neatly kicks $N's head into pieces -- YUMMY!", true, ch, null, vict, constants.TO_NOTVICT);
			act(vict.red(constants.CL_OFF)+"$n aims a kick at your face which splits your head in two!"+vict.normal(constants.CL_OFF), true, ch, null, vict, constants.TO_VICT);
		}
		vict.comm("");
		vict.damage(damage, ch);
	}
	ch.setFighting(vict);
	ch.lag(8);

	setSval(ch, 4001, "lastKickUnixTimestamp", now);
}