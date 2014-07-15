var script9884 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//Jump the Fence: Kill Undercover Guard
	var qName = "Jump the Fence";
	var args = getArgList(args);
	if ( actor.quest(qName) != 1 || !actor.questTaskProgress(qName)[3].comp || !isName(args[1],self.namelist) ) {
		return;
	}
	_block;
	getCharCols(actor,constants.CL_OFF);
	actor.send(grn+"You jab "+self.name+" in the throat, crushing her windpipe!"+nrm);
	echoaround(actor,actor.name+" jabs "+self.name+" in the throat, crushing her windpipe!"+nrm);
	self.loadObj(9884);
	actor.warrantByClan(2);
	actor.send("You have been warranted by Andor!");
	actor.exp(500000);
	self.damage(500,actor);
}