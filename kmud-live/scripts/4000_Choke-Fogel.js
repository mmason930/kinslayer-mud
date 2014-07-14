var script4000 = function(self, actor, here, args, extra)
{
	_block;

/* 7/28 Fogel - Temporarily disabled this skill until I can make it less gay */
actor.send("This skill has been temporarily disabled, sorry!");
return;
var vArgs = getArgList(args);
var ch = actor;
var vict;
if( !ch.getSkill( getSkillVnum("Choke") ) ) {
	ch.send("You would, but you don't know how!");
	return;
}
if( here.isFlagged( constants.ROOM_PEACEFUL ) ) {
	ch.send("This room just has such a peaceful, easy feeling...");
	return;
}
if( !vArgs[1] && ch.fighting && ch.room == ch.fighting.room ) {
	vict = ch.fighting;
}
else if( !vArgs[1] || !(vict = ch.getCharRoomVis(vArgs[1])) ) {
	ch.send("Choke who?");
	ch.lag(2);
	return;
}
if( ch.fighting && ch.fighting != vict && vict.fighting != ch ) {
	ch.send("You can't choke someone else while engaged!");
	return;
}
if( !vict || !(ch.room == vict.room && ch.canSee(vict)) ) {
	ch.send("They must have left already...");
	return;
}
if( vict == ch ) {
	ch.send("And what would that accomplish?");
	return;
}
if( vict.position == constants.POS_FLYING ) {
	ch.send("They're too high off the ground to reach!");
	return;
}
if( vict.affectedBy(constants.AFF_SHIELD) ) {
	ch.send("But they have already been choked!");
	return;
}
ch.startTimer(4.0);
var success = runTimer(ch);
if(success){
	var roll = random(1, 100) - (vict.level - ch.level);
	if( ch.room != vict.room ) {
		ch.send("Choke who?");
		return;
	}
	if( roll > 40 ) {
		// Success
		vict.damage(random(5, 15), ch);
		if( !ch.fighting ) {
			ch.setFighting(vict);
		}
		if( !vict.affectedBy(constants.AFF_SHIELD) && vict.channelingAbility() ) {
			var durationRounds = 4;
			var durationSeconds = durationRounds * 4;
			vict.affect( constants.AFF_SHIELD, durationSeconds, getSkillVnum("Choke") );
			setSval(vict, 4000, "chokeTime", durationSeconds );
			setTimeout( 8, updateChoke, vict );
			vict.send("You stumble and lose your connection to the True Source!");
			vict.removeSource();
		}
		act("You wrap your fingers around $N's throat, squeezing as hard as you can!", true, ch, null, vict, constants.TO_CHAR);
		act("$n begins choking you! You lose your focus!", true, ch, null, vict, constants.TO_VICT);
		act("$n wraps $s fingers around $N's throat, squeezing as hard as $e can.", true, ch, null, vict, constants.TO_NOTVICT);
	}
	else{
		if( !ch.fighting ) {
			ch.setFighting(vict);
		}
		ch.lag(8);
		if( vict.channelingAbility() ) {
			ch.damage(random(5, 10), vict);
			act("You attempt to grab $N's throat, but you are knocked back by a sudden gust of wind!", true, ch, null, vict, constants.TO_CHAR);
			act("$n attempts to choke you, but you push $m away with a quick pulse of Air.", true, ch, null, vict, constants.TO_VICT);
			act("$n tries to choke $N, but gets pushed back by a sudden gust of wind.", true, ch, null, vict, constants.TO_NOTVICT);
		}
		else {
			act("Your attempt to grab $N's throat results in a fist full of air.", true, ch, null, vict, constants.TO_CHAR);
			act("$n tries to choke you, but winds up choking the air instead.", true, ch, null, vict, constants.TO_VICT);
			act("$n tries to choke $N, but winds up choking the air instead.", true, ch, null, vict, constants.TO_NOTVICT);
		}
	}
}
else {
	ch.send("Cancelled...");
	return;
}
function updateChoke( vict ) {
	var chokeSeconds = getSval( vict, 4000, "chokeTime" ) - 1;
	vict.unaffect( constants.AFF_SHIELD );
	if( chokeSeconds > 0 ) {
		vict.affect( constants.AFF_SHIELD, chokeSeconds, getSkillVnum("Choke") );
		setSval( vict, 4000, "chokeTime", chokeSeconds );
		setTimeout( 8, updateChoke, vict );
	}
}


};

