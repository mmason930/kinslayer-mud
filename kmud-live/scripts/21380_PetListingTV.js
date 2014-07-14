var script21380 = function(self, actor, here, args, extra)
{
	if( actor.race != self.race )
	return;
wait 1;
self.say("I'm glad you asked!  Here is a list of the currently available pets to buy.");
wait 1;
actor.send("You can buy a parrot for 4 silver coins.");
actor.send("You can buy a frisky little dog for 3 silver coins.");
actor.send("You can buy a tabby cat for 6 silver coins.");

};

