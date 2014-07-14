var script8001 = function(self, actor, here, args, extra)
{
	_block;
actor.send("Ok.");
var doorGuard = getMobAtRoom(here.vnum,8012);
if( !doorGuard )
	return;
if( actor.race == constants.RACE_TROLLOC )
{
	wait 1;
	act("$n getures $N towards the door.", false, self, null, actor, constants.TO_ROOM);
	act("$n gestures you towards the door.", false, self, null, actor, constants.TO_CHAR);
	wait 2;
	doorGuard.comm("unlock irondoor");
	doorGuard.comm("open irondoor");
	wait 7;
	doorGuard.comm("close irondoor");
	doorGuard.comm("lock irondoor");
	wait 1;
	if( actor.room == here ) {
		doorGuard.say("I thought you were leaving.");
		wait 1;
		doorGuard.comm("peer " + actor.name);
	}
	else {
		doorGuard.say("He is gone, master.");
	}
}

};

