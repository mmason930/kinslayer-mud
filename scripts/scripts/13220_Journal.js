var script13220 = function(self, actor, here, args, extra) {
	//Alder December 2009
	var vArgs = getArgList(args);
	_block;
	if ( actor.position != constants.POS_SITTING && actor.position != constants.POS_STANDING && actor.position != constants.POS_RESTING ) {
		actor.send("Now isn't the time to be reading your journal.");
		return;
	}
	if ( vArgs.length != 1 ) {
		actor.send("Type"+actor.cyan(constants.CL_OFF)+" journal "+actor.normal(constants.CL_OFF)+"if you are trying to access your quest journal.");
		return;
	}
	if ( !actor.journal ) {
		// actor.send("You don't have a virtual journal at the moment.");
		actor.recreateJournal();
	}
	//Monitoring (Alder)
	var alder = global.alder;
	if ( alder ) {
		getCharCols(alder);
		alder.send(grn+"[ "+actor.name+" accessing journal. ]"+nrm);
	}
	//*****************
	actor.attach(13214);
	act("$n begins paging through a leather-bound journal.",true,actor,null,null,constants.TO_ROOM);
	actor.dispJournal("reread");
}