var script502 = function(self, actor, here, args, extra)
{
	_block;
getCharCols(actor);
if (actor.quest("newbieTutorial") == 2) {
	actor.gsend("You have already taken the tutorial, "+actor.name+".");
	return;
}
else if (actor.level > 4) {
	actor.gsend("You can only take the tutorial before attaining level five.");
	return;
}
else if (actor.race != constants.RACE_HUMAN) {
	actor.gsend("Only humans can take the tutorial, "+actor.name+".");
	return;
}
else if (actor.affectedBy(constants.AFF_NOQUIT)) {
	actor.gsend("You cannot enter the tutorial with NOQUIT, "+actor.name+". Type STAT to view your effects.");
	return;
}
actor.gsend("Greetings "+actor.name+". Would you like to take the tutorial?");
actor.send(" ");
actor.send("["+cyn+"Y"+nrm+"]es       ["+cyn+"N"+nrm+"]o");
actor.send(" ");
setSval(actor,501,"tutorial",1);
actor.attach(501);



};

