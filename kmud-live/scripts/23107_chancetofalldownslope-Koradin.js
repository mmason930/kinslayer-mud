var script23107 = function(self, actor, here, args, extra)
{
	waitpulse 1;
if (actor.room == self)
{
	act("You lose your footing and tumble down the slope!", true, actor, null, null, constants.TO_CHAR);
	act("$n loses $s footing and tumbles down the slope!", true, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom(getRoom(23157));
	actor.comm("look");
	actor.send("Your fall is broken by the stone floor of the cave.");
	actor.send("Ouch!");
	actor.damage(10, self);
	act("$n suddenly tumbles into the cave from the bushes on the slope above!", true, actor, null, null, constants.TO_ROOM);
}

};

