var script13546 = function(self, actor, here, args, extra)
{
	var startRoom = actor.room;
if( extra.direction == constants.WEST ) {
}
var rand = random(1,5);
if( rand == 1 ) {
waitpulse 6;
    actor.send("A cool wind blows in from the west.");
}
else if( rand == 2 ) {
waitpulse 6;
    actor.send("A cold breeze rustles the bushes along side the path.");
}
else if( rand == 3 ) {
waitpulse 6;
actor.send("A chilly current of air twists around here.");
}
else if( rand == 4 ) {
waitpulse 6;
actor.send("The air here feels icy.");
}
else if( rand == 5 ) {
waitpulse 6;
actor.send("A quick frosty wind gusts in from the west.");
}

};

