var script20851 = function(self, actor, here, args, extra) {
	//SMITHing command
	//Koradin - May 2010
	_block;
	//actor.send("Coming soon!");
	getCharCols(actor);
	var vArgs = getArgList(args);
	var skill = actor.getSkill(getSkillVnum("Smithing"));
	if (!vArgs[1])
	{
		var line = ["","","","","","","","","",""];
		for (i = 0; i < getSmithableTypes( actor ).length; i++)
		{
			line[i] += getSmithableTypes( actor )[i][0];
			line[i] += getSmithableTypes( actor )[i][3];
		}
		for (i = 0; i < getSmithableWeapons( actor ).length; i++)
		{
			if (line[i] == "")
				line[i] += "                  ";
			line[i] += getSmithableWeapons( actor )[i][0];
			line[i] += getSmithableWeapons( actor )[i][3];
		}
		for (i = 0; i < getSmithableArmorSlots( actor ).length; i++)
			line[i] += getSmithableArmorSlots( actor )[i][0];
		actor.send("At your skill level, you can "+cyn+"smith"+nrm+" the following items:");
		actor.send("_______________________________________________________");
		actor.send(" ");
		actor.send(cyn+"   MATERIAL          WEAPON              ARMOR"+nrm);
		actor.send(" ");
		for (i = 0; i < 10; i++)
		{
			if (line[i] != "")
				actor.send("   "+line[i]);
		}
		actor.send("_______________________________________________________");
		actor.send(" ");
		actor.send("To smith an item, type "+cyn+"smith "+nrm+"<"+cyn+"material"+nrm+"> <"+cyn+"weapon/armor"+nrm+">");
		//actor.send("For example, "+cyn+"smith copper throwing spear"+nrm);
		return;
	}
	if (!actor.room.isFlagged(constants.ROOM_SMITHING))
	{
		actor.send("You can only smith in a workshop!");
		return;
	}
	if (actor.eq(constants.WEAR_HOLD))
	{
		if (actor.eq(constants.WEAR_HOLD).vnum != 22111)
		{
			actor.send("You need to hold a blacksmith's hammer to start smithing!");
			return;
		}
	}
	else
	{
		actor.send("You need to hold a blacksmith's hammer to start smithing!");
		return;
	}
	var item_found = false;
	var mat_found = false;
	vArgs.splice(0,1);
	var material = vArgs.shift();
	var availableArmor = getSmithableArmorSlots(actor);
	var availableWeapons = getSmithableWeapons(actor);
	var availableMats = getSmithableTypes(actor);
	for each (var armor in availableArmor)
	{
		if (!str_cmp(vArgs.join(" "),armor[0]))
		{
			var type = "abs";
			var end_object = armor;
			item_found = true;
		}
	}
	for each (var weapon in availableWeapons)
	{
		if (!str_cmp(vArgs.join(" "),weapon[0]))
		{
			var type = "weapon";
			var end_object = weapon;
			item_found = true;
		}
	}
	for each (var aMat in availableMats)
	{
		if (!str_cmp(material,aMat[0]))
		{
			var end_mat = aMat;
			mat_found = true;
		}
	}
	if (mat_found == false)
	{
		actor.send("You can't smith that material. Type SMITH to view your currently available materials.");
		return;
	}
	if (item_found == false)
	{
		actor.send("You can't smith that item. Type SMITH to view your currently available items.");
		return;
	}
	for each (var rItem in actor.room.items) {
		var rIName = rItem.namelist.split(" ");
		if (rIName[rIName.length-1] == "smithingforge")
			var forge = rItem;
	}
	if (!forge) {
		actor.send("There's no forge to use here.");
		return;
	}
	function IsLit( forge ) {
		var forgeSval = getSval(forge,20863,"isLit");
		if (!forgeSval || forgeSval == 0)
			return false;
		else
			return true;
	}
	if (IsLit( forge ) == false) {
		actor.send("You have to light the forge first!");
		return;
	}
	setSval(actor,20851,"weapon type",end_object[0]);
	setSval(actor,20851,"weapon mat",end_mat[0]);
	var sval = getSval(actor,20851,"paging");
	if ( (!sval || sval == 0) && type == "weapon")
	{
		if (end_object[0] == "Large Projectile")
		{
			setSval(actor,20851,"paging",3);
			getMenu(actor,getSval(actor,20851,"paging"),"smithing");
			actor.attach(20854);
			return;
		}
		else if (end_object[0] != "Small Projectile" && end_object[0] != "Short Blade")
		{
			setSval(actor,20851,"paging",1);
			getMenu(actor,getSval(actor,20851,"paging"),"smithing");
			actor.attach(20854);
			return;
		}
		else if (end_object[0] == "Short Blade")
		{
			setSval(actor,20851,"paging",4);
			getMenu(actor,getSval(actor,20851,"paging"),"smithing");
			actor.attach(20854);
			return;
		}
	}
	else if (!sval || sval == 0)
	{
		if (end_object[0] == "Shield")
		{
			setSval(actor,20851,"paging",5);
			getMenu(actor,getSval(actor,20851,"paging"),"smithing");
			actor.attach(20854);
			return;
		}
	}
	var cond = getCondition(actor,type);
	var vnumAdd = cond[3];
	//here.echo(getSval(actor,20851,"attribute"));
	if (getSval(actor,20851,"handed") == 2 || getSval(actor,20851,"attribute") == 8 || getSval(actor,20851,"attribute") == 4)
		var rand_name = end_object[8][random(0,end_object[8].length-1)];
	else
		var rand_name = end_object[7][random(0,end_object[7].length-1)];
	var obj_name = end_object[5]+" "+cond[0]+" "+end_mat[0]+" "+rand_name;
	obj_name = obj_name.toLowerCase();
	var cool_msg = cond[1];
	setSval(actor,20851,"paging",null);
	setSval(actor,20851,"weapon type",null);
	setSval(actor,20851,"weapon mat",null);
	var strAttribute = getSval(actor,20851,"attribute");
	var intHanded = getSval(actor,20851,"handed");
	setSval(actor,20851,"handed",null);
	setSval(actor,20851,"attribute",null);
	if (strAttribute > 0)
	{
		if (strAttribute == 1)
			strAttribute = "Offensive";
		else if (strAttribute == 2)
			strAttribute = "Defensive";
		else if (strAttribute == 3)
			strAttribute = "Dodge";
		else if (strAttribute == 4)
			strAttribute = "Parry";
		else if (strAttribute == 5)
			strAttribute = "Abs";
		else if (strAttribute == 6)
			strAttribute = "Weight";
		else if (strAttribute == 7)//throwing spear
			strAttribute = "Spear";
		else if (strAttribute == 8)//throwing axe
			strAttribute = "Axe";
		else if (strAttribute == 9)//short blade dmg
			strAttribute = "Damage";
		else if (strAttribute == 10)//short blade land rate
			strAttribute = "HitRate";
	}
	//here.echo(intHanded);
	//here.echo(typeof intHanded);
	//here.echo(end_mat[0]+ " " +end_object[0]+ " "+getSval(actor,20851,"handed")+ " "+ strAttribute);
	var new_obj = getBaseObject(end_mat[0],end_object[0],intHanded,strAttribute);
	//here.echo(new_obj);
	if (end_object[0] != "Small Projectile" && end_object[0] != "Large Projectile")
		new_obj = new_obj+vnumAdd;
	//here.echo(vnumAdd);
	var actorHasRecipe = false;
	var handCheck = false;
	var matCheck = false;
	var itemCheck = false;
	for (i = 0; i < actor.getRecipes('S').length; i++)
	{
		var sRecipe = actor.getRecipes('S')[i];
		if (sRecipe) {
			matCheck = false;
			itemCheck = false;
			handCheck = false;
			var recipeName = getRecipeByVnum(sRecipe.vnum, 'S').name;
			//here.echo(recipeName);
			if (isName(end_mat[0],recipeName))
				matCheck = true;
			if (isName(end_object[0].split(" ")[0],recipeName))
				itemCheck = true;
			if (isName("one-handed",recipeName) && intHanded == 1)
				handCheck = true;
			if (isName("two-handed",recipeName) && intHanded == 2)
				handCheck = true;
			if (type == "abs" || end_object[0] == "Small Projectile" || end_object[0] == "Large Projectile" || end_object[0] == "Short Blade")
				handCheck = true;
		}
		if (matCheck == true && itemCheck == true && handCheck == true)
		{
			actorHasRecipe = true;
			var pounds = getRecipeByVnum(sRecipe.vnum, 'S').ingredients[0].quantity;
			break;
		}
	}
	if (actorHasRecipe == false)
	{
		//here.echo(end_object[0]+ " mat: "+matCheck + " item: " + itemCheck + " hand: " + handCheck);
		actor.send("You don't know how to smith one of those.");
		return;
	}
	var mat = end_mat[0];
	if (!str_cmp(mat,"tin"))
		var reqs = [ [[4581,4582],pounds,0] ];
	else if (!str_cmp(mat,"aluminum"))
		var reqs = [ [[4577,4578],pounds,0] ];
	else if (!str_cmp(mat,"copper"))
		var reqs = [ [[4579,4580],pounds,0] ];
	else if (!str_cmp(mat,"iron"))
		var reqs = [ [[4575,4576],pounds,0] ];
	else if (!str_cmp(mat,"steel"))
		var reqs = [ [[4565,4566],pounds,0] ];
	else if (!str_cmp(mat,"bronze"))
		var reqs = [ [[4536,4537],pounds,0] ];
	else if (!str_cmp(mat,"silver"))
		var reqs = [ [[4573,4560],pounds,0] ];
	else if (!str_cmp(mat,"gold"))
		var reqs = [ [[4563,4564],pounds,0] ];
	else if (!str_cmp(mat,"platinum"))
		var reqs = [ [[4552,4553],pounds,0] ];
	else if (!str_cmp(mat,"obsidian"))
		var reqs = [ [[4550,4551],pounds,0] ];
	else {
		actor.send("Smith what?!");
		return;
	}
	var ingredients = [];
	var totalWeight = 0;
	for each (var iItem in actor.inventory) {
		for each (var req in reqs) {
			if (req[2] >= req[1])
				continue;
			for each (var vnum in req[0]) {
				if (iItem.vnum == vnum) {
					ingredients.push(iItem);
					req[2] += iItem.weight;
					totalWeight += iItem.weight;
				}
			}
			
		}
	}
	for (i = 0; i < reqs.length; i++) {
		if (reqs[i][2] < reqs[i][1]) {
			actor.send("You don't have enough materials to smith that!  Check your recipe book.");
			return;
		}
	}
	var timer = 10;
	if (skill > 39)
		timer -= 1;
	if (skill > 74)
		timer -= 1;
	if (skill > 98)
		timer -= 1;
	if (IsLit( forge ) == false) {
		actor.send("The forge isn't hot enough for smithing anymore...");
		return;
	}
	waitpulse 1;
	actor.send("\nYou place the materials in the forge, and start to work on the "+end_object[0].toLowerCase()+".");
	act("$n places some materials in the forge, and starts working on a new project.",true,actor,null,null,constants.TO_ROOM);
	actor.startTimer(timer);
	var success1 = runTimer(actor);
	if (!success1 || IsLit( forge ) == false) {
		if (IsLit( forge ) == false)
			var endMsg = " when you see the forge go out.";
		else
			var endMsg = ".";
		actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+endMsg);
		return;
	}
	actor.send("\nTaking the white-hot "+end_mat[0].toLowerCase()+" to the anvil, you start shaping it with strong blows of the hammer.");
	act("$n holds some glowing materials in a pair of heavy tongs, shaping them with sharp blows.",true,actor,null,null,constants.TO_ROOM);
	actor.startTimer(timer);
	var success2 = runTimer(actor);
	if (!success2 || IsLit( forge ) == false) {
		if (IsLit( forge ) == false)
			var endMsg = " when you see the forge go out.";
		else
			var endMsg = ".";
		actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+endMsg);
		return;
	}
	actor.send("\nAfter a quick quench in the barrel, you begin the process again, making sure the forge temperature remains even.");
	act("$n tosses some materials into the quenching barrel with a loud HISS!",true,actor,null,null,constants.TO_ROOM);
	actor.startTimer(timer*2);
	var success3 = runTimer(actor);
	if (!success3 || IsLit( forge ) == false) {
		if (IsLit( forge ) == false)
			var endMsg = " when you see the forge go out.";
		else
			var endMsg = ".";
		actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+endMsg);
		return;
	}
	actor.send("\nYou heat, shape, and quench several more times, and finally start to see the "+end_object[0].toLowerCase()+" coming together.");
	act("$n works $mself into a rhythm between the forge and anvil.",true,actor,null,null,constants.TO_ROOM);
	actor.startTimer(timer);
	var success4 = runTimer(actor);
	if (!success4 || IsLit( forge ) == false) {
		if (IsLit( forge ) == false)
			var endMsg = " when you see the forge go out.";
		else
			var endMsg = ".";
		actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+endMsg);
		return;
	}
	actor.send("\nAfter weighing and balancing, you toss the "+end_object[0].toLowerCase()+" into the quenching barrel one last time.");
	act("A loud HISS accompanies some steam that rises from the quenching barrel.",true,actor,null,null,constants.TO_ROOM);
	actor.startTimer(timer);
	var success4 = runTimer(actor);
	if (!success4 || IsLit( forge ) == false) {
		if (IsLit( forge ) == false)
			var endMsg = " when you see the forge go out.";
		else
			var endMsg = ".";
		actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+endMsg);
		return;
	}
	for each (var iIng in ingredients)
		iIng.extract();
	actor.loadObj(new_obj);
	new_obj = actor.inventory[0];
	new_obj.setRetoolSDesc(obj_name);
	new_obj.setRetoolDesc(capFirstLetter(end_object[5])+" "+end_mat[0].toLowerCase()+" "+rand_name+" lies here.");
	new_obj.setRetoolName(end_mat[0]+" "+cond[0]+" "+end_object[0]+" "+rand_name);
	if (end_object[0] == "Small Projectile" || end_object[0] == "Large Projectile")
	{
		//actor.send("pval set");
		new_obj.setPval("throwing",vnumAdd,true);
	}
	new_obj.setPval("made_by",actor.name,true);
	//actor.send(new_obj.vnum);
	here.echo("\nAs you take the "+end_object[0].toLowerCase()+" from the barrel, "+cool_msg.toLowerCase());
	actor.send("You finish smithing " +obj_name+ ".");
	act("$n finishes smithing "+obj_name+", and wipes $s brow.",true,actor,null,null,constants.TO_ROOM);
	awardPracs(actor,"Smithing",end_mat[0],end_object[0]);
	for each (var player in getConnectedPlayers())
	{
		if (player.name == "Koradin")
			player.send("SMITHING: " +actor.name + " just got " + obj_name);
	}
	
	
	
	
}