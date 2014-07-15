var script20859 = function(self, actor, here, args, extra) {
	//Smelting in prep. for smithing
	//Koradin - April 2011
	_block;
	var room = actor.room;
	if (!room.isFlagged(constants.ROOM_SMITHING)) {
		actor.send("You can only smelt at a forge!");
		return;
	}
	var skill = actor.getSkill(getSkillVnum("Smithing"));
	// if (skill < 10) {
		// actor.send("You have no idea how to refine ore.");
		// return;
	// }
	var argArray = getArgList(args);
	if (!argArray[1] || argArray[2]) {
		actor.send("Smelt what?!");
		return;
	}
	//SMELTING SMITHED ITEMS BACK DOWN TO BARS
		
	var mat = argArray[1];
	if (!str_cmp(mat,"tin"))
		var reqs = [ [[4596,4587],6,0] ];
	else if (!str_cmp(mat,"aluminum"))
		var reqs = [ [[4597,4586],6,0] ];
	else if (!str_cmp(mat,"copper"))
		var reqs = [ [[4588,4595],6,0] ];
	else if (!str_cmp(mat,"iron"))
		var reqs = [ [[4599,4598],6,0] ];
	else if (!str_cmp(mat,"steel"))
		var reqs = [ [[4599,4598],6,0], [[4583],4,0] ];
	else if (!str_cmp(mat,"bronze"))
		var reqs = [ [[4588,4595],6,0], [[4596,4587],3,0], [[4597,4586],3,0] ];
	else if (!str_cmp(mat,"silver"))
		var reqs = [ [[4590,4593],6,0] ];
	else if (!str_cmp(mat,"gold"))
		var reqs = [ [[4591,4592],6,0] ];
	else if (!str_cmp(mat,"platinum"))
		var reqs = [ [[4589,4594],6,0] ];
	else if (!str_cmp(mat,"obsidian"))
		var reqs = [ [[4603,4585],6,0] ];
	else if (!str_cmp(mat,"coal"))
		var reqs = [ [[4583],4,0] ];
	else {
		actor.send("Try SMELT <MATERIAL> instead.");
		return;
	}
	var ingredients = [];
	var totalWeight = 0;
	for each (var item in actor.inventory) {
		for each (var req in reqs) {
			for each (var vnum in req[0]) {
				if (item.vnum == vnum) {
					ingredients.push(item);
					req[2] += item.weight;
					totalWeight += item.weight;
				}
			}
			
		}
	}
	//here.echo(totalWeight);
	var oreSmelt = true;
	//NOT ENOUGH ORE TO SMELT = CHECK OTHER SMELTABLE ITEMS
	for (i = 0; i < reqs.length; i++) {
		if (reqs[i][2] < reqs[i][1]) {
			oreSmelt = false;
			totalWeight = 0;
			ingredients = [];
			for each (var smeltObj in actor.inventory) {
				var smeltProto = getObjProto(smeltObj.vnum);
				if (isName(argArray[1], smeltObj.namelist) && !isName("ore",smeltProto.name) && !isName("bar",smeltProto.name)) {
					ingredients.push(smeltObj);
					var objWeight = smeltObj.weight;
					if (getDecayProgress(smeltObj) > 0)
						objWeight *= ((100 - getDecayProgress(smeltObj)) / 100);
					if (smeltObj.vnum < 2600 || smeltObj.vnum > 2699 && smeltObj.vnum < 11300 || smeltObj.vnum > 12999 && smeltObj.vnum < 30000 || smeltObj.vnum > 31499) {
						if (!isName(argArray[1], smeltProto.namelist))
							objWeight /= 100;
						else if (smeltObj.vnum > 4584 && smeltObj.vnum < 4604 || smeltObj.absorb < 10 && smeltObj.wearPos != constants.ITEM_WEAR_SHIELD && smeltObj.type != constants.ITEM_WEAPON && smeltObj.weight > 0.9)
							objWeight /= 10;
						else if (smeltObj.weight > 0.9)
							objWeight /= 3;
					}
					else
						objWeight *= 0.6;
				// here.echo(smeltObj.name + " = " + objWeight);
				totalWeight += objWeight;
				}
			}
			if (totalWeight < 1) {
				actor.send("You don't have enough materials to smelt "+argArray[1]+"!");
				return;
			}
			if (!str_cmp(argArray[1],"tin"))
				var barVnums = [4581,4582];
			else if (!str_cmp(argArray[1],"aluminum"))
				var barVnums = [4577,4578];
			else if (!str_cmp(argArray[1],"copper"))
				var barVnums = [4579,4580];
			else if (!str_cmp(argArray[1],"iron"))
				var barVnums = [4575,4576];
			else if (!str_cmp(argArray[1],"steel"))
				var barVnums = [4565,4566];
			else if (!str_cmp(argArray[1],"bronze"))
				var barVnums = [4536,4537];
			else if (!str_cmp(argArray[1],"silver"))
				var barVnums = [4573,4560];
			else if (!str_cmp(argArray[1],"gold"))
				var barVnums = [4563,4564];
			else if (!str_cmp(argArray[1],"platinum"))
				var barVnums = [4553,4552];
			else if (!str_cmp(argArray[1],"obsidian"))
				var barVnums = [4550,4551];
			var resBars = totalWeight / 2;
			var numLarge = 0;
			var numSmall = 0;
			resBars = resBars.toString().split(".");
			numLarge += parseInt(resBars[0]);
			if (resBars[1]) {
				if (parseInt(resBars[1].charAt(0)) > 4)
					numSmall += 1;
			}
			if (getSval(actor,20859,"smeltConfirm") != 1) {
				actor.send(" ");
				actor.send("Smelting "+argArray[1]+" will destroy the following items:");
				actor.send("______________________________________________________");
				actor.send(" ");
				for each (var desItem in ingredients)
					actor.send(desItem.name);
				actor.send("______________________________________________________");
				actor.send(" ");
				actor.send("Are you sure you wish to continue?");
				actor.send(" ");
				getCharCols(actor);
				actor.send("["+cyn+"Y"+nrm+"] Yes      ["+cyn+"N"+nrm+"] No");
				setSval(actor,20859,"smeltType",argArray[1]);
				actor.attach(20879);
				return;
			}
			break;
		}
	}
	if (oreSmelt == false) {
		var results = [];
		for (i=0;i<numLarge;i++)
			results.push(barVnums[1]);
		for (i=0;i<numSmall;i++)
			results.push(barVnums[0]);
	}
	else {
		//here.echo(mat+"~"+totalWeight);
		var results = getSmeltingResults(mat, totalWeight);
	}
	actor.detach(20879);
	setSval(actor,20859,"smeltConfirm",0);
	setSval(actor,20859,"smeltType",null);
	// here.echo(results+ " ~ " + totalWeight);
	// return;
	var timer = 10;
	if (skill > 39)
		timer -= 2;
	if (skill > 74)
		timer -= 2;
	if (skill > 98)
		timer -= 2;
	//here.echo(ingredients);
	for each (var ingItem in ingredients) {
		ingItem.moveToRoom(getRoom(20899));
		ingItem.extract();
	}
	waitpulse 1;
	actor.send("\nYou place the materials in the forge and start to work the bellows...");
	room.echo("The forge roars to life as the oxygen ignites.");
	actor.startTimer(timer);
	var success = runTimer(actor);
	if (!success) {
		actor.send("\nYou give up the smelting attempt, abandoning your materials.");
		room.echo("The forge quiets, leaving only glowing coals.");
		return;
	}
	if (!results || results == 0) {
		//here.echo(results);
		actor.send("\nAs the impurities are burned off, you realize there isn't enough left to fill even the smallest molding!");	
		room.echo("The forge quiets, leaving only glowing coals.");
		return;
	}
	actor.send("\nAs the impurities are burned off, the molten "+mat+" flows into the molding and begins to cool.");
	room.echo("The forge quiets, leaving only glowing coals.");
	for each (var thing in room.items) {
		for each (var name in thing.namelist.split(" ")) {
			if (name == "quenching") {
				var barrel = thing;
				break;
			}
		}
	}
	//here.echo(results);
	for each (var result in results) {
		wait 5;
		if (barrel) {
			barrel.loadObj(result);
			room.echo("The bar of "+mat + " cools enough to drop into the quenching barrel with a loud HISS!");
		}
		else {
			wait 20;
			room.loadObj(result);
			room.echo("The bar of "+mat+ " cools completely, dropping from the molding onto the floor with a loud CLANG!");
		}
	}
}