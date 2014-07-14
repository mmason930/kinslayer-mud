var script13387 = function(self, actor, here, args, extra)
{
	self.comm("rem shoufa");
for each(var item in self.inventory)
	item.extract();
self.loadObj(43);
self.comm("wear hood");

};

