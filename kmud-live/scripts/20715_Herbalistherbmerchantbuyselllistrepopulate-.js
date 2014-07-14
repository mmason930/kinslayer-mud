var script20715 = function(self, actor, here, args, extra)
{
	self.deletePval("herblist",true);
var forSale = [];
var itemList = [20985,20986,20987,20988,20989,20990,20991,20992,20993,20994,20995,20996,20997,20998];
for (i=0;i<9;i++)
{
forSale.push(itemList[random(0,itemList.length-1)]);
}
self.setPval("herblist",forSale.join("~"),true);

};

