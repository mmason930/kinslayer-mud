var script20970 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
if (!vArgs[1]) {
	actor.send("Who do you want to consider?");
	return;
}
var vict = getCharInListVis(actor, vArgs[1], actor.room.people);
/** if (actor.fighting) {
	actor.send("You're a little too busy for that right now!");
	return;
} **/
if (!str_cmp(vArgs[1],"me") || !str_cmp(vArgs[1],"self") || vict == actor) {
	actor.send("Lookin' damn sexy, "+actor.name+"!");
	act("$n checks $mself out.",true,actor,null,null,constants.TO_ROOM);
	return;
}
if (!vict) {
	actor.send("You don't see that person here.");
	return;
}
//sendKoradin(vict.race);
var weapons = ["sword","dagger","club","staff","spear","axe","chain","bow","lance"];
var awt = actor.weight;
var vwt = vict.weight;
var aht = actor.height;
var vht = vict.height;
var adb = actor.dodge;
var vdb = vict.dodge;
var aob = actor.offense;
var vob = vict.offense;
var apb = actor.parry;
var vpb = vict.parry;
var aabs = actor.absorb;
var vabs = vict.absorb;
var alvl = actor.level;
var vlvl = vict.level;
var astr = actor.strength;
var vstr = vict.strength;
var aint = actor.intelligence;
var vint = vict.intelligence;
var awis = actor.wisdom;
var vwis = vict.wisdom;
var adex = actor.dexterity;
var vdex = vict.dexterity;
var acon = actor.constitution;
var vcon = vict.constitution;
var ahps = actor.maxHit;
var vhps = vict.maxHit;
var amvs = actor.maxMove;
var vmvs = vict.maxMove;
var asps = actor.maxSpell;
var vsps = vict.maxSpell;
var awpn = actor.eq(18);
var vwpn = vict.eq(18);
var awpnWeight = 0;
var awpnDmg = 0;
var vwpnWeight = 0;
var vwpnDmg = 0;
var aScore = 0;
var vScore = 0;
var aSub = alvl+astr+aint+awis+adex+acon+aabs+aob;
var vSub = vlvl+vstr+vint+vwis+vdex+vcon+vabs+vob;
aSub += Math.floor(ahps/10);
aSub += Math.floor(amvs/10);
vSub += Math.floor(vhps/10);
vSub += Math.floor(vmvs/10);
aSub += Math.floor((adb+apb)/6);
vSub += Math.floor((vdb+vpb)/6);
aScore += aSub;
vScore += vSub;
var pros = [];
var cons = [];
var he = capFirstLetter(vict.he_she());
if (awpn) {
	awpnWeight = awpn.weight;
	awpnDmg = Math.floor( (awpn.value(2)+awpn.value(1))/2 );
	awpnType = weapons[awpn.value(0)];
}
else if (!awpn)
	cons.push("You are unarmed.");
if (vwpn) {
	//sendKoradin(vwpn);
	vwpnWeight = vwpn.weight;
	vwpnDmg = Math.floor( (vwpn.value(2)+vwpn.value(1))/2 );
	vwpnType = weapons[vwpn.value(0)];
}
else if (!vwpn && vict.race < 4)
	pros.push(he+" is unarmed.");
if (vwpnDmg-awpnDmg>2 && vwpn) {
	cons.push(capFirstLetter(vict.his_her())+" "+vwpnType+" looks deadly.");
}
else if (awpnDmg-vwpnDmg>2 && vwpn) {
	pros.push("Your "+awpnType+" beats "+vict.his_her()+" "+vwpnType+".");
}
if (vict.mobFlagged(constants.MOB_NOFIGHT)) {
	pros.push(he+" won't fight back.");
	aScore += 50;
}
if (aob-10>vob)
	pros.push("You are more aggressive.");
else if (vob-10>aob)
	cons.push(he+" is more aggressive.");
if (adb+apb+10 < vdb+vpb)
	cons.push(he+" is more evasive.");
else if (adb+apb-10 > vdb+vpb)
	pros.push("You are more evasive.");
if (aabs-5>vabs)
	pros.push("You are better armored.");
else if (aabs+5<vabs)
	cons.push(he+" is better armored.");
if (asps-5>vsps && asps > 150 && actor.sex == vict.sex && vict.vnum == -1)
	pros.push("You are stronger in the Power.");
else if (vsps-5>asps && asps > 150 && actor.sex == vict.sex && vict.vnum == -1)
	cons.push(he+" is stronger in the Power.");
if (alvl-5>vlvl)
	pros.push("You are more experienced.");
else if (alvl+5<vlvl)
	cons.push(he+" is more experienced.");
