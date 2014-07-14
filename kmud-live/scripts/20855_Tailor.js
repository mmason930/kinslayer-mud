var script20855 = function(self, actor, here, args, extra)
{
	//TAILOR for tailoring
//Koradin - May 2010
_block;
//actor.send("Coming soon!");
getCharCols(actor);
var vArgs = getArgList(args);
var skill = actor.getSkill(getSkillVnum("Tailoring"));
if (!vArgs[1])
{
	var line = ["","","","","","","","","",""];
	for (i = 0; i < getTailoringTypes( actor, "dodge" ).length; i++)
	{
		line[i] += getTailoringTypes( actor, "dodge" )[i][0];
		line[i] += getTailoringTypes( actor, "dodge" )[i][3];
	}
	for (i = 0; i < getTailoringTypes( actor, "combo" ).length; i++)
	{
		line[i] += getTailoringTypes( actor, "combo" )[i][0];
		line[i] += getTailoringTypes( actor, "combo" )[i][3];
	}
	for (i = 0; i < getTailoringArmorSlots( actor ).length; i++)
	{
		if (line[i] == "")
			line[i] += "                                      ";
		line[i] += getTailoringArmorSlots( actor )[i][0];
		line[i] += getTailoringArmorSlots( actor )[i][3];
	}
	actor.send("At your skill level, you can "+cyn+"tailor"+nrm+" the following items:");
	actor.send("________________________________________________________");
	actor.send(" ");
	//actor.send(cyn+"         MATERIALS"+nrm);
	actor.send(cyn+"   DODGE           COMBO                 ARMOR"+nrm);
	actor.send(" ");
	for (i = 0; i < 10; i++)
	{
		if (line[i] != "")
			actor.send("   "+line[i]);
	}
	actor.send("________________________________________________________");
	actor.send(" ");
	actor.send("To tailor an item, type "+cyn+"tailor "+nrm+"<"+cyn+"dodge/combo"+nrm+"> <"+cyn+"armor"+nrm+">");
	return;
}
var room = actor.room;
if (!room.isFlagged(constants.ROOM_TAILORING)) {
	actor.send("You can only prepare materials at a workshop!");
	return;
}
if (actor.eq(constants.WEAR_HOLD))
{
	if (actor.eq(constants.WEAR_HOLD).vnum != 22116) {
		actor.send("You need to hold a tailor's needle to start tailoring!");
		return;
	}
}
else if (!actor.eq(constants.WEAR_HOLD)) {
	actor.send("You need to hold a tailor's needle to start tailoring!");
	return;
}
var item_found = false;
var mat_found = false;
vArgs.splice(0,1);
var item_arg = vArgs.splice(vArgs.length-1,1);
vArgs = vArgs.join(" ");
item_arg = item_arg.join("");
// here.echo(item_arg);
// here.echo(vArgs);
var availableArmor = getTailoringArmorSlots(actor);
var availableDodgeMats = getTailoringTypes(actor,"dodge");
var availableComboMats = getTailoringTypes(actor,"combo");
for each (var armor in availableArmor)
{
	if (!str_cmp(item_arg,armor[0]))
	{
		var end_object = armor;
		item_found = true;
	}
}
for each (var cMat in availableComboMats)
{
	if (!str_cmp(vArgs,cMat[0]))
	{
		var type = "combo";
		var end_mat = cMat;
		mat_found = true;
	}
}
for each (var mat in availableDodgeMats)
{
	if (!str_cmp(vArgs,mat[0]))
	{
		var type = "dodge";
		var end_mat = mat;
		mat_found = true;
	}
}
if (type == "combo") {
	actor.send("Tailoring heavy (combo) equipment is currently still in construction. Check back soon!");
	return;
}
if (mat_found == false)
{
	actor.send("You can't tailor that material. Type TAILOR to view your currently available materials.");
	return;
}
if (item_found == false)
{
	actor.send("You can't tailor that item. Type TAILOR to view your currently available items.");
	return;
}
setSval(actor,20855,"obj type",end_object[0]);
setSval(actor,20855,"obj mat",end_mat[0]);
var sval = getSval(actor,20855,"paging");
if ( (!sval || sval == 0) )
{
	if (type == "combo" && (end_mat[0] == "Scale" || end_mat[0] == "Brigandine") ) {
		// setSval(actor,20855,"paging",11);
		// getMenu(actor,getSval(actor,20855,"paging"),"tailoring");
		// actor.attach(20848);
		// return;
		setSval(actor,20855,"attribute",1);
	}
}
var cond = getCondition(actor,type);
//here.echo(cond);
if (type == "dodge")
	var obj_title = end_object[7][random(0,end_object[7].length-1)];
else
	var obj_title = end_object[8][random(0,end_object[8].length-1)];
var obj_name = end_object[5]+" "+cond[0]+" "+end_mat[0]+" "+obj_title;
obj_name = obj_name.toLowerCase();
//here.echo(obj_name);
if (end_object[5] == "a")
	var cool = " dries completely, ";
else
	var cool = " dry completely, ";
var cool_msg = cond[1];
setSval(actor,20851,"paging",null);
setSval(actor,20851,"obj type",null);
setSval(actor,20851,"obj mat",null);
var strAttribute = getSval(actor,20851,"attribute");
setSval(actor,20851,"attribute",null);
if (strAttribute > 0)
{
	if (strAttribute == 1)
		strAttribute = "Heavier";
	else if (strAttribute == 2)
		strAttribute = "Lighter";
}
var actorHasRecipe = false;
var matCheck = false;
var itemCheck = false;
for (i = 0; i < actor.getRecipes('T').length; i++)
{
	var sRecipe = actor.getRecipes('T')[i];
	if (sRecipe) {
		matCheck = false;
		itemCheck = false;
		var recipeName = getRecipeByVnum(sRecipe.vnum, 'T').name;
		//here.echo(recipeName);
		if (isName(end_mat[0],recipeName))
			matCheck = true;
		if (isName(end_object[0].split(" ")[0],recipeName))
			itemCheck = true;
	}
	if (matCheck == true && itemCheck == true)
	{
		actorHasRecipe = true;
		var thisRec = getRecipeByVnum(sRecipe.vnum, 'T');
		for (i=0;i<thisRec.ingredients.length;i++) {
			if (thisRec.ingredients[i].vnum < 4950 && thisRec.ingredients[i].vnum > 4939) {
				var thread = thisRec.ingredients[i].quantity;
				var threadVnum = thisRec.ingredients[i].vnum;
			}
			else
				var pounds = thisRec.ingredients[i].quantity;
		}
		break;
	}
}
if (actorHasRecipe == false)
{
	//here.echo(end_object[0]+ " mat: "+matCheck + " item: " + itemCheck );
	actor.send("You don't know how to tailor one of those.");
	return;
}
var mat = end_mat[0];
if (!str_cmp(mat,"skin"))
	var reqs = [ [[4951,4952],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"hide"))
	var reqs = [ [[4953,4954],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"cloth"))
	var reqs = [ [[4955,4956],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"linen"))
	var reqs = [ [[4957,4958],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"wool"))
	var reqs = [ [[4959,4960],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"silk"))
	var reqs = [ [[4961,4962],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"velvet"))
	var reqs = [ [[4963,4964],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"fur"))
	var reqs = [ [[4965,4966],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"light leather"))
	var reqs = [ [[4967,4968],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"leather"))
	var reqs = [ [[4969,4970],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"quilted leather"))
	var reqs = [ [[4971,4972],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"padded leather"))
	var reqs = [ [[4973,4974],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"studded leather"))
	var reqs = [ [[4975,4976],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"chainmail"))
	var reqs = [ [[4977,4978],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"scale"))
	var reqs = [ [[4979,4980],pounds,0], [[threadVnum],thread,0] ];
else if (!str_cmp(mat,"brigandine"))
	var reqs = [ [[4981,4982],pounds,0], [[threadVnum],thread,0] ];
else {
	actor.send("Tailor what?!");
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
				//here.echo("pushing "+iItem.name);
				if (iItem.vnum == threadVnum)
					req[2] += 1;
				else
					req[2] += iItem.weight;
				totalWeight += iItem.weight;
			}
		}
		
	}
}
for (i = 0; i < reqs.length; i++) {
	if (reqs[i][2] < reqs[i][1]) {
		// here.echo(reqs[i][0]+" "+reqs[i][2]);
		// for each (var ingg in ingredients)
			// here.echo(ingg.name);
		actor.send("You don't have enough materials to tailor that!  Check your recipe book.");
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
waitpulse 1;
actor.send("\nYou start to work on the "+end_object[0].toLowerCase()+", cutting up squares of material.");
act("$n starts working on a new project, cutting up squares of material.",true,actor,null,null,constants.TO_ROOM);
actor.startTimer(timer);
var success1 = runTimer(actor);
if (!success1) {
	actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+".");
	return;
}
actor.send("\nThreading the needle, you start lacing the "+end_mat[0].toLowerCase()+" together with deft strokes.");
act("$n threads $s needle and starts lacing materials together deftly.",true,actor,null,null,constants.TO_ROOM);
actor.startTimer(timer);
var success2 = runTimer(actor);
if (!success2) {
	actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+".");
	return;
}
actor.send("\nAfter neatly tying off a thread, you carefully begin again, taking time to measure your work.");
act("$n studies $s work, measuring materials and planning $s next moves.",true,actor,null,null,constants.TO_ROOM);
actor.startTimer(timer*2);
var success3 = runTimer(actor);
if (!success3) {
	actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+".");
	return;
}
actor.send("\nYou cut, sew, and tie several more times, and finally start to see the "+end_object[0].toLowerCase()+" coming together.");
act("$n works $mself into a furious rhythm, pieces of material flying everywhere.",true,actor,null,null,constants.TO_ROOM);
actor.startTimer(timer);
var success4 = runTimer(actor);
if (!success4) {
	actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+".");
	return;
}
actor.send("\nAfter shaping and trimming, you wash the "+end_object[0].toLowerCase()+" and set them aside to dry.");
act("Grumbling softly, $n picks apart some stitching.",true,actor,null,null,constants.TO_ROOM);
actor.startTimer(timer);
var success4 = runTimer(actor);
if (!success4) {
	actor.send("\nYou stop working on the "+end_object[0].toLowerCase()+".");
	return;
}
for each (var iIng in ingredients)
	iIng.extract();
here.echo("As the "+end_object[0].toLowerCase()+cool+cool_msg);
var new_obj = getBaseObject(end_mat[0],end_object[0],null,strAttribute);
new_obj = (new_obj+cond[3]);
actor.loadObj(new_obj);
new_obj = actor.inventory[0];
new_obj.setRetoolSDesc(obj_name);
new_obj.setRetoolDesc(capFirstLetter(end_object[5])+" "+end_mat[0].toLowerCase()+" "+obj_title+" lies here.");
new_obj.setRetoolName(end_mat[0]+" "+cond[0]+" "+end_object[0]+" "+obj_title);
actor.send("You finish tailoring " +obj_name+ ".");
act("$n finishes tailoring "+obj_name+", and wrings $s hands.",true,actor,null,null,constants.TO_ROOM);
setSval(actor,20855,"paging",0);
setSval(actor,20855,"obj type",null);
setSval(actor,20855,"obj mat",null);
awardPracs(actor,"Tailoring",end_mat[0],end_object[0]);









};

