var script10606 = function(self, actor, here, args, extra)
{
	if ( actor.quest('Search and Rescue') > 0 && !actor.questTaskProgress('Search and Rescue')[6].comp && self.leader != actor ) {
     waitpulse 7;
     self.comm("say Save me!");
     waitpulse 7
     self.comm("fol " + actor.name);
}

};

