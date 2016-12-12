var script18600 = function(self, actor, here, args, extra) {
	//Sets initial conditions for mercenaries
	//Rhollor Dec 09
	MercUtil.setOwner(self, 0);
	MercUtil.setPrice(self, 100);
	MercUtil.setMercenaryNumber(self, -1);
	MercUtil.setHomeRoom(self, here.vnum);
};