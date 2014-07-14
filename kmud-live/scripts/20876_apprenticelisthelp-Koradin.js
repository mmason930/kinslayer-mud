var script20876 = function(self, actor, here, args, extra)
{
	if (self.race != actor.race)
{
self.say("I don't trade with your kind!");
return;
}
if (arrContains(self.namelist.split(" "), "blacksmith")) {
	var tradeType = "Smithing";
	var trader = "blacksmith";
	var place = "forge";
}
else if (arrContains(self.namelist.split(" "), "tailor")) {
	var tradeType = "Tailoring";
	var trader = "tailor";
	var place = "workshop";
}
var skill = actor.getSkill(getSkillVnum(tradeType));
// if (skill < 10) {
	// self.say("You have no need to rent the forge if you can't smith, "+actor.name+"!");
	// actor.send(capFirstLetter(self.name) + " hands you your coins back.");
	// act("$n hands $N some coins.", true, self, null, actor, constants.TO_NOTVICT);
	// actor.gold += payment;
	// return;
// }
if (skill > 98) {
	var reqPayment = 240;
	var rank = "artisan "+trader+"s";
}
else if (skill > 74) {
	var reqPayment = 180;
	var rank = "master "+trader+"s";
}
else if (skill > 39) {
	var reqPayment = 120;
	var rank = "journeyman "+trader+"s";
}
else if (skill > 9) {
	var reqPayment = 60;
	var rank = "apprentice "+trader+"s";
}
else {
	var reqPayment = 30;
	var rank = "commoners";
}
wait 1;
self.say("For "+rank +", the cost to rent the "+place+" is"+actor.numCopperToText(reqPayment)+" per hour.")
self.say("You can pay for as much time as you want, with a minimum of one hour!");
wait 1;
self.say("Just hand me the coin and I'll see if the "+place+" is available.");
wait 2;
getCharCols(actor);
self.say("My master will buy any crafted item you don't have need for!  Just hand it over.");
wait 2;
getCharCols(actor);
self.say("You can also see what recipes are available to you by trying to "+cyn+"buy"+nrm+" a "+cyn+"recipe"+nrm+".");
wait 2;
getCharCols(actor);
self.say("Or, to see what the "+trader+" has to sell at the moment, check my "+cyn+"list"+nrm+".");
return;


};

