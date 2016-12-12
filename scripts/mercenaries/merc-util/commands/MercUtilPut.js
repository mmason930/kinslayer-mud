var MercUtil = (function(MercUtil) {

	MercUtil.performPut = function(actor, target, vArgs, here, myMercObj)
	{
		var arg2 = vArgs[1], arg3 = vArgs[2];
		if(!arg2) {
			self.say("What do you want me to put where?!");
			return;
		}
		else {
			var allPoint = "";
			if(arg2 != "all") {
				if(arg2.split(".")[0] == "all") {
					arg2 = arg2.split(".")[1];
					allPoint = "all.";
				}
			}
			else
			{
				var cont = self.getObjWorn(arg3); // check worn containers first
				if(cont == null) {
					cont = getObjInListVis(self,arg3,self.inventory); //Try in inv second
				}
				if(cont == null) {
					//Check ground for cont...
					cont = getObjInListVis(self,arg3,here.items);
				}
				if(cont == null) {
					self.say("I don't seem to have a " + arg3 + " to put " + arg2 + " in.");
					return;
				}

				self.inventory.forEach(function(item) {
					self.comm("put " + item.namelist.split(" ")[0] + " " + arg3);
				});
				return;
			}
			var itemTarget = getObjInListVis(self,arg2,self.inventory);
			if(itemTarget== null) {
				self.say("I don't see a " + arg2 + " that I can put anywhere.");
			}
			else {
				//Found target item -> search for container to put into
				var cont = self.getObjWorn(arg3); // check worn containers first
				if(cont == null) {
					cont = getObjInListVis(self,arg3,self.inventory); //Try in inv second
				}
				if(cont == null) {
					//Check ground for cont...
					cont = getObjInListVis(self,arg3,here.items);
				}
				if(cont == null) {
					self.say("I don't seem to have a " + arg3 + " to put " + itemTarget.name + " in.");
					return;
				}
				else {
					//Have target obj and cont
					self.comm("put " + allPoint + arg2 + " " + arg3);
				}
			}
		}
		MercUtil.saveMercenaryEquipment(self);
	};

	return MercUtil;
})(MercUtil || {});
