var script43 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var timer = getSval(self,43,actor.name);
	if (!timer)
		timer = time()+21600;
	for each (var mr in global.manors) {
		if (self.room.vnum == mr.innRoomVnum)
			var manor = mr;
	}
	if (!manor)
		return;
	getCharCols(actor);
	var allowed = false;
	if (arrContains(manor.allowedUsers, actor.id))
		allowed = true;
	if (strn_cmp(vArgs[0],"offer",2)) {
		if (manor.isUnderSiege) {
			_block;
			self.comm("say Are you crazy?! The "+manor.name+" is under attack right now!");
			return;
		}
		if (allowed == false && actor.id != manor.ownerUserId) {
			_block;
			self.comm("say You are not allowed to rent a room here!");
			return;
		}
		else if (allowed == true && actor.id != manor.ownerUserId) {
			_block;
			actor.send(capFirstLetter(self.name) + " tells you, 'Rent for you is free, courtesy of "+getUserNameByUserId(manor.ownerUserId)+".'");
			return;
		}
	}
	else if (strn_cmp(vArgs[0],"rent",3)) {
		//sendKoradin("sup");
		if (manor.isUnderSiege) {
			_block;
			self.comm("say Are you crazy?! The "+manor.name+" is under attack right now!");
			return;
		}
		if (allowed == false && actor.id != manor.ownerUserId) {
			_block;
			self.comm("say You are not allowed to rent a room here!");
			return;
		}
		else if (allowed == true && actor.id != manor.ownerUserId) {
			//if (actor.gold < (actor.level*40)) {
			//	_block;
			//	self.comm("say You can't afford it!");
			//	return;
			//}
			//actor.send(capFirstLetter(self.name) + " collects "+actor.numCopperToText(actor.level*40, true)+" from you.");
			//actor.gold -= (actor.level*40);
			if (timer < time()) {
				setSval(self,43,actor.name,time()+21600);
				manor.goldOwed += (actor.level*50);
				saveManor(manor);
			}
		}
	}
	
	
	
}