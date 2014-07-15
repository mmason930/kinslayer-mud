var script1216 = function(self, actor, here, args, extra) {
	_block;
	var ch = actor;
	var room = here;
	var obj = self;
	if( ch.fighting ) {
		ch.send("You're too busy to try pulling anything!");
		return;
	}
	if( getSval(obj, 1216, "pulling") == "1" ) {
		ch.send("Wait your turn!");
		return;
	}
	if( getSval(obj, 1216, "pulling") == "2") {
		ch.send("What?!?");
		return;
	}
	var timerLength = 60 + (21-ch.strength)*10;
	var guardWait = random(20, 30);
	setSval(obj, 1216, "pulling", "1");
	setTimeout(guardWait, summonGuardian, [ch, room]);
	setTimeout(guardWait*3, summonGuardian, [ch, room]);
	gecho("You suddenly get the feeling that somewhere, " + ch.name + " is attempting to pull an ancient shocklance out of the ground.", gechoPredicate);
	ch.startTimer(timerLength);
	var success = runTimer(ch);
	if(success){
		act("You successfully pull the shocklance from the ground!", true, ch, null, null, constants.TO_CHAR);
		act("With a mighty heave, " + ch.name + " pulls the ancient shocklance from the ground!", true, ch, null, null, constants.TO_ROOM);
		obj.moveToChar(ch);
		obj.setRetoolName("");
		obj.setRetoolDesc("");
		obj.setRetoolExDesc("");
		setSval(obj, 1216, "pulling", "2");
	}
	else {
		ch.send("Cancelled...");
		setSval(obj, 1216, "pulling", "0");
	}
	function gechoPredicate(ch) {
		if( ch.level < 10 ) {
			return false;
		}
		else {
			return true;
		}
	}
	function summonGuardian(args) {
		args[1].echo( "A mechanical spider suddenly climbs out from a hole in the ground!" );
		var guard = args[1].loadMob(1215);
		setTimeout(16, guardianKill, [args[0], guard]);
	}
	function guardianKill(args) {
		args[1].comm("kill " + args[0].name);
	}
}