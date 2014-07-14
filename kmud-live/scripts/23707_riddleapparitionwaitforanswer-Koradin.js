var script23707 = function(self, actor, here, args, extra)
{
	//After saying his riddle, the apparition will wait for a response.
//Koradin - December 2009
if (getSval(self, 23706, "riddle_sval") != 2)
return;
wait 2;
var rand = random(1,3)
if (rand == 1)
here.echo(capFirstLetter(self.name) + " fiddles with the scroll, patiently waiting for the answer to his riddle.");
else if (rand == 2)
self.say("I'm here for eternity, so any time you're ready...");
else if (rand == 3)
here.echo(capFirstLetter(self.name) + " yawns, waiting for the correct response.");

};

