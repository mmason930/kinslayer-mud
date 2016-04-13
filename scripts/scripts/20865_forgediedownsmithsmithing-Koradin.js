var script20865 = function(self, actor, here, args, extra) {
	//self.room.echo(time());
	if (getSval(self,20863,"isLit") > 0) {
		if (getSval(self,20863,"isLit") < time()) {
			setSval(self,20863,"isLit",0);
			self.room.echo(capFirstLetter(self.name) + " dies down, the coals inside crumbling to ash.");
		}
	}
}