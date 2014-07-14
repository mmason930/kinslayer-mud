var script10952 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( !str_cmp(vArgs[1],"bell") ) {
	_block;
	if (actor.room.vnum == 1704)
	{
		var zone = "Fal Daran lookouts answer the call of the city alarm bell, sounding their long horns.";
		var global = "The long Fal Dara alarm horn echoes far in the distance.";
	}
	else if (actor.room.vnum == 100)
	{
		var zone = "Caemlyn scouts answer the call of the bell, sounding their gongs.";
		var global = "The gongs of the Caemlyn city alarm sound off in the distance.";
	}
	else if (actor.room.vnum == 22381)
	{
		var zone = "The Valon Guard answers the call of the bell, sounding the alarms.";
		var global = "The loud tolling of the Tar Valon bell can be heard in the distance.";
	}
	else if (actor.room.vnum == 10335)
	{
		var zone = "The strong color guard of Fal Moran answers the call of the bell.";
		var global = "The bold call of the Fal Moran trumpets can be heard sounding off in the distance.";
	}
	else if (actor.room.vnum == 8003)
	{
		var zone = "Trolloc wardrummers answer the call of the bell.";
		var global = "The terrifying beat of the trolloc war drums can be heard in the distance.";
	}
	else if (actor.room.vnum == 20101)
	{
		var zone = "The Murandian Cavalry address the alarm with haste.";
		var global = "The Lugard distress bell echoes throughout the landscape.";
	}
	else if (actor.room.vnum == 4530)
	{
		var zone = "The Baerlon Watchmen hear the bell and begin cranking the long alarm horn, warning the people.";
		var global = "A long whining alarm horn sounds off, warning of danger in Baerlon.";
	}
	else if (actor.room.vnum == 8102)
	{
		var zone = "Trollocs can be seen running to alert the Dreadlord of battle to come.";
		var global = "A screaming alarm echoes in the distance, summoning trollocs to the Stronghold.";
	}
	if( getSval(self,10952,"bellIsRinging") ) {
		actor.send("The bell is already in the process of ringing.");
		return;
	}
	setSval(self,10952,"bellIsRinging",true);
	actor.send("You pull hard on the rope, and the bell starts to swing back and forth high above you.");
	act("$n pulls hard on the rope, and the bell starts swinging back and forth.", true, actor, null, null, constants.TO_ROOM);
	wait 4;
	self.room.zecho("The tolling of a bell can be heard throughout the area.");
	wait 3;
	self.room.zecho(zone);
	wait 3;
	gecho(global, function(actor) { return true; } );
	wait 5;
	setSval(self,10952,"bellIsRinging",undefined);
}
else
	actor.send("Ring what?");

};

