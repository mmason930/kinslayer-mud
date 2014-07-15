var script23709 = function(self, actor, here, args, extra) {
	_block;
	var newroom = getRoom(23711);
	echoaround(actor, actor.name + " steps on a board which immediately snaps under " + actor.his_her() + " weight!");
	echoaround(actor, capFirstLetter(actor.he_she()) + " falls through the floor and out of sight!");
	actor.send("You step on one of the loose boards while trying to leave, and it snaps under your weight!");
	actor.send("You fall through the floor!");
	newroom.echo("Suddenly the rotted ceiling EXPLODES above you!");
	newroom.echo(capFirstLetter(actor.name) + " falls in from the floor above and goes sprawling.");
	actor.moveToRoom(newroom);
	actor.comm("look");
	actor.send(actor.red(constants.CL_SPARSE) + "You hit the stone floor and everything goes black." + actor.normal(constants.CL_SPARSE));
	actor.position = constants.POS_STUNNED;
	wait 5;
	if (actor.position == constants.POS_STUNNED)
	{
		actor.position = constants.POS_SITTING;
		actor.send("You come to and sit up, rubbing your head.");
		echoaround(actor, actor.name + " comes to and sits up, rubbing " + actor.his_her() + " head.");
	}
}