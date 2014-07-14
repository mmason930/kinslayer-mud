var script1253 = function(self, actor, here, args, extra)
{
	waitpulse 1;
if( extra.obj.vnum == 3173 ) {
    extra.obj.extract();
	if( actor.quest('The Haunted Well') == -1 || actor.quest('The Haunted Well') >= 100 )
	    return;
	wait 3;
    self.comm("smile");
	wait 3;
	self.say("Ah. This marble. A dreadful looking thing. Fell from the well, you say?");
	wait 3;
	self.say("And the monsters. Gone as well? That is magnificant news!");
	wait 3;
	self.say("You have my deepest gratitude! I will be sure to send my thanks to Mazrim Taim.");
	wait 3;
	actor.qval('The Haunted Well', 100);
}

};

