var script20951 = function(self, actor, here, args, extra) {
	//Herbalism notice script to be attached to rooms
	//Koradin - Converted from DG May 2010
	//Rewritten by Galnor 08/19/2010
	var inside = constants.SECT_INSIDE;
	var city = constants.SECT_CITY;
	var sWater = constants.SECT_WATER_SWIM;
	var nWater = constants.SECT_WATER_NOSWIM;
	var uWater = constants.SECT_UNDERWATER;
	var flying = constants.SECT_FLYING;
	var road = constants.SECT_ROAD;
	var sect = self.sector;
	waitpulse 1;
	if (actor.room != self)
	return;
	var skill = actor.getSkill(getSkillVnum("Herbalism"));
	if (skill > 49)
		var rand = 125;
	else if (skill > 24)
		var rand = 100;
	else
		var rand = 75;
	if (random(1,rand) < skill )
	{
		getCharCols(actor);
		actor.send(cyn+"You notice some plant life growing here..."+nrm);
		setSval(actor,actor.room.vnum,"herbalism",1);
	}
	/********
	var targets = [];
	targets.push(actor);
	for each (var person in actor.followers)
	{
		if (person.room == actor.room)
			targets.push(person);
	}
	for each (var target in targets)
	{
		var msg = "$n leaves " +dirToText(revDir(extra.direction));
		var enter_msg = "$n has arrived from the " +dirToText(extra.direction);
		if (target.mount)
		{
			target.mount.moveToRoom(self);
			msg += ", riding a "+target.mount.name+".";
			enter_msg += ", riding a "+target.mount.name+".";
		}
		else
		{
			msg += ".";
			enter_msg += ".";
		}
		act(msg,true,target,null,null,constants.TO_ROOM);
		target.moveToRoom(self);
		act(enter_msg,true,target,null,null,constants.TO_ROOM);
		target.comm("look");
		if (target.vnum == -1)
		{
			var skill = target.getSkill(getSkillVnum("Herbalism"));
			if (skill > 49)
				var rand = 125;
			else if (skill > 24)
				var rand = 100;
			else
				var rand = 75;
			if (random(1,rand) < skill )
			{
				getCharCols(target);
				target.send(cyn+"You notice some plant life growing here..."+nrm);
				setSval(target,target.room.vnum,"herbalism",1);
			}
		}
	}
	********/
}