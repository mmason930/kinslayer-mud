var script9861 = function(self, actor, here, args, extra)
{
		if ( strn_cmp(getArgList(args)[0],"leave",4) == true ) {
		_block;
		getCharCols(actor,constants.CL_OFF);
		actor.send("You lift the locking mechanism on the hatch and push it outwards.");
		waitpulse 10;
		if ( actor.getPval("ZIND_PASS") != null ) {
			var word = global.alderQuestPasswords[random(0,global.alderQuestPasswords.length-1)];
			actor.setPval("ZIND_PASS",word,true);
			actor.send("Zind Ambla says 'Your new password is "+cyn+word+nrm+".'");
			waitpulse 5;
			actor.send("You record the word in your journal and leave through the hatchway.");
			waitpulse 3;
		}
		getRoom(21766).echo(capFirstLetter(actor.name) + " crawls out of a hatchway on the south wall.");
		actor.moveToRoom(getRoom(21766));
		actor.comm("look");
	}

};

