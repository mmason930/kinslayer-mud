var script20875 = function(self, actor, here, args, extra) {
	//random blacksmith smithing
	//Koradin - May 2010
	if (getSval(self,20866,"working") == 1)
		return;
	setSval(self,20866,"working",1);
	var room = self.room;
	var name = capFirstLetter(self.name);
	room.echo(name + " begins working on a new project.");
	var itemArr = ["boots", "gloves", "belt", "pads", "hat", "leggings", "sleeves", "vest", "cloak"];
	var itemArg = itemArr[random(0,itemArr.length-1)];
	var matRoll = random(1,100);
	if (matRoll < 12)
		var matArg = (random(1,2)==1?"fur":"brigandine");
	else if (matRoll < 46)
		var matArg = (random(1,2)==1?"velvet":"scale");
	else if (matRoll < 71)
		var matArg = (random(1,2)==1?"silk":"chainmail");
	else if (matRoll < 96)
		var matArg = (random(1,2)==1?"wool":"studded leather");
	else
		var matArg = (random(1,2)==1?"linen":"padded leather");
	if (matArg == "linen" || matArg == "wool" || matArg == "silk" || matArg == "velvet" || matArg == "fur")
		var type = "dodge";
	else {
		setSval(self,20866,"working",0);
		return; // FOR NOW
		var type = "combo";
	}
	var availableArmor = getTailoringArmorSlots(self);
	var availableMats = getTailoringTypes(self, type);
	for (var _autoKey in availableArmor) {
		var armor = availableArmor[_autoKey];
		if (!str_cmp(itemArg,armor[0]))
			var end_object = armor;
	}
	for (var _autoKey in availableMats) {
		var aMat = availableMats[_autoKey];
		if (!str_cmp(matArg,aMat[0]))
			var end_mat = aMat;
	}
	var cond = getCondition(self,type);
	var vnumAdd = cond[3];
	if (type == "combo")
		var attArg = random(1,2);
	else
		var attArg = random(1,2);
	var rand_name = end_object[7][random(0,end_object[7].length-1)];
	var obj_name = end_object[5]+" "+cond[0]+" "+end_mat[0]+" "+rand_name;
	obj_name = obj_name.toLowerCase();
	if (end_object[5] == "a")
	{
		var cool_msg = cond[1];
		var cool = " dries completely, ";
	}
	else
	{
		var cool_msg = cond[1];
		var cool = " dry completely, ";
	}
	var strAttribute = attArg;
	if (strAttribute > 0)
	{
		if (strAttribute == 1)
			strAttribute = "Heavier";
		else if (strAttribute == 2)
			strAttribute = "Lighter";
	}
	var new_obj = getBaseObject(end_mat[0],end_object[0],null,strAttribute);
	new_obj = (new_obj+vnumAdd);
	wait 15;
	var firStep = [name+" cuts some material into strips with tailor's scissors.", "Walking to wash basin, "+self.name+" starts preparing some materials.", name+" strides to the work bench and begins sharpening a pair of scissors."];
	var secStep = [name+" threads a needle and begins sewing some materials together.", name+" shapes some materials against a molding on the work bench.", "A square of material rips, and "+self.name+" mutters angrily."];
	var thiStep = [name+" picks out some stitching and starts again.", name+" sets one piece aside to dry and starts measuring another.", name+" works furiously, strips of fabric flying everywhere."];
	var rounds = random(2,4);
	while ( rounds > 0 ) {
		wait 15;
		room.echo(firStep[random(0,firStep.length-1)]);
		wait 15;
		room.echo(secStep[random(0,secStep.length-1)]);
		wait 15;
		room.echo(thiStep[random(0,thiStep.length-1)]);
		rounds--;
	}
	wait 15;
	room.echo(name + " finishes up the last details of the "+end_object[0].toLowerCase()+", holding it up to the light.");
	wait 2;
	for (var _autoKey in self.room.neighbors) {
		var nRoom = self.room.neighbors[_autoKey];
		if (nRoom) {
			for (var _autoKey in nRoom.people) {
				var nPerson = nRoom.people[_autoKey];
				for (var _autoKey in nPerson.namelist.split(" ")) {
					var nName = nPerson.namelist.split(" ")[_autoKey];
					if (nName == "dude")
						var apprentice = nPerson;
				}
			}
		}
	}
	if (apprentice) {
			self.comm("yell I've just finished tailoring "+ obj_name+"! See my apprentice if you're interested.");
			room.echo("As the apprentice appears in the doorway, "+self.name+" says 'Make yourself useful and sell this for me!'");
			room.echo("The tailor's apprentice nods quickly, taking the item back outside with him.");
			apprentice.loadObj(new_obj);
			new_obj = apprentice.inventory[0];
			new_obj.setRetoolSDesc(obj_name);
			new_obj.setRetoolDesc(capFirstLetter(end_object[5])+" "+end_mat[0].toLowerCase()+" "+rand_name+" lies here.");
			new_obj.setRetoolName(end_mat[0]+" "+cond[0]+" "+end_object[0]+" "+rand_name);
			apprentice.comm("put all pack");
	}
	setSval(self,20866,"working",0);
	
	
	
	
	
}