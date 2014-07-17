var script20713 = function(self, actor, here, args, extra) {
	self.deletePval("orelist",true);
	var forSale = [];
	var itemList = [4567,4568,4584,4525,4562,4569,4585,4586,4587,4588,4589,4590,4591,4592,4593,4594,4595,4596,4597,4598,4599,20835,20836,20837];
	for (i=0;i<9;i++)
	{
	forSale.push(itemList[random(0,itemList.length-1)]);
	}
	self.setPval("orelist",forSale.join("~"),true);
}