var script33203 = function(self, actor, here, args, extra)
{
	wait random(0,10);
var room = getRoom(getSval(self,33210,"start"));
var manor = getManor(self.room);
if (!manor)
	return;
//here.echo(self.room.vnum);
if (room) {
	if (self.room != room && room.vnum != manor.outsideGateRoomVnum) {
		self.comm("say I must return to my post!");
		act("$n runs away!",true,self,null,null,constants.TO_ROOM);
		self.moveToRoom(room);
		act("$n arrives to guard $s post.",true,self,null,null,constants.TO_ROOM);
	}
}
if (self.fighting) {
	if (arrContains(manor.allowedUsers, self.fighting.id)) {
		getCharCols(self.fighting);
		self.fighting.send(bld+capFirstLetter(getUserNameByUserId(manor.ownerUserId))+" has revoked your permission into the "+manor.name+"!"+nrm);
		updateAllowed(manor, self.fighting.id);
	}
}
if (!manor)
	return;
var victim = getManorTarget(self);
if (!victim) {
	//sendKoradin("no victim for "+self.name);
	return;
}
var owner = getManorOwnerById(manor.ownerUserId);
if (owner) {
	if (owner.room == self.room && owner.fighting) {
		if (!self.fighting)
			self.comm("assist "+owner.name);
	}
}
if (getSval(victim,33200,"warning") > time()) {
	//sendKoradin("stopping for sval");
	return;
}
//sendKoradin(self.name+" attacking "+victim.name);
if (random(1,10) == 1) {
	var shoutArr = ["For "+getUserNameByUserId(manor.ownerUserId)+"!","Aargh!","Die filth!","The "+manor.name+" is under attack!"];
	self.comm("yell "+shoutArr[random(0,shoutArr.length-1)]);
}
if (victim.room == self.room && !isName("manorArcher", self.namelist)) {//archers dont engage
	//sendKoradin(victim.name);
	if (!self.fighting) {
		if (victim.vnum > 0)
			var vName = victim.namelist.split(" ")[0];
		else
			var vName = victim.name;
		self.comm("kill "+vName);
	}
	return;
}
if (self.isBashed || (self.position != constants.POS_STANDING && self.position != constants.POS_FIGHTING))
	return;
var dmg = 0;
var waitTime = 0;
waitTime += self.room.distanceTo(victim.room);
var firstRoom = victim.room;
if (victim.room.vnum == manor.outside && random(1,3) > 1)
	return;
var dmgMsg1 = "grazes";
var dmgMsg2 = "";
var msg = "";
var selfMsg = "";
var obj = "an arrow from above";
var noTargMsg = "";
if (isName("manorArcher", self.namelist)) {
	dmg = random(5,20);
	dmg = Math.floor((dmg*(100-victim.absorb))/100);
	selfMsg = self.name+" fires an arrow onto the "+manor.name+" grounds below!";
	msg = "An archer fires an arrow from atop the "+manor.name+" tower!";
	if (dmg < 10) {
		dmgMsg1 = " grazes ";
		dmgMsg2 = " as it flies by.";
	}
	else if (dmg < 20) {
		dmgMsg1 = " finds its target, hitting ";
		dmgMsg2 = " in the chest!";
	}
	else {
		dmgMsg1 = " hits ";
		dmgMsg2 = ", spraying blood everywhere!";
	}
	noTargMsg = " hits the ground harmlessly.";
}
else if (isName("manorChanneler", self.namelist)) {
	dmg = random(15,30);
	dmg = Math.floor((dmg*(100-victim.absorb))/100);
	if (random(1,2) == 1) {
		msg = "Flames streak down from above...";
		selfMsg = self.name+" hurls a fireball toward an unseen victim below!";
		obj = "a fireball";
		if (dmg < 10) {
			dmgMsg1 = " streaks by, burning ";
			dmgMsg2 = "!";
		}
		else if (dmg < 20) {
			dmgMsg1 = " hits ";
			dmgMsg2 = " in the chest!";
		}
		else {
			dmgMsg1 = " explodes in ";
			dmgMsg2 = " face!";
		}
		noTargMsg = " streaks by, missing its target.";
	}
	else {
		msg = "Storm clouds crackle overhead...";
		selfMsg = self.name+" calls lightning from the sky onto the grounds below!";
		obj = "a bolt of lightning";
		dmgMsg1 = " strikes ";
		dmgMsg2 = "!";
		noTargMsg = " strikes the ground, its target no longer around.";
	}
}
for each (var person in self.room.people) {
	act(selfMsg, true, person, null, null, constants.TO_CHAR);
}
for each (var pers in victim.room.people) {
	act(msg, true, pers, null, null, constants.TO_CHAR);
}
wait waitTime;
if (!victim || victim.room != firstRoom) { // target moved/died, not there anymore
	for each (var endP in firstRoom.people)
		act(capFirstLetter(obj)+noTargMsg, true, endP, null, null, constants.TO_CHAR);
	return;
}
if (victim.dodge > random(1,200)) {
	victim.send("You dive out of the way, narrowly avoiding "+obj+"!");
	for each (var dude in victim.room.people) {
		if (dude != victim)
			act(victim.name+" dives out of the way, narrowly avoiding "+obj+"!",true,dude,null,null,constants.TO_CHAR)
	}
	return;
}
getCharCols(victim);
if (dmg > 19 && obj == "a fireball")
	var youR = "your";
else
	var youR = "you";
victim.send(red+capFirstLetter(obj)+dmgMsg1+youR+dmgMsg2+nrm);
for each (var dude in victim.room.people) {
	if (obj == "a fireball" && dmgMsg1 == " explodes in ")
		var maybeR = "'s";
	else
		var maybeR = "";
	if (dude != victim)
		act(capFirstLetter(obj)+dmgMsg1+victim.name+maybeR+dmgMsg2,true,dude,null,null,constants.TO_CHAR)
}
victim.damage(dmg);








};

