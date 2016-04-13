var script18614 = function(self, actor, here, args, extra) {
	var owner = getSval(self,18600,"owner");
	if ( owner == undefined ) {
		return;
	}
	if( owner.level > 99 ) {
	        owner.send("For: " + owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum")));
	        owner.send("Before " + fread(owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum"))));
	}
	self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
	if(owner.level > 99)
	        owner.send("After " + fread(owner.id + ".mercEQ." + getSval(self,18600,"mercNum")));
	waitpulse 1;
	var owner = getSval(self,18600,"owner");
	if(owner.level > 99) {
	        owner.send("For: " + owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum")));
	        owner.send("Before " + fread(owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum"))));
	}
	self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
	if(owner.level > 99)
	        owner.send("After " + fread(owner.id + ".mercEQ." + getSval(self,18600,"mercNum")));
	//Attaches to a PC and removes their merc EQ files on extraction
	//Rhollor - December 2009
	var myMercs = [];
	myMercs = getSval(self,-18600,"myMercs");
	for(var i=0; i < myMercs.length; i++) {
	        myMercs[i].extract();
	}
	//The number of files to check for data
	var maxMercs = 30;
	var fileName = (self.id + ".mercEQ.");
	if(getSval(self,18601,"numMerc")) {
	        //Just making sure they know they are losing their saves...
	        self.send(self.red(constants.CL_OFF) + "You are losing any mercenaries you still own and they will not save their equipment!" + self.normal(constants.CL_OFF));
	}
	for(var i = 1; i <= maxMercs; i++) {
	        //Overwrite file with ""
	        fwrite(fileName + i, "");
	}
	//NEED TO ADD SOMETHING TO DECREASE MercsInGame
}