var script20717 = function(self, actor, here, args, extra)
{
	self.deletePval("clothlist",true);
var forSale = [];
var itemList = [4905,4910,4911,4912,4913,4914,4915,4916,4917,4918,4919,4920,4921,4922,4923,4924,4925,4926,4927,4928,4929,4930,4931,4932,4933,4934,4935,4936,4937,4938,4939];
for (i=0;i<9;i++)
{
forSale.push(itemList[random(0,itemList.length-1)]);
}
self.setPval("clothlist",forSale.join("~"),true);

};

