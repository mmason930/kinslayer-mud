var script13257 = function(self, actor, here, args, extra) {
	var vArgs = getArgList( args );
	var obj;
	var here = here;
	_block;
	if( !(obj = actor.hasItem(23261,true,true,false)) ) {
		actor.send("You need a sledgehammer, worn or in your inventory, to do that.");
		return;
	}
	var questName = "A Hindering Blockade";
	if( actor.questInProgress(questName) == false || actor.quest(questName) >= 10 ) {
		actor.send("Shatter that wall? For what purpose?");
		return;
	}
	if( vArgs.length < 2 ) {
		actor.send("Which direction?");
		return;
	}
	for(var i = 0;i < 4;++i )
	{
		waitpulse 6;
		actor.send("You raise " + obj.name + ", preparing to swing...");
		act("$n raises $p, preparing to swing...", true, actor, obj, null, constants.TO_ROOM);
		actor.startTimer(3);
		var success = runTimer(actor);
		if( !strn_cmp(vArgs[0], "north", 1) )
		{
			if( success ) {
				act("\r\nYou slam $p hard against the wall.", false, actor, obj, null, constants.TO_CHAR );
				act("Rubble crackles as it falls to the ground after $n pounds against the wall to the north.", true, actor, obj, null, constants.TO_ROOM);
			}
			else {
				return;
			}
		}
		else {
			actor.send("\r\nYou let loose, but to no avail.");
			act("$n swings, but to no avail.", true, actor, null, null, constants.TO_ROOM);
			return;
		}
	}
	var people = here.people;
	for(var i = 0;i < people.length;++i) {
		getCharCols(people[i], constants.CL_NORMAL);
		var hilt = here.loadObj( 13202 );
		people[i].send(bld + cyn + capFirstLetter(hilt.name) + " falls to the ground from behind the shattered wall." + nrm);
		actor.qval(questName, 10);
	}
}