var script1215 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
var ch = actor;
var vict;
var weapon = self;
if( here.isFlagged( constants.ROOM_PEACEFUL ) ) {
	ch.send("This room just has such a peaceful, easy feeling...");
	return;
}
if( !vArgs[1] && ch.fighting && ch.room == ch.fighting.room ) {
	vict = ch.fighting;
}
else if( !vArgs[1] || !(vict = ch.getCharRoomVis(vArgs[1])) ) {
	ch.send("Blast who?");
	ch.lag(2);
	return;
}
if( !vict || !(ch.room == vict.room && ch.canSee(vict)) ) {
	ch.send("They must have left already...");
	return;
}
if( vict == ch ) {
	ch.send("Your arms aren't long enough to aim at yourself and still shoot!");
	return;
}
if( getSval(weapon, 1215, "cooldown") == "true" ) {
	ch.send("Your weapon needs to recharge for a bit longer!");
	return;
}
ch.startTimer(3.0);
var success = runTimer(ch);
if(success){
	if( !ch.canSee(vict) ) {
		ch.send("You can no longer see your target!");
		return;
	}
	var dam = random(weapon.value(constants.VALUE_WEAPON_DAM_LOW), weapon.value(constants.VALUE_WEAPON_DAM_HIGH));
	vict.damage(dam);
	if( !ch.fighting ) {
		ch.setFighting(vict);
	}
	setSval(weapon, 1215, "cooldown", "true");
	setTimeout(80, expireCooldown, [ch, weapon]);
	act("You squeeze the base of $p, blasting $N with a piercing bolt of light!", true, ch, weapon, vict, constants.TO_CHAR);
	act("You feel your body spasm slightly as a bolt of light from $n's weapon blasts into you!", true, ch, weapon, vict, constants.TO_VICT);
	act("$n's weapon blasts $N with a bright bolt of light, who spasms in pain!", true, ch, weapon, vict, constants.TO_NOTVICT);
}
else {
	ch.send("Cancelled...");
}
function expireCooldown( args ) {
	setSval(args[1], 1215, "cooldown", "false");
	act("$p begins to emit a soft yellow light.", true, args[0], weapon, args[0], constants.TO_CHAR);	
}

};