var heavierLighter = "about the same weight";
var tallerShorter = "about as tall";
var stocky = "about as stocky as";
if (awt-10 > vwt)
	heavierLighter = "lighter";
else if (vwt-10 > awt)
	heavierLighter = "heavier";
if (aht-2 > vht)
	tallerShorter = "shorter";
else if (vht-2 > aht)
	tallerShorter = "taller";
if (astr+acon-adex-2>vstr+vcon+vdex)
	stocky = "less stocky than";
else if (astr+acon-adex+2<vstr+vcon+vdex)
	stocky = "stockier than";
var vRace = raceNumToText(vict.race);
var aAn = "a";
if (vRace == "ogier") {
	aAn += "n";
	//vScore += 20;
}
else if (vict.level > 99) {
	aAn += "n";
	vRace = "immortal";
	vScore += 100000;
	cons.push(he+" cannot die.");
}
if (vict.class == constants.CLASS_DREADLORD) {
	vScore += 40;
}
else if (vict.class == constants.CLASS_MYRDDRAAL || vict.class == constants.CLASS_OGIER || vict.class == constants.CLASS_BLADEMASTER || vict.class == constants.CLASS_GREYMAN || vict.class == constants.CLASS_FADE)
	vScore += 20;
else if (vict.class == constants.CLASS_DREADGUARD || vict.class == constants.CLASS_CHANNELER)
	vScore += 30;
if (actor.class == constants.CLASS_OGIER || actor.class == constants.CLASS_BLADEMASTER || actor.class == constants.CLASS_FADE || actor.class == constants.CLASS_GREYMAN)
	aScore += 20;
else if (actor.class == constants.CLASS_DREADGUARD || actor.class == constants.CLASS_CHANNELER)
	aScore += 30;
else if (actor.class == constants.CLASS_DREADLORD)
	aScore += 50;
if (actor.legend > 0 && actor.legend < 9) {
	pros.push("You are legendary.");
	aScore += 20;
}
if (vict.legend > 0 && vict.legend < 9) {
	cons.push(he+" is legendary.");
	vScore += 20;
}
aScore += awpnDmg;
vScore += vwpnDmg;
getCharCols(actor);
var raceMsg = "";
if (vict.vnum == -1)
	raceMsg = ", "+aAn+" "+vRace;
act("$N considers you.", true, vict, null, actor, constants.TO_CHAR);
actor.send("You consider "+nrm+vict.name+nrm+raceMsg+".");
if (vict.vnum == -1)
	actor.send(capFirstLetter(vict.he_she())+"'s "+heavierLighter+", "+tallerShorter+", and "+stocky+" you.");

	
actor.send(grn+"\nPROS"+nrm+"                                "+red+"CONS"+nrm);
if (pros.length == 0 && cons.length == 0) {
	pros.push("An even match!");
	cons.push("An even match!");
}
/** if (pros.length == 0)
	pros.push("None");
if (cons.length == 0)
	cons.push("None"); **/
for (i=0;i<10;i++) {
	var buffer = "";
	if (pros[i]) {
		buffer += strPadding(pros[i]," ",37,"right");
	}
	else
		buffer += "                                    ";
	if (cons[i]) {
		buffer += cons[i];
	}
	if (buffer.length > 36 || pros.length > i)
		actor.send(buffer);
}
var msg = bld+"A perfect match!";
if (aScore-vScore > 150)
	msg = bld+"You could beat "+vict.name+" in your sleep!";
else if (aScore-vScore > 120)
	msg = bld+capFirstLetter(vict.name)+" is no match for you.";
else if (aScore-vScore > 90)
	msg = bld+capFirstLetter(vict.name)+" will need some luck against you!";
else if (aScore-vScore > 60)
	msg = bld+"You have the upper hand.";
else if (aScore-vScore > 30)
	msg = bld+"You have a slight advantage.";
else if (aScore-vScore > 0)
	msg = bld+"It will be a good match!";
else if (vScore-aScore > 150)
	msg = bld+"You should be running for your life right now!";
else if (vScore-aScore > 120)
	msg = bld+"Be afraid of "+vict.name+"...";
else if (vScore-aScore > 90)
	msg = bld+"You would need a lot of luck!";
else if (vScore-aScore > 60)
	msg = bld+capFirstLetter(vict.name)+" has the upper hand.";
else if (vScore-aScore > 30)
	msg = bld+capFirstLetter(vict.name)+" has a slight advantage.";
else if (vScore-aScore > 0)
	msg = bld+"It will be a good match!";
actor.send("\n"+msg+nrm);
sendKoradin(actor.name+" score: "+aScore);
sendKoradin(vict.name+" score: "+vScore);




};

