var script14201 = function(self, actor, here, args, extra)
{
	//Falling from the steep ledges in arena
//Koradin - June 2011
wait random(0,6);
if (self.vnum == 14260)
	var destination = random(14285,14286);
else if (self.vnum == 14239)
	var destination = random(14211,14210);
var peopleArr = [];
for each (var person in self.people) {
	if (random(1,3) < 3)
		peopleArr.push(person);
}
for each (var victim in peopleArr) {
	victim.send("You lose your footing and go plummeting into the forest!");
	victim.send(" ");
	act("$n loses $s footing and goes plummeting into the forest below!",true,victim,null,null,constants.TO_ROOM);
	victim.moveToRoom(getRoom(destination));
	victim.comm("look");
	getCharCols(victim);
	victim.send(red+"You bounce off a few branches and land in a crumpled heap."+nrm);
	victim.damage(random(20,30));
	victim.position = constants.POSITION_SITTING;
	act("$n falls out of a tree, landing in a crumpled heap.",true,victim,null,null,constants.TO_ROOM);
}

};

