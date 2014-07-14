var script14202 = function(self, actor, here, args, extra)
{
	//River pushing downriver to waterfall - ARENA
//Koradin - June 2011
var totalTime = 6;
//while (totalTime > 0) {
wait random(0,10);
if (self.people.length == 0)
	//continue;
	return;
var actor = self.people[random(0,self.people.length-1)];
if (!actor)
	//continue;
	return;
if (actor.room != self)
	return;
if (self.direction(constants.SOUTH)) {
	var nextRoom = self.direction(constants.SOUTH);
	var message = "The swift current sweeps you down the river!";
	var actMsg = "$n is swept downriver by the rushing current!";
}
else {
	var message = "You're pulled over the edge, your screams drowned by the rushing water.";
	var actMsg = "$n is pulled over the edge, $s last screams drowned by the rushing water.";
	var nextRoom = self.direction(constants.DOWN);
}
actor.send(message);
act(actMsg,true,actor,null,null,constants.TO_ROOM);
actor.send(" ");
actor.moveToRoom(nextRoom);
actor.comm("look");
if (actor.room.vnum == 14200) {
	actor.send("You hardly notice the SPLAT your body makes against the rocks before the torrent washes over you.");
	act("$n smashes against the rocks, the water carrying $s lifeless body out to sea.",true,actor,null,null,constants.TO_ROOM);
	actor.damage(10000);
	return;
}
totalTime -= 1;
//}

};

