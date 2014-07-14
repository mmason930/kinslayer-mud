var script1005 = function(self, actor, here, args, extra)
{
	waitpulse 1;
if(random(1,5) == 1) {
	if (actor.room == self)
{
	act("You lose your footing and fall off the ladder!!", true, actor, null, null, constants.TO_CHAR);
	act("$n loses $s footing and falls off the ladder!", true, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom(getRoom(7023));
	actor.comm("look");
	actor.send("You fall into the alley below.");
	actor.send("Ouch!");
	actor.damage(15, self);
	act("$n suddenly falls into the alley from the ladder above!", true, actor, null, null, constants.TO_ROOM);
}
}

};

