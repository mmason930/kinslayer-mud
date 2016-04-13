var script18699 = function(self, actor, here, args, extra) {
	/*Attached to object 18699 - Used to HITCH the cart to a horse that is following actor*/
	/*Rhollor December 09*/
	/*DOES NOT WORK ATM!!!!*/
	/*To create an object that works as a horse cart include "horsecartchest" in its namelist*/
	var vArgs = args.split(" ");
	var command = vArgs[ 0 ].toLowerCase();
	_block;
	
	var commandIsHitch = command == "hitch";
	var toFrom = commandIsHitch ? "to" : "from";
	
	if(actor.mount)
	{
		actor.send("You can't do that while riding!");
		return;
	}
		
	if(actor.position < constants.POS_FIGHTING)
	{
		actor.send("Nah... You're much too relaxed for that.");
		return;
	}
		
	if(actor.position == constants.POS_FIGHTING)
	{
		actor.send("You'd better stop fighting first!");
		return;
	}
		
	if(actor.position == constants.POS_FLYING)
	{
		actor.send("You'd better land on the ground first.");
		return;
	}
		
	if(vArgs.length < 3)
	{
		actor.send((commandIsHitch ? "Hitch" : "Unhitch") + " what " + toFrom + " what?");
		return;
	}
		
	var objectName = vArgs[1], horseName = vArgs[2];
	var targetObject = getObjInListVis(actor, objectName, here.items);
		
	if(!targetObject)
	{
		actor.send("You don't see any '" + objectName + "' here.");
		return;
	}
		
	var targetHorse = getCharInListVis(actor, horseName, here.people);
		
	if(!targetHorse)
	{
		actor.send("You don't see any '" + horseName + "' here to " + (commandIsHitch ? "hitch to" : "unhitch from") + "!");
		return;
	}
		
	if(!targetHorse.isHorse())
	{
		actor.send("You can only do that to a mount!");
		return;
	}
		
	if(targetObject.vnum != 18699)
	{
		actor.send("You can't " + command + " " + targetObject.name + " " + toFrom + " " + targetHorse.name + "!");
		return;
	}
		
	if(!commandIsHitch && !targetHorse.hitchedBy)
	{
		actor.send("There is nothing hitched to " + targetHorse.name + ".");
		return;
	}
		
	if(targetHorse.riddenBy)
	{
		actor.send("I don't think " + targetHorse.riddenBy.name + " will appreciate that...");
		return;
	}
		
	if(commandIsHitch && targetHorse.hitchedBy)
	{
		actor.send("There is already " + targetHorse.hitchedBy.name + " hitched to " + targetHorse.name + "!");
		return;
	}
		
	if(commandIsHitch)
	{
		act("You successfully hitch $p to $N", true, actor, targetObject, targetHorse, constants.TO_CHAR);
		act("$n hitches $p to $N", true, actor, targetObject, targetHorse, constants.TO_NOTVICT);
		act("$n hitches $p to you.", true, actor, targetObject, targetHorse, constants.TO_VICT);
	
		targetHorse.hitchedBy = targetObject;
		targetObject.hitchedTo = targetHorse;
	}
	else
	{
		act("You successfully unhitch $p from $N.", true, actor, targetObject, targetHorse, constants.TO_CHAR);
		act("$n unhitches $p from $N.", true, actor, targetObject, targetHorse, constants.TO_NOTVICT);
		act("$n unhitches $p from you.", true, actor, targetObject, targetHorse, constants.TO_VICT);
		
		targetHorse.hitchedBy = null;
		targetObject.hitchedTo = null;
	}
	
}