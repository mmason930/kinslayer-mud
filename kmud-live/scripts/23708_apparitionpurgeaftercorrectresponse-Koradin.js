var script23708 = function(self, actor, here, args, extra)
{
	//Apparition will purge, thus giving access to Maerwynn's smob rooms after correct lanfear response.
//Koradin - December 2009
if (getSval(self, 23706, "riddle_sval") != 2)
return;
else
{
setSval(self, 23706, "riddle_sval", 1);
self.comm("emote perks up when he hears the name.");
wait 2;
self.say("Yes! Lanfear is right!");
self.comm("scowl");
wait 2;
self.say("Maerwynn is more Chosen than Lanfear will ever be, and don't you forget it!");
self.comm("sigh");
wait 3;
self.say("Well, I suppose I have to hold up my end of the bargain...");
wait 3;
self.say("It won't matter much. Maerwynn will tear you to pieces!");
here.echo("The apparition gives his last smirk as he slowly fades out of existence.");
self.extract();
return;
}

};

