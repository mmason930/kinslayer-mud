var script33213 = function(self, actor, here, args, extra)
{
	//here.echo("yo");
var timer = getSval(self,33212,"gate");
var manor = getManor(self);
if ((timer - time() < 1 || !timer) && manor.gateIsBroken) { // timer's up, rebuild gate.
	manor.gateIsBroken = false;
	here.direction(self.firstStep(getRoom(manor.insideGateRoomVnum))).echo("The gate has been rebuilt by "+getUserNameByUserId(manor.ownerUserId)+"'s workers.");
	here.echo("The gate has been rebuilt by "+getUserNameByUserId(manor.ownerUserId)+"'s workers.");
	self.closeDoor(self.firstStep(getRoom(manor.insideGateRoomVnum)), true);
	self.lockDoor(self.firstStep(getRoom(manor.insideGateRoomVnum)), true);
}

};

