var script9851 = function(self, actor, here, args, extra)
{
		_block;
	var vArgs = getArgList(args);
	if ( isName(vArgs[1],self.namelist) != true ) {
		actor.send("Unfold what?");
	}
	else {
		getCharCols(actor,constants.CL_OFF);
		actor.send(bld+"You unfold "+self.name+"."+nrm);
		waitpulse 2;
		actor.send(bld+"There is some sort of message written on the paper, but it's meaning is currently unclear. The heading reads: "+cyn+"'Just think.'"+nrm);
		waitpulse 14;
		actor.send("You slide the letter into the pages of your journal for safekeeping.");
		self.extract();
		actor.journalEdit("ADD","Just Think...");
	}

};

