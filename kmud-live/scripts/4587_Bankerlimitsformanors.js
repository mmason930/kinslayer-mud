var script4587 = function(self, actor, here, args, extra)
{
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
if (!strn_cmp(vArgs[0],"balance",3) && !strn_cmp(vArgs[0],"withdraw",3) && !strn_cmp(vArgs[0],"deposit",2))
	return;
_block;
if (strn_cmp(vArgs[0],"balance",3)) {
	if (allowed == false && actor.id != manor.ownerUserId) {
		self.comm("say You are not allowed to check your balance here!");
		return;
	}
	else if (allowed == true && actor.id != manor.ownerUserId) {
		actor.send(capFirstLetter(self.name) + " tells you, 'Here's your balance, courtesy of "+getUserNameByUserId(manor.ownerUserId)+".'");
	}
	actor.send("You currently have "+actor.bankGold+" copper coins deposited."+nrm);

}
else if (strn_cmp(vArgs[0],"withdraw",3)) {
	if (!vArgs[1]) {
		actor.send("Withdraw what?");
		return;
	}
	if (allowed == false && actor.id != manor.ownerUserId) {
		self.comm("say You are not allowed to withdraw gold here!");
		return;
	}
	if (vArgs[1] < 1 || isNaN(vArgs[1])) {
		self.tell(actor,"You can't withdraw that.");
		return;
	}
	var vGold = parseInt(vArgs[1]);
	if (vGold > actor.bankGold) {
		self.tell(actor, "You don't have that much deposited.");
		return;
	}
	if (allowed == true && actor.id != manor.ownerUserId) {
		actor.send(capFirstLetter(self.name) + " tells you, 'Courtesy of "+getUserNameByUserId(manor.ownerUserId)+".'");
		if (timer < time()) {
			setSval(self,43,actor.name,time()+21600);
			manor.goldOwed += (Math.floor(vGold/100));
			saveManor(manor);
		}
	}
	actor.send("You withdraw "+vGold+" copper coins."+nrm);
	actor.bankGold -= vGold;
	actor.gold += vGold;
}
else if (strn_cmp(vArgs[0],"deposit",2)) {
	if (!vArgs[1]) {
		actor.send("Deposit what?");
		return;
	}
	if (allowed == false && actor.id != manor.ownerUserId) {
		self.comm("say You are not allowed to deposit gold here!");
		return;
	}
	if (vArgs[1] < 1 || isNaN(vArgs[1])) {
		self.tell(actor,"You can't deposit that.");
		return;
	}
	var vGold = parseInt(vArgs[1]);
	if (vGold > actor.gold) {
		self.tell(actor, "You don't have that much!");
		return;
	}
	if (allowed == true && actor.id != manor.ownerUserId) {
		actor.send(capFirstLetter(self.name) + " tells you, 'Courtesy of "+getUserNameByUserId(manor.ownerUserId)+".'");	
		if (timer < time()) {
			setSval(self,43,actor.name,time()+21600);
			manor.goldOwed += (Math.floor(vGold/100));
			saveManor(manor);
		}
	}
	actor.send("You deposit "+vGold+" copper coins."+nrm);
	actor.bankGold += vGold;
	actor.gold -= vGold;
}


};

