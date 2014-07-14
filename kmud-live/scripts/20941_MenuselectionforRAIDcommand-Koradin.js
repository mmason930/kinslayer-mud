var script20941 = function(self, actor, here, args, extra)
{
	// The actor must choose a selection, blocks all other commands.
// Koradin - March 2010
var cmd = getArgList(args)[0];
var selector = getMobAtRoom(20897,20942);
function canSend(actor)
{
	if (actor.level < 106)
		return true;
	return false;
}
if (strn_cmp(cmd,"alias",1) || strn_cmp(cmd,"x",1))
{
	actor.detach(20941);
	setSval(selector,actor.id,"raid",0);
	actor.send("You have exited the Raid Menu.");
	if (strn_cmp(cmd,"x",1))
		_block;
	else
		_noblock;
	return;
}
_block;
getCharCols(actor,constants.CL_OFF);
if (getSval(selector,actor.id,"raid") == 0)
{
	if (cmd == 1)
	{
		actor.send(" ");
		actor.send("Next, select the desired Champion of the "+mag+"LIGHT"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") Lord Marshal Uno");
		actor.send(mag+"2"+nrm+") Kari Aedoneas");
		actor.send(mag+"3"+nrm+") Random Champion of the Light");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-3"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		setSval(selector,actor.id,"raid",1)
		return;
	}
	else if (cmd == 2)
	{
		actor.send(" ");
		actor.send("Next, select the desired Champion of the "+mag+"DARK"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") The Myrddraal Syyggar");
		actor.send(mag+"2"+nrm+") Mirage the Dreadlord");
		actor.send(mag+"3"+nrm+") Random Champion of the Dark");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-3"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		setSval(selector,actor.id,"raid",2)
		return;
	}
	else if (cmd == 3)
	{
		actor.send(" ");
		actor.send("Next, select the desired Champion of the "+mag+"SEANCHAN"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") The High Lord Turak");
		actor.send(mag+"2"+nrm+") Random Champion of the Seanchan");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-2"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		setSval(selector,actor.id,"raid",3)
		return;
	}
	else if (cmd == 4)
	{
		actor.send(" ");
		actor.send("Next, select the desired Champion of the "+mag+"AIEL"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") Cuarn the Blackrock Chieftain");
		actor.send(mag+"2"+nrm+") Random Champion of the Aiel");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-2"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		setSval(selector,actor.id,"raid",4)
		return;
	}
	else if (cmd == 5)
	{
		actor.send(" ");
		// actor.send("Next, select the desired "+mag+"ANIMAL"+nrm+" raid.");
		// actor.send(" ");
		// actor.send(mag+"1"+nrm+") Lord Marshal Uno");
		// actor.send(mag+"2"+nrm+") Kari Aedoneas");
		// actor.send(mag+"3"+nrm+") Random Champion of the Light");
		// actor.send(" ");
		// actor.send("Make a Selection ("+mag+"1-3"+nrm+")");
		// actor.send(" ");
		// setSval(selector,actor.id,"raid",5)
		actor.send("There are currently no "+mag+"ANIMAL"+nrm+" raids to choose from.");
		actor.send(" ");
		actor.send("To purchase a raid, first select the desired "+mag+"RACE"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") Light");
		actor.send(mag+"2"+nrm+") Dark");
		actor.send(mag+"3"+nrm+") Seanchan");
		actor.send(mag+"4"+nrm+") Aiel");
		actor.send(mag+"5"+nrm+") Animal");
		actor.send(mag+"6"+nrm+") Random Raid");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-6"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		return;
	}
	else if (cmd == 6)
	{
		actor.send(" ");
		actor.send("Randomization is still under construction. Please try again later.");
		actor.send(" ");
		actor.send("To purchase a raid, first select the desired "+mag+"RACE"+nrm+".");
		actor.send(" ");
		actor.send(mag+"1"+nrm+") Light");
		actor.send(mag+"2"+nrm+") Dark");
		actor.send(mag+"3"+nrm+") Seanchan");
		actor.send(mag+"4"+nrm+") Aiel");
		actor.send(mag+"5"+nrm+") Animal");
		actor.send(mag+"6"+nrm+") Random Raid");
		actor.send(" ");
		actor.send(mag+"X"+nrm+") Cancel and Exit Menu");
		actor.send("_______________________");
		actor.send(" ");
		actor.send("Make a Selection ("+mag+"1-6"+nrm+"), or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		return;
	}
	else if (cmd == 7)
	{
		actor.send("You have exited the Raid Menu.");
		actor.detach(20941);
		setSval(selector,actor.id,"raid",0);
		return;
	}
	else
	{
		actor.send("Make a selection from "+mag+"1"+nrm+" to "+mag+"7"+nrm+", or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		actor.send("NOTE: Make sure you do not have an alias set for the number you're selecting.");
		actor.send("      To remove your alias, type 'alias #'.");
		return;
	}
}
else if (getSval(selector,actor.id,"raid") == 1)
{
	if (cmd == 1)
	{
		var name = "Lord Marshal Uno";
		var vnum = 1711;
		var location = [1024,19301,1090,5658];
	}
	else if (cmd == 2)
	{
		var name = "Kari Aedoneas";
		var vnum = 20892;
		var location = [1024,19301,1090,5658];
	}
	else if (cmd == 3)
	{
		var name = "a random Champion of the Light";
		var location = [1024,19301,1090,5658];
		if (random(1,2) == 1)
			var vnum = 1711;
		else
			var vnum = 20892;
	}
	else
	{
		actor.send("Make a selection from "+mag+"1"+nrm+" to "+mag+"3"+nrm+", or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		actor.send("NOTE: Make sure you do not have an alias set for the number you're selecting.");
		actor.send("      To remove your alias, type 'alias #'.");
		return;
	}
	actor.send(" ");
	actor.send("You have chosen " + name + ".");
	actor.gold -= 7500;
	gecho(mag+"*** A NEW RAID HAS BEGUN, STARTED BY " + actor.name.toUpperCase() + "! ***" +nrm,canSend);
	getRoom(location[random(0,location.length - 1)]).loadMob(vnum);
	selector.extract();
	actor.detach(20941);
	getRoom(20897).loadMob(20801);
	setSval(getMobAtRoom(20897,20801),20801,"raid",vnum);
	return;
}
else if (getSval(selector,actor.id,"raid") == 2)
{
	if (cmd == 1)
	{
		var name = "The Myrddraal Syyggar";
		var vnum = 5609;
		var location = [10247,412,20675,1239,6463];
	}
	else if (cmd == 2)
	{
		var name = "Mirage the Dreadlord";
		var vnum = 20890;
		var location = [10247,412,20675,1239,6463];
	}
	else if (cmd == 3)
	{
		var name = "a random Champion of the Dark";
		var location = [10247,412,20675,1239,6463];
		if (random(1,2) == 1)
			var vnum = 5609;
		else
			var vnum = 20890;
	}
	else
	{
		actor.send("Make a selection from "+mag+"1"+nrm+" to "+mag+"3"+nrm+", or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		actor.send("NOTE: Make sure you do not have an alias set for the number you're selecting.");
		actor.send("      To remove your alias, type 'alias #'.");
		return;
	}
	actor.send(" ");
	actor.send("You have chosen " + name + ".");
	actor.gold -= 7500;
	gecho(mag+"*** A NEW RAID HAS BEGUN, STARTED BY " + actor.name.toUpperCase() + "! ***" +nrm,canSend);
	getRoom(location[random(0,location.length - 1)]).loadMob(vnum);
	selector.extract();
	actor.detach(20941);
	getRoom(20897).loadMob(20801);
	setSval(getMobAtRoom(20897,20801),20801,"raid",vnum);
	return;
}
else if (getSval(selector,actor.id,"raid") == 3)
{
	if (cmd == 1)
	{
		var name = "The High Lord Turak";
		var vnum = 18800;
		var location = [10247,412,20675,1239,6463,1024,19301,1090,5658,21473];
	}
	else if (cmd == 2)
	{
		var name = "a random Champion of the Seanchan";
		var location = [10247,412,20675,1239,6463,1024,19301,1090,5658,21473];
		if (random(1,2) == 1)
			var vnum = 18800;
		else
			var vnum = 18800;
	}
	else
	{
		actor.send("Make a selection from "+mag+"1"+nrm+" to "+mag+"2"+nrm+", or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		actor.send("NOTE: Make sure you do not have an alias set for the number you're selecting.");
		actor.send("      To remove your alias, type 'alias #'.");
		return;
	}
	actor.send(" ");
	actor.send("You have chosen " + name + ".");
	actor.gold -= 7500;
	gecho(mag+"*** A NEW RAID HAS BEGUN, STARTED BY " + actor.name.toUpperCase() + "! ***" +nrm,canSend);
	getRoom(location[random(0,location.length - 1)]).loadMob(vnum);
	selector.extract();
	actor.detach(20941);
	getRoom(20897).loadMob(20801);
	setSval(getMobAtRoom(20897,20801),20801,"raid",vnum);
	return;
}
else if (getSval(selector,actor.id,"raid") == 4)
{
	if (cmd == 1)
	{
		var name = "Cuarn the Blackrock Chieftain";
		var vnum = 20637;
		var location = [10247,412,20675,1239,6463,1024,19301,1090,5658,21473];
	}
	else if (cmd == 2)
	{
		var name = "a random Champion of the Aiel";
		var location = [10247,412,20675,1239,6463,1024,19301,1090,5658,21473];
		if (random(1,2) == 1)
			var vnum = 20637;
		else
			var vnum = 20637;
	}
	else
	{
		actor.send("Make a selection from "+mag+"1"+nrm+" to "+mag+"2"+nrm+", or select "+mag+"X"+nrm+" to exit.");
		actor.send(" ");
		actor.send("NOTE: Make sure you do not have an alias set for the number you're selecting.");
		actor.send("      To remove your alias, type 'alias #'.");
		return;
	}
	actor.send(" ");
	actor.send("You have chosen " + name + ".");
	actor.gold -= 7500;
	gecho(mag+"*** A NEW RAID HAS BEGUN, STARTED BY " + actor.name.toUpperCase() + "! ***" +nrm,canSend);
	var randomize = location[random(0,location.length - 1)];
	getRoom(randomize).loadMob(vnum);
	selector.extract();
	actor.detach(20941);
	getRoom(20897).loadMob(20801);
	setSval(getMobAtRoom(20897,20801),20801,"raid",getMobAtRoom(randomize,vnum).id);
	return;
}
else
{
	actor.room.echo("An error has occured. Please report this to an immortal staff member.");
	actor.send(getSval(selector,actor.id,"raid"));
	actor.detach(20941);
	return;
}

};

