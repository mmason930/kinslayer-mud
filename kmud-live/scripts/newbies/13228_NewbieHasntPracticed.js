var script13228 = function(self, actor, here, args, extra) {

	here.echo("Test");
	if(actor.level == 1 && actor.getSkillPractices() >= 6)
	{
		here.echo("Dir: " + extra.dir);
		getCharCols(actor);
		actor.send(bld + red + "You are leaving the main area of the village without fully training your skills." + nrm);
//		actor.send(bld + red + "Return ")
	}
};//