var script9816 = function(self, actor, here, args, extra)
{
	// Alder
// June 2011
/****************************************
 *             Store Item               *
 ****************************************/
if ( actor.race != self.race ) {
	return;
}
var item = extra.obj;
var cost = getStoragePrice(item);
if ( !isNumber(cost) ) {
	_block;
	self.tell(actor, cost);
	return;
}
if ( !actor.countEmptyStorageSlots() ) {
	_block;
	self.tell(actor, "You are currently storing the maximum number of items allowed on your account.");
	return;
}
var deficit = (actor.gold - cost);
if ( deficit < 0 ) {
	_block;
	self.tell(actor, "Come back when you have "+(-deficit)+" more copper.");
	return;
}
if ( item.contents.length ) {
	_block;
	self.tell(actor, "You must first remove the contents of "+item.name+".");
	return;
}
actor.gold -= cost;
actor.send("You hand "+cost+" copper to "+self.name+".");
act("$n hands $N some coins.",true,actor,null,self,constants.TO_ROOM);
waitpulse 1;
self.tell(actor,"I'll store this away immediately.");
actor.storeItem(item);

};

