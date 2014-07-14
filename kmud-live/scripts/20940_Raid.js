var script20940 = function(self, actor, here, args, extra)
{
	//RAID command for players to initiate raids.
//Koradin - March 2010
_block;
if(actor.level < 103) {
	actor.send("The RAID command has been temporarily removed for updates. Please try again later!");
	return;
}
var mag = actor.magenta(constants.CL_OFF);
var nrm = actor.normal(constants.CL_OFF);
var cyn = actor.cyan(constants.CL_OFF);
var selector = getMobAtRoom(20897,20942);
var bell = getMobAtRoom(20897,20801);
if (bell)
{
	actor.send("There is already a raid in progress!");
	return;
}
if (!selector)
{
	actor.send("The last raid ended too soon to start another one. Try again later.");
	return;
}
else if (actor.gold < 5000)
{
	actor.send("All raids currently cost "+cyn+"50"+nrm+" silver coins.");
	actor.send("...and it seems you don't have that much!");
	return;
}
else
{
	actor.send(" ");
	actor.send("*** All raids currently cost "+cyn+"50"+nrm+" silver coins. ***");
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
	setSval(selector,actor.id,"raid",0);
	actor.attach(20941);
}

};

