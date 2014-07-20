var script33204 = function(self, actor, here, args, extra) {
	_block;
		getCharCols(actor);
		for (var _autoKey in global.manors) {
			var mr = global.manors[_autoKey];
			if (mr.ownerUserId == actor.id) {
				var manor = mr;
				//sendKoradin("worked");
			}
		}
		if (!manor) {
			actor.send("You don't own a property!");
			return;
		}
		var manorTime = manorTimeToText(timeTillManorExpires(manor));
		actor.send(bld+"You currently own the "+manor.name+" near "+manor.area+"."+nrm);
		actor.send(bld+"\nUpkeep has been paid for "+manorTime+"."+nrm);
		actor.send(bld+"Current Upkeep: "+nrm+actor.numCopperToText(getUpkeep(manor), true)+bld+" per day."+nrm);
		actor.send(bld+"\nYou've allowed the following players into the "+manor.name+":"+nrm);
		var users = [];
		for (var _autoKey in manor.allowedUsers) {
			var id = manor.allowedUsers[_autoKey];
			users.push(getUserNameByUserId(id));
		}
		if (users.length == 0)
			actor.send(nrm+"None"+nrm);
		else
			actor.send(bld+users.join(", ")+nrm);
	
}