var script20852 = function(self, actor, here, args, extra)
{
	//GATHER command for herbalism
//Koradin - Converted from DG May 2010
_block;
var vArgs = getArgList(args);
var skill = actor.getSkill(getSkillVnum("Herbalism"));
var skill_level = Math.floor(skill/10);
var skill_roll = random(1,100);
var herb = getHerb(actor);
getCharCols(actor);
if (!vArgs[1] || vArgs[2])
{
	actor.send("Gather what?");
	return;
}
else if (str_cmp(vArgs[1],"plant") && str_cmp(vArgs[1],"herb"))
{
	actor.send("Gather what?");
	return;
}
if (actor.room.countJS(20951) < 1 || getSval(actor,actor.room.vnum,"herbalism") != 1)
{
	actor.send("You don't see any "+vArgs[1].toLowerCase()+"s to gather here.");
	return;
}
if (actor.position != constants.POS_STANDING)
{
	actor.send("You're not in the position to do that right now!");
	return;
}
actor.room.detach(20951);
act("You kneel down and start carefully uprooting the plant...",true,actor,null,null,constants.TO_CHAR);
act("$n kneels down and starts pulling a plant out of the ground.",true,actor,null,null,constants.TO_ROOM);
var timer = (10 - skill_level);
actor.startTimer(timer);
var success = runTimer(actor);
if (!success)
{
	actor.send("You give up gathering the plant.");
	act("$n gives up the gathering attempt.",true,actor,null,null,constants.TO_ROOM);
	actor.room.attach(20951);
	return;
}
if (skill_roll > skill)
{
	actor.send("Your clumsy work mangles the plant, leaving it unidentifiable!");
	act("$n shakes $s head and sighs, standing back up.", true,actor,null,null,constants.TO_ROOM);
	actor.loadObj(20969);
	setSval(actor,actor.room.vnum,"herbalism",0);
	awardPracs(actor,"Herbalism",null,20969);
	return;
}
else
{
	actor.send("The plant comes free of the soil, and you gather it gently.");
	act("$n finishes carefully gathering a plant.",true,actor,null,null,constants.TO_ROOM);
	actor.loadObj(herb[0]);
	if (random(1,3) == 1)
			actor.loadObj(herb[0]);
	if (random(1,10) == 1)
			actor.loadObj(herb[0]);
	actor.send("You gathered "+herb[1]+"!");
	setSval(actor,actor.room.vnum,"herbalism",0);
	awardPracs(actor,"Herbalism",null,herb[0]);
	return;
}



};

