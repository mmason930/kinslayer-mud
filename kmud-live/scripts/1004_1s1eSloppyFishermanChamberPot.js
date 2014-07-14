var script1004 = function(self, actor, here, args, extra)
{
	var ch = actor;
if( getSval( ch, 300, "chamber pot") != 1 ) {
	setSval(ch, 300, "chamber pot", 1)
	waitpulse 30;
	if (ch.room.vnum == 21477)
		if (ch.dodge > 100)
			ch.send("You narrowly dodge excrement thrown from a chamber pot out a second story window.")
	else
		if (ch.room.vnum == 21477)
			ch.send("You are hit by excrement thrown from a chamber pot out a second story window.")
	setSval(ch, 300, "chamber pot", 0);
}

};

