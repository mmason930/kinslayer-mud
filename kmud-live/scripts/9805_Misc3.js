var script9805 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length < 3 ) {
	return;
}
var can = getObjInListVis( actor, vArgs[1], [actor.eqList, actor.inventory] );
if( !can || can.vnum != 9803 ) return;
var saw = getObjInListVis( actor, vArgs[2], [actor.eqList, actor.inventory, here.items] );
if( !saw || saw.name != "a rusty chain saw" ) return;
_block;//Both chain saw and can of gas have been found.
var amtInCan = getSval(can, 9804, "gasoline");
var amtInSaw = getSval(saw, 9804, "gasoline");
//here.echo( amtInCan );
if( amtInCan == undefined ) amtInCan = 75;
if( amtInSaw == undefined ) amtInSaw = 100;
if( amtInCan == 0 ) {
	actor.send( capFirstLetter( can.name ) + " is empty." );
	return;
}
if( getSval(saw, 9804, "onoff") == "on" ) {
	actor.send( capFirstLetter( saw.name ) + " is turned off! It must be off in order to pour gas into it." );
	return;
}
if( amtInSaw == 100 ) {
	actor.send( capFirstLetter( saw.name ) + " is already full." );
	return;
}
var needed = 100 - amtInSaw;
var amtToPour = Math.min(needed, amtInCan);
amtInSaw += amtToPour;
amtInCan -= amtToPour;
setSval(can, 9804, "gasoline", amtInCan);
setSval(saw, 9804, "gasoline", amtInSaw);
actor.send("You pour gasoline from " + can.name + " into " + saw.name + ".");
act("$n pours gasoline from " + can.name + " into " + saw.name + ".", true, actor, null, null, constants.TO_ROOM);


};

