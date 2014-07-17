var script20952 = function(self, actor, here, args, extra) {
	//Herbalism notice script to be attached to rooms
	//Koradin - Converted from DG May 2010
	//Rewritten by Galnor 08/19/2010
	waitpulse 1;
	if (actor.room != self)
	return;
	var skill = actor.getSkill(getSkillVnum("Mining"));
	if (random(1,125) < skill )
	{
		getCharCols(actor);
		actor.send(mag+"A glint of ore in the rocks catches your eye..."+nrm);
		setSval(actor,actor.room.vnum,"scatterMine",1);
	}
	
	
}