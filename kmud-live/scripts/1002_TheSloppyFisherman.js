var script1002 = function(self, actor, here, args, extra)
{
	if (actor.race == constants.RACE_HUMAN) {
	waitpulse 5;
	if (actor.room.vnum == 21474)
		actor.send("The Innkeeper tells you, 'Ello, if you want some ale or food you'll have to buy it in the basement and bring it up here.'");
	waitpulse 3;
	if (actor.room.vnum == 21474)
		actor.send("The Innkeeper tells you, 'All the rooms are filled up, but there's pallets in the basement.'");
}	
if (actor.race == constants.RACE_TROLLOC) {
	waitpulse 3;
	if (actor.room.vnum == 21474)
		actor.send("The Innkeeper yells, 'Blood and ashes!  It's the Last Battle!'");
	waitpulse 4;		
	if (actor.room.vnum == 21474)
		actor.send("The patrons all turn towards the door. When they notice you, food falls from forks and open mouths and drinks are spilled.");
}

};

