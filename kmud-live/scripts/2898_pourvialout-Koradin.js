var script2898 = function(self, actor, here, args, extra)
{
	var argArray = getArgList(args);
getCharCols(actor);
if (!argArray[1] || !getObjInListVis(actor,argArray[1],actor.inventory))
{
	_block;
	actor.send("Pour what?");
	return;
}
if (!argArray[2] || argArray[3])
{
	_block;
	actor.send("You can currently pour the vial "+cyn+"out"+nrm+" onto the ground,");
	actor.send("or into a drinking container such as a "+cyn+"fountain"+nrm+" or "+cyn+"flask"+nrm+".");
	return;
}
var item = getObjInListVis(actor,argArray[1],actor.inventory);
var container = getObjInListVis(actor,argArray[2],actor.inventory);
if (!container)
	var container = getObjInListVis(actor,argArray[2],actor.room.items);
if (!container && str_cmp(argArray[2],"out")) {
	_block;
	actor.send("You can currently pour the vial "+cyn+"out"+nrm+" onto the ground,");
	actor.send("or into a drinking container such as a "+cyn+"fountain"+nrm+" or "+cyn+"flask"+nrm+".");
	return;
}
if (container) {
	if (container.type != constants.ITEM_DRINKCON && container.type != constants.ITEM_FOUNTAIN) {
		actor.send("You can't pour "+self.name+" into "+container.name+"!");
		_block;
		return;
	}
	if (container.type == constants.ITEM_DRINKCON)
		var conType = "liqCon";
	else if (container.type == constants.ITEM_FOUNTAIN)
		var conType = "fountain";
}
if (item == self) {
	_block;
	if (!str_cmp(argArray[2],"out")) {
		actor.send("You empty "+self.name+" out over the ground.");
		act("$n pours "+self.name+" out over the ground, emptying it.",true,actor,null,null,constants.TO_ROOM);
	}
	else if (container) {
		if (self.vnum == 2030) {
			if (self.contents.length == 0)
				actor.send("There's nothing in that to pour...");
			else
				actor.send("The contents wouldn't mix too well...");
			return;
		}
		if (conType == "liqCon" && container.value(constants.VALUE_LIQ_CONTAINER_DRINKS) == 0) {
			actor.send(capFirstLetter(container.name) + " is empty... there's nothing to hide the evidence!");
			return;
		}
		else if (conType == "fountain" && container.value(constants.VALUE_FOUNTAIN_DRINKS) == 0) {
			actor.send(capFirstLetter(container.name) + " is empty... there's nothing to hide the evidence!");
			return;
		}
		var affected = container.getPval("vialEffect");
		if (affected) {
			affected = affected.split("~");
			var vialVnum = parseInt(affected[0]);
			var numEffect = parseInt(affected[1]);
			var timeAffected = parseInt(affected[2]);
			if (vialVnum != self.vnum) {
				// actor.send("There's already something mixed into "+container.name+"!");
				// return;
				numEffect = 0;
			}
			container.deletePval("vialEffect",true);
		}
		else
			var numEffect = 0;
		actor.send("You pour "+self.name+" into "+container.name+".");
		act("$n pours "+self.name+" into "+container.name+".",true,actor,null,null,constants.TO_ROOM);
		numEffect = numEffect + 1;
		container.setPval("vialEffect",self.vnum+"~"+numEffect+"~"+time(),true);
	}
	actor.loadObj(2030);
	//here.echo("Trying to extract: " + self.name);
	self.moveToRoom(getRoom(20899));
	self.extract();
}
else {
	_noblock;
	return;
}

};

