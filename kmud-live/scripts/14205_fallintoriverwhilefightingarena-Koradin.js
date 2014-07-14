var script14205 = function(self, actor, here, args, extra)
{
	wait random(1,6);
if (self.people.length == 0 || random(1,2) == 1)
	return;
var actor = self.people[random(0,self.people.length-1)];
if (!actor.fighting)
	return;
var victim = actor.fighting;
actor.send("As you avoid a blow from "+victim.name+", you slip and fall into the river!");
act("As $n avoids a blow from $N, $e slips and falls into the river!",true,actor,null,victim,constants.TO_NOTVICT);
act("As $n avoids your attack, $e slips and falls into the river!",true,actor,null,victim,constants.TO_VICT);
if (self.vnum == 14264 || self.vnum == 14238 )
	var nextRoom = 14247;
else if (self.vnum == 14263 || self.vnum == 14237 )
	var nextRoom = 14249;
else if (self.vnum == 14262 || self.vnum == 14236 )
	var nextRoom = 14250;
else if (self.vnum == 14261 || self.vnum == 14235 )
	var nextRoom = 14251;
for each (var thing in actor.inventory) {
	if (arrContains(global.arenaItems, thing.vnum))
		actor.comm("drop "+thing.namelist.split(" ")[0]);
}
actor.moveToRoom(getRoom(nextRoom));
actor.comm("look");
actor.send("You plunge into the rushing waters.");
act("$n falls into the river from the rocky cliffs above with a SPLASH!",true,actor,null,null,constants.TO_ROOM);

};

