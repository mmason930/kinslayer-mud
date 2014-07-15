var script20863 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	if (!vArgs[1] || vArgs[2]) {
		actor.send("To light the forge, type 'light forge'.");
		return;
	}
	for each (var per in self.people)
	{
		var perName = per.namelist.split(" ");
		if (per.maxHit > 30000 && perName[perName.length-1] == "blacksmith")
			var blacksmith = per;
	}
	if (!blacksmith) {
		actor.send("You don't want to attempt this without the blacksmith present!");
		return;
	}
	for each (var item in self.items)
	{
		//here.echo(item.name);
		for each (var iName in item.namelist.split(" ")) {
			if (iName == "smithingforge") {
				var forge = item;
			}
		}
	}
	if (!forge) {
		actor.send("The forge seems to have gone missing... contact an immortal!");
		return;
	}
	var coal = [];
	if (strn_cmp(vArgs[0],"light",3) && !str_cmp(vArgs[1],"forge")) {
		if (getSval(forge,20863,"isLit") > 0)
		{
			blacksmith.comm("arch");
			blacksmith.say("Can't you see the forge is already lit?");
			return;
		}
		for each (var item in actor.inventory) {
			if (item.vnum == 4571)
				coal.push(item);
		}
		if (coal.length == 0) {
			blacksmith.say("You'll need some refined coal to keep the forge going!");
			wait 1;
			blacksmith.comm("stroke");
			getCharCols(actor);
			blacksmith.say("I think I might have some here I could sell you.");
			actor.send(" ");
			blacksmith.say("Check my "+cyn+"list"+nrm+" to see what's in stock.");
			return;
		}	
		var timeLit = Math.floor(coal.length * random(40,75));
	var hours = timeLit / 60;
	hours = hours.toString();
	hours = hours.split(".");
	if (hours[1]) {
		var partial = parseInt(hours[1].charAt(0));
	}
	else {
		var timeMessage = hours[0] + " hour";
		if (hours[0] != "1")
			timeMessage += "s";
	}
	hours = parseInt(hours[0]);
	if (partial < 2) {
		var timeMessage = "just over " + hours + " hour";
		if (hours > 1)
			timeMessage += "s";
	}
	else if (partial > 1 && partial < 5) {
		var timeMessage = "a little over " + hours + " hour";
		if (hours > 1)
			timeMessage += "s";
	}
	else if (partial == 5)
		var timeMessage = hours + " and a half hours";
	else if (partial == 6)
		var timeMessage = "just over " + hours + " and a half hours";
	else if (partial > 6 && partial < 9) {
		var timeMessage = "almost " + (hours+1) + " hour";	
		if (hours > 1)
			timeMessage += "s";
	}
	else if (partial == 9) {
		var timeMessage = "just shy of " + (hours+1) + " hour";
		if (hours > 1)
			timeMessage += "s";
	}
		
		
		actor.send("You carefully arrange the coal and set to work lighting the forge.");
		actor.startTimer(5);
		var success = runTimer(actor);
		if (!success) {
			actor.send("\nYou give up the attempt, and take your coal from the forge.");
			return;
		}
		setSval(forge,20863,"isLit",time()+timeLit);
		self.echo("\nThe forge roars to life.");
		for each (var cItem in coal)
			cItem.extract();
		wait 1;
		self.echo(capFirstLetter(blacksmith.name) + " looks up from his work, smiling.");
		blacksmith.say("That'll keep it going for "+timeMessage+", I'd say.");
		//here.echo(time());
		//here.echo(timeLit);
	}
	else
	{
		actor.send("To light the forge, type 'light forge'.");
		return;
	}
}