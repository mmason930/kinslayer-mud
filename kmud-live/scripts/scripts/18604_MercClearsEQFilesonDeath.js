var script18604 = function(self, actor, here, args, extra) {
	//Clears Merc EQ Files on Death
	//Rhollor - December 2009
	var owner = getSval(self,18600,"owner");
	if(owner) {
		fwrite(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"), "");
	}
}