var script20815 = function(self, actor, here, args, extra)
{
	wait random(0,3);
if (self.items[0])
	self.items[0].extract();
wait random(1,3);
if (self.people[0]) {
	if (self.people[0].vnum > 0)
		self.people[0].extract();
}


};

