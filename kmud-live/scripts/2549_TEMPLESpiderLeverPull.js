var script2549 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);

_block;

if(vArgs.length == 1)
{
	actor.send("Pull what?");
	return;
}

var targetObject = getObjInListVis(actor, vArgs[1], here.items);

if(targetObject == null)
{
	actor.send("You don't see that here.");
	return;
}

if(targetObject != self)
{
	actor.send("You can't pull that!");
	return;
}

act("You pull $P.", false, actor, null, targetObject, constants.TO_CHAR);
act("$n pull $P.", false, actor, null, targetObject, constants.TO_ROOM);

setTimeout(3, function() {
	
	here.echo("You hear a loud thud from the east.");
	var room1 = getRoom(2543);
	var room2 = getRoom(2546);
	
	if(self.spiderSlabOpen === undefined || self.spiderSlabOpen === false)
	{
		room1.unlockDoor(constants.EAST);
		room1.openDoor(constants.EAST);
		
		room2.unlockDoor(constants.WEST);
		room2.openDoor(constants.WEST);
		
		room1.echo("The " + room1.doorName(constants.EAST) + " opens to the east.");
		room2.echo("The " + room2.doorName(constants.WEST) + " opens to the west.");
		
		self.spiderSlabOpen = true;
	}
	else
	{
		room1.closeDoor(constants.EAST);
		room1.lockDoor(constants.EAST);
		
		room2.closeDoor(constants.WEST);
		room2.lockDoor(constants.WEST);
		
		room1.echo("The " + room1.doorName(constants.EAST) + " closes to the east.");
		room2.echo("The " + room2.doorName(constants.WEST) + " closes to the west.");
		
		self.spiderSlabOpen = false;
	}
});

};

