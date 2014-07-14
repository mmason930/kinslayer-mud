var script89 = function(self, actor, here, args, extra)
{
	wait 1;
if( actor.race != self.race || !self.canSee(actor) )
    return;
self.say("Hello, " + actor.name + ".");
wait 1;
self.comm("Welcome to my store!");

};

