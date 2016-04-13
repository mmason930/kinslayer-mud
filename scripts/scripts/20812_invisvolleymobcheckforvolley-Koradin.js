var script20812 = function(self, actor, here, args, extra) {
	actor = getSval(self, 20802, "actor");
	if (getSval(actor, 20802, "volleying") == 0)
	{
		self.extract();
		return;
	}
	
}