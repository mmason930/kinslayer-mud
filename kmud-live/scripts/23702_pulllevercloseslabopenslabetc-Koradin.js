var script23702 = function(self, actor, here, args, extra)
{
	//Lever to open slab doorway in Maerwynn Smob
//Koradin - December 2009
_block;
var arg_array = getArgList(args);
if (actor.position != constants.POS_STANDING)
{
actor.send("You're not in the right position for that!");
return;
}
if (arg_array.length != 2)
{
actor.send("Pull what?");
return;
}
if (arg_array[1] != "lever")
{
actor.send("Try pulling something else.");
return;
}
var maerwynn = actor.getCharVis("maersmob");
if (self.vnum == 23719)
{
	if (maerwynn)
	{
	actor.send("You attempt to pull the lever, but it won't budge!");
	return;
	}
}
var roomwest = getRoom(23714);
var roomeast = getRoom(23716);
if (!roomwest.doorIsClosed(constants.EAST))
{
var updown = "up";
var state = "open";
}
else
{
var updown = "down";
var state = "closed";
}
actor.send("You start pulling the lever " + updown + "...");
echoaround(actor, actor.name + " starts pulling the lever " + updown + "...");
actor.startTimer(3);
var success = runTimer(actor);
if (success)
{
here.echo("*CLICK*");
here.echo("The lever comes to rest in the '" + updown + "' position.");
	if (state == "open")
	{
	roomwest.closeDoor(constants.EAST);
	roomwest.echo("Suddenly a huge stone slab rolls into place, blocking the way east!");
	roomeast.closeDoor(constants.WEST);
	roomeast.echo("Suddenly a huge stone slab rolls into place, blocking the way west!");
	}
	else
	{
	roomwest.openDoor(constants.EAST);
	roomwest.echo("The stone slab starts moving all on its own, clearing the way east.");
	roomeast.openDoor(constants.WEST);
	roomeast.echo("The stone slab starts moving all on its own, clearing the way west.");
	}
}
else
{
echoaround(actor, actor.name + " gives up and lets the lever snap back into place.")
actor.send("You give up on the lever and let it snap back into place.")
return;
}

};

