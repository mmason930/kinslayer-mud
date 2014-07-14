var script20878 = function(self, actor, here, args, extra)
{
	//Prepare command for tailoring
//Koradin - February 2012
_block;
var room = actor.room;
if (!room.isFlagged(constants.ROOM_TAILORING)) {
	actor.send("You can only prepare materials at a workshop!");
	return;
}
var skill = actor.getSkill(getSkillVnum("Tailoring"));
var argArray = getArgList(args);
if (!argArray[1]) {
	actor.send("Prepare what?!");
	return;
}
//PREPARING TAILORING MATS
	
var mat = argArray[1];
if (!str_cmp(mat,"skin"))
	var reqs = [ [[4925,4926,4927],6,0] ];
else if (!str_cmp(mat,"hide"))
	var reqs = [ [[4928,4929,4930],6,0] ];
else if (!str_cmp(mat,"cloth"))
	var reqs = [ [[4910,4911,4912],6,0] ];
else if (!str_cmp(mat,"linen"))
	var reqs = [ [[4913,4914,4915],6,0] ];
else if (!str_cmp(mat,"wool"))
	var reqs = [ [[4916,4917,1918],6,0] ];
else if (!str_cmp(mat,"silk"))
	var reqs = [ [[4919,4920,4921],6,0] ];
else if (!str_cmp(mat,"velvet"))
	var reqs = [ [[4922,4923,4924],6,0] ];
else if (!str_cmp(mat,"fur"))
	var reqs = [ [[4931,4932,4933],6,0] ];
else if (!str_cmp(mat,"light"))
	var reqs = [ [[4925,4926,4927],6,0] ];
else if (!str_cmp(mat,"leather"))
	var reqs = [ [[4928,4929,4930],6,0] ];
else if (!str_cmp(mat,"quilted"))
	var reqs = [ [[4925,4926,4927],6,0], [[4928,4929,4930],6,0] ];
else if (!str_cmp(mat,"padded"))
	var reqs = [ [[4928,4929,4930],6,0], [[4910,4911,4912],6,0] ];
else if (!str_cmp(mat,"studded"))
	var reqs = [ [[4928,4929,4930],6,0], [[4575,4576],6,0] ];
else if (!str_cmp(mat,"chainmail"))
	var reqs = [ [[4913,4914,4915],6,0], [[4565,4566],6,0]  ];
else if (!str_cmp(mat,"scale"))
	var reqs = [ [[4937,4938,4939],9,0] ];
else if (!str_cmp(mat,"brigandine"))
	var reqs = [ [[4969,4970],6,0], [[4957,4958],3,0], [[4977,4978],6,0] ];
else {
	actor.send("Try PREPARE <MATERIAL> instead.");
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
var matPrep = true;
//NOT ENOUGH MATS TO PREP = CHECK OTHER MATS
for (i = 0; i < reqs.length; i++) {
	if (reqs[i][2] < reqs[i][1]) {
		matPrep = false;
		totalWeight = 0;
		ingredients = [];
		for each (var prepObj in actor.inventory) {
			var prepProto = getObjProto(prepObj.vnum);
			if (isName(argArray[1], prepObj.namelist) && !isName("material",prepProto.name) && !isName("square",prepProto.name)) {
				ingredients.push(prepObj);
				var objWeight = prepObj.weight;
				if (getDecayProgress(prepObj) > 0)
					objWeight *= ((100 - getDecayProgress(prepObj)) / 100);
				if (prepObj.vnum > 31079 && prepObj.vnum < 32378) { // Item is tailored
					if (objWeight < 2)
						objWeight = 2; // Tailoring items can't give less than 1 large square back
				}
				else
					objWeight *= 0.6;
				if (isName("spool", prepProto.namelist))
						objWeight /= 10;
			// here.echo(prepObj.name + " = " + objWeight);
			totalWeight += objWeight;
			}
		}
		if (!str_cmp(argArray[1],"skin"))
			var barVnums = [4951,4952];
		else if (!str_cmp(argArray[1],"hide"))
			var barVnums = [4953,4954];
		else if (!str_cmp(argArray[1],"cloth"))
			var barVnums = [4955,4956];
		else if (!str_cmp(argArray[1],"linen"))
			var barVnums = [4957,4958];
		else if (!str_cmp(argArray[1],"wool"))
			var barVnums = [4959,4960];
		else if (!str_cmp(argArray[1],"silk"))
			var barVnums = [4961,4962];
		else if (!str_cmp(argArray[1],"velvet"))
			var barVnums = [4963,4964];
		else if (!str_cmp(argArray[1],"fur"))
			var barVnums = [4965,4966];
		else if (!str_cmp(argArray[1],"light")) {
			var barVnums = [4967,4968];
			var leather = "leather";
		}
		else if (!str_cmp(argArray[1],"leather"))
			var barVnums = [4969,4970];
		else if (!str_cmp(argArray[1],"quilted")) {
			var barVnums = [4971,4972];
			var leather = "leather";
		}
		else if (!str_cmp(argArray[1],"padded")) {
			var barVnums = [4973,4974];
			var leather = "leather";
		}
		else if (!str_cmp(argArray[1],"studded")) {
			var barVnums = [4975,4976];
			var leather = "leather";
		}
		else if (!str_cmp(argArray[1],"chainmail"))
			var barVnums = [4977,4978];
		else if (!str_cmp(argArray[1],"scale"))
			var barVnums = [4979,4980];
		else if (!str_cmp(argArray[1],"brigandine"))
			var barVnums = [4981,4982];
		if (leather)
			mat = mat+" "+leather;
		if (totalWeight < 1) {
			actor.send("You don't have enough materials to prepare "+mat+"!");
			return;
		}
		var resBars = totalWeight / 2;
		var numLarge = 0;
		var numSmall = 0;
		resBars = resBars.toString().split(".");
		numLarge += parseInt(resBars[0]);
		if (resBars[1]) {
			if (parseInt(resBars[1].charAt(0)) > 4)
				numSmall += 1;
		}
		if (getSval(actor,20859,"prepConfirm") != 1) {
			actor.send(" ");
			actor.send("Preparing "+mat+" will destroy the following items:");
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
			setSval(actor,20859,"prepType",argArray[1]);
			actor.attach(20877);
			return;
		}
		break;
	}
}
if (matPrep == false) {
	var results = [];
	for (i=0;i<numLarge;i++)
		results.push(barVnums[1]);
	for (i=0;i<numSmall;i++)
		results.push(barVnums[0]);
}
else {
	//here.echo(mat+"~"+totalWeight);
	var results = getPreparingResults(mat, totalWeight);
}
actor.detach(20877);
setSval(actor,20859,"prepConfirm",0);
setSval(actor,20859,"prepType",null);
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
	ingItem.moveToRoom(getRoom(20800));
	ingItem.extract();
}
waitpulse 1;
actor.send("\nYou wash the materials and begin intricately weaving them together...");
actor.startTimer(timer);
var success = runTimer(actor);
if (!success) {
	actor.send("\nYou give up early, leaving the ruined pieces where they are.");
	return;
}
if (!results || results == 0) {
	//here.echo(results);
	actor.send("\nAs you wash and weave, you realize you didn't start with enough materials to finish a square!");	
	return;
}
actor.send("\nYou finish weaving the "+mat+" and set it on the rack to dry.");
for each (var thing in room.items) {
	for each (var name in thing.namelist.split(" ")) {
		if (name == "dryingrack") {
			var rack = thing;
			break;
		}
	}
}
//here.echo(results);
for each (var result in results) {
	wait 5;
	if (rack) {
		rack.loadObj(result);
		room.echo("A square of "+mat + " has dried completely and is available on the rack.");
	}
	else {
		wait 20;
		room.loadObj(result);
		room.echo("A square of "+mat+ " dries completely, drifting its way to the floor.");
	}
}






};

