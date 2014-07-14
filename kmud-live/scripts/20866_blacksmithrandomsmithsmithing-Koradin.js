var script20866 = function(self, actor, here, args, extra)
{
	//random blacksmith smithing
//Koradin - May 2010
if (getSval(self,20866,"working") == 1)
	return;
setSval(self,20866,"working",1);
var room = self.room;
var name = capFirstLetter(self.name);
var smithLitTheForge = false;
for each (var rItem in room.items) {
	var rIName = rItem.namelist.split(" ");
	if (rIName[rIName.length-1] == "smithingforge")
		var forge = rItem;
}
if (!forge)
	return;
function IsLit( forge ) {
	var forgeSval = getSval(forge,20863,"isLit");
	if (!forgeSval || forgeSval == 0)
		return false;
	else
		return true;
}
if (IsLit( forge ) == false) {
	room.echo(name + " lights the forge and begins working on a new project.");
	setSval(forge,20863,"isLit", time()+1000);
	room.echo("The forge roars to life.");
	smithLitTheForge = true;
}
else
	room.echo(name + " begins working on a new project.");
var itemArr = ["boots", "gauntlets", "belt", "spaulders", "helmet", "greaves", "vambraces", "breastplate", "shield", "small projectile", "short blade", "large projectile", "spear", "lance", "staff", "long blade", "chain", "axe", "club"];
var itemArg = itemArr[random(0,itemArr.length-1)];
var matRoll = random(1,100);
if (matRoll < 12)
	var matArg = "obsidian";
else if (matRoll < 46)
	var matArg = "platinum";
else if (matRoll < 71)
	var matArg = "gold";
else if (matRoll < 91)
	var matArg = "silver";
else
	var matArg = "bronze";
var handArg = random(1,2);
if (itemArg == "short blade" || itemArg == "small projectile" || itemArg == "large projectile")
	handArg = 1;
var availableArmor = getSmithableArmorSlots(self);
var availableWeapons = getSmithableWeapons(self);
var availableMats = getSmithableTypes(self);
for each (var armor in availableArmor)
{
	if (!str_cmp(itemArg,armor[0]))
	{
		var type = "abs";
		var end_object = armor;
	}
}
for each (var weapon in availableWeapons)
{
	if (!str_cmp(itemArg,weapon[0]))
	{
		var type = "weapon";
		var end_object = weapon;
	}
}
for each (var aMat in availableMats)
{
	if (!str_cmp(matArg,aMat[0]))
		var end_mat = aMat;
}
var cond = getCondition(self,type);
var vnumAdd = cond[3];
if (itemArg == "shield")
	var attArg = random(3,4);
else if (itemArg == "short blade")
	var attArg = random(9,10);
else if (itemArg == "large projectile")
	var attArg = random(7,8);
else if ((itemArg == "chain" || itemArg == "lance" || itemArg == "spear" || itemArg == "axe") && handArg == 1)
	var attArg = 2;
else if ((itemArg == "axe" || itemArg == "chain") && handArg == 2)
	var attArg = 1;
else if (itemArg == "staff")
	var attArg = 2;
else if (itemArg == "club")
	var attArg = 1;
else
	var attArg = random(1,2);
//here.echo(getSval(actor,20851,"attribute"));
if (handArg == 2 || attArg == 8 || attArg == 4) {
	if (end_object[8])
		var rand_name = end_object[8][random(0,end_object[8].length-1)];
	else
		var rand_name = end_object[7][random(0,end_object[7].length-1)];
}
else
	var rand_name = end_object[7][random(0,end_object[7].length-1)];
var obj_name = end_object[5]+" "+cond[0]+" "+end_mat[0]+" "+rand_name;
obj_name = obj_name.toLowerCase();
if (end_object[5] == "a")
{
	var cool_msg = cond[1];
	var cool = " cools, it ";
}
else
{
	var cool_msg = cond[1].split(" ");
	cool_msg[0] = cool_msg[0].slice(0,-1);
	cool_msg = cool_msg.join(" ");
	var cool = " cool, they ";
}
var strAttribute = attArg;
var intHanded = handArg;
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
var new_obj = getBaseObject(end_mat[0],end_object[0],intHanded,strAttribute);
if (end_object[0] != "Small Projectile" && end_object[0] != "Large Projectile")
	new_obj = new_obj+vnumAdd;
wait 15;
if (IsLit( forge ) == false) {
	room.echo(name + " relights the forge, pumping the bellows to keep the coals burning brightly.");
	setSval(forge,20863,"isLit",time()+1000);
	room.echo("The forge roars to life.");
	smithLitTheForge = true;
}
var firStep = [name+" places some material in the forge, taking a short break as it heats.", "Walking to the forge, "+self.name+" pumps the bellows in one flowing motion.", name+" checks on the color of some materials inside the forge."];
var secStep = ["Taking a pair of tongs, "+self.name+" removes some material from the forge.", name+" heads to the anvil and begins shaping some materials.", "As the materials glow nearly white, "+self.name+" removes them from the forge."];
var thiStep = [name+" works rhythmically with the hammer, shaping some material against the anvil.", name+" suddenly tosses something into the quenching barrel with a loud HISS!", "Muttering, "+self.name+" drops something in the quenching barrel and returns to the forge."];
var rounds = random(2,4);
while ( rounds > 0 ) {
	wait 15;
	if (IsLit( forge ) == false) {
		room.echo(name + " relights the forge, pumping the bellows to keep the coals burning brightly.");
		setSval(forge,20863,"isLit",time()+1000);
		room.echo("The forge roars to life.");
		smithLitTheForge = true;
	}
	room.echo(firStep[random(0,firStep.length-1)]);
	wait 15;
	if (IsLit( forge ) == false) {
		room.echo(name + " relights the forge, pumping the bellows to keep the coals burning brightly.");
		setSval(forge,20863,"isLit",time()+1000);
		room.echo("The forge roars to life.");
		smithLitTheForge = true;
	}
	room.echo(secStep[random(0,secStep.length-1)]);
	wait 15;
	if (IsLit( forge ) == false) {
		room.echo(name + " relights the forge, pumping the bellows to keep the coals burning brightly.");
		setSval(forge,20863,"isLit",time()+1000);
		room.echo("The forge roars to life.");
		smithLitTheForge = true;
	}
	room.echo(thiStep[random(0,thiStep.length-1)]);
	rounds--;
}
wait 15;
room.echo(name + " finishes up the last details of the "+end_object[0].toLowerCase()+", holding it up to the light.");
wait 2;
for each (var nRoom in self.room.neighbors) {
	if (nRoom) {
		for each (var nPerson in nRoom.people) {
			for each (var nName in nPerson.namelist.split(" ")) {
				if (nName == "dude")
					var apprentice = nPerson;
			}
		}
	}
}
if (apprentice) {
	if (random(1,20) == 1) {
		new_obj = 1515;
		obj_name = getObjProto(new_obj).name;
		self.comm("yell I've just finished smithing "+ obj_name+"! See my apprentice if you're interested.");
		room.echo("As the apprentice appears in the doorway, "+self.name+" says 'Make yourself useful and sell this for me!'");
		room.echo("The blacksmith's apprentice nods quickly, taking the item back outside with him.");
		apprentice.loadObj(new_obj);
		apprentice.comm("put all pack");
	}
	else {
		self.comm("yell I've just finished smithing "+ obj_name+"! See my apprentice if you're interested.");
		room.echo("As the apprentice appears in the doorway, "+self.name+" says 'Make yourself useful and sell this for me!'");
		room.echo("The blacksmith's apprentice nods quickly, taking the item back outside with him.");
		apprentice.loadObj(new_obj);
		new_obj = apprentice.inventory[0];
		new_obj.setRetoolSDesc(obj_name);
		new_obj.setRetoolDesc(capFirstLetter(end_object[5])+" "+end_mat[0].toLowerCase()+" "+rand_name+" lies here.");
		new_obj.setRetoolName(end_mat[0]+" "+cond[0]+" "+end_object[0]+" "+rand_name);
		if (end_object[0] == "Small Projectile" || end_object[0] == "Large Projectile")
		{
			//actor.send("pval set");
			new_obj.setPval("throwing",vnumAdd,true);
		}
		apprentice.comm("put all pack");
	}
}
setSval(self,20866,"working",0);
if (smithLitTheForge == true) {
	setSval(forge,20863,"isLit",0);
	room.echo("A large stone forge dies down, the coal inside crumbling to ash.");
}



};

