var script20721 = function(self, actor, here, args, extra) {
	self.deletePval("mixlist",true);
	var forSale = [];
	var i = 0;
	var noSale = [2707,2708,2709,2717,2718,2719,2723,2724,2728,2729]
	while (i < 9)
	{
		var obj = random(2700,2799);
		if (getObjProto(obj) && !arrContains(noSale,obj))
		{
			forSale.push(obj);
			i++;
		}
	}
	self.setPval("mixlist",forSale.join("~"),true);
	
}