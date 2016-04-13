var script18606 = function(self, actor, here, args, extra) {
	waitpulse 1;
	var owner = getSval(self,18600,"owner");
	if(owner.level > 99) {
		owner.send("For: " + owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum")));
		owner.send("Before " + fread(owner.id + ".mercEQ." + parseInt(getSval(self,18600,"mercNum"))));
	}
	self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
	if(owner.level > 99)
		owner.send("After " + fread(owner.id + ".mercEQ." + getSval(self,18600,"mercNum")));
}