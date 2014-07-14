var script178 = function(self, actor, here, args, extra)
{
	if( actor.vnum == -1 && random(1,100) <= 15 ) {
	wait 1;
	if(self.canSee(actor)) {
		self.say("Please spare me a few coins, " + actor.name + "!");
	}
}


};

