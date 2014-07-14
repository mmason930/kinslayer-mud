var script20850 = function(self, actor, here, args, extra)
{
	//MINE global command for MINING
//Koradin - converted from DG April 2010
_block;
if (getSval(actor,20850,"mining") == 1)
{
	actor.send("You're already mining!");
	return;
}
var skill = actor.getSkill(getSkillVnum("Mining"));
if (skill == 0)
{
	actor.send("You know nothing about mining...");
	return;
}
if (actor.level < 5)
{
	actor.send("You're too inexperienced to start developing the black lung, kid!");
	return;
}
if (actor.position != constants.POS_STANDING)
{
	actor.send("You're not in the right position to do that right now!");
	return;
}
function getPick( actor )
{
	var wield_item = actor.eq(constants.WEAR_HOLD);
	if (wield_item)
	{
		for each(var word in wield_item.namelist.split(" "))
		{
			if (word == "pickformining")
				return wield_item;
		}
	}
	return null;
}
var pick = getPick( actor );
if (!pick)
{
	actor.send("You need to hold a pick in order to start mining!");
	return;
}
var room = actor.room;
var room_count = 0;
for each (var person in room.people)
{
	if (getPick( person ) && person != actor)
		room_count += 1;
}
if (room_count > 3)
{
	actor.send("It's too crowded here! Pick another spot!");
	return;
}
var hit_chance = 1000;
var skill_roll = 600;
var scaling_skill_roll = 15 - (skill / 10);
for (i = 0; i < skill; i++)
	skill_roll -= 15;
if (skill_roll < 0)
	skill_roll = 0;
if (pick.vnum == 1509)
	hit_chance = 1000;
else if (pick.vnum == 1510)
	hit_chance -= 100;
else if (pick.vnum == 1512 && skill > 39)
	hit_chance -= 200;
else if (pick.vnum == 1515 && skill > 74)
	hit_chance -= 500;
if (skill > 69)
{
	var bonus_sub = ((skill - 69) * 4);
	hit_chance -= bonus_sub;
}
if (!room.isFlagged(constants.ROOM_MINING))
{
	var scatterMine = getSval(actor,room.vnum,"scatterMine");
	if ((!scatterMine || scatterMine == 0) || room.countJS(20952) == 0) {
		actor.send("You can't find any rocks to start picking apart here...");
		setSval(actor,room.vnum,"scatterMine",0);
		return;
	}
	else if (getSval(room,20850,"isMining") == 1) {
		actor.send("Someone else is already digging here!");
		return;
	}
	else {
		if (pick.vnum != 1515)
			hit_chance -= 530;
		else
			hit_chance -= 330;
		if (hit_chance < skill_roll)
			hit_chance = skill_roll;
		//here.echo(hit_chance);
	}
	
}
var diamond = [4569,null,1,"glittering",cyn];
var artifact_1 = [4584,4567,4,"glowing",cyn];
var artifact_2 = [4568,4562,7,"glowing",cyn];
var artifact_3 = [4525,null,10,"glowing",cyn];
var obsidian = [4585,4603,17,"dark and shining"];
var platinum = [4589,4594,27,"polished"];
var gold = [4591,4592,40,"golden"];
var silver = [4590,4593,60,"bright"];
var stone_1 = [20835,null,64,"strange",blu];
var stone_2 = [20836,null,68,"strange",red];
var stone_3 = [20837,null,72,"strange",grn];
var iron = [4598,4599,146,"dark"];
var copper = [4588,4595,226,"orange"];
var aluminum = [4586,4597,326,"metallic"];
var tin = [4587,4596,556,"metallic"];
var coal = [4583,null,700,"grey"];
var items = [diamond,artifact_1,artifact_2,artifact_3,obsidian,platinum,gold,silver,stone_1,stone_2,stone_3,iron,copper,aluminum,tin,coal];
var miner = [6110,null,1,"emote blinks his eyes, adjusting to the light."];
var scorpion = [4204,null,2,"kill " + actor.name];
var beetle = [20840,null,3,"kill " + actor.name];
//var thief = [20841,null,4,"emote cackles and says, 'Thanks for the donation!'"];
var animals = [miner,scorpion,beetle/*,thief*/];
var total_items = [diamond,artifact_1,artifact_2,artifact_3,obsidian,platinum,gold,silver,stone_1,stone_2,stone_3,iron,copper,aluminum,tin,coal,miner,scorpion,beetle/*,thief*/];
act("\nYou start swinging your pick, breaking off tiny pieces of the rocky wall...",true,actor,null,null,constants.TO_CHAR);
actor.send("You start mining.");
//here.echo(scatterMine + " " + room.countJS(20952));
act("$n starts mining, swinging $s pick at a rocky wall...",true,actor,null,null,constants.TO_ROOM);
if (scatterMine == 1 && room.countJS(20952) > 0) {
	setSval(room,20850,"isMining",1);
	actor.startTimer(7);
	var success = runTimer(actor);
	if (!success || !getPick(actor)) {
		actor.send("You stop mining.");
		act("$n stops mining.",true,actor,null,null,constants.TO_ROOM);
		setSval(room,20850,"isMining",0);
		return;
	}
	var miningRoll = random(skill_roll,hit_chance);
	for (j=0; j < items.length; j++) {
		if (miningRoll < items[j][2]) {
			if (items[j][1] != null)
				var resultVnum = items[j][random(0,1)];
			else
				var resultVnum = items[j][0];
			if (items[j][4])
				var sColor = items[j][4];
			var adjective = items[j][3];
			break;
		}
	}
	getCharCols(actor);
	if (sColor)
		var sName = sColor+getObjProto(resultVnum).name+nrm;
	else
		var sName = getObjProto(resultVnum).name;
	actor.send("\nYou unearthed "+sName+"!");
	act("$n uncovers something "+adjective+".",true,actor,null,null,constants.TO_ROOM);
	room.loadObj(resultVnum);
	setSval(room,20850,"isMining",0);
	if (random(1,100) < 6 && skill < 99) {
		awardPracs(actor,"Mining",null,"randomMining");
	}
	for each (var player in getConnectedPlayers())
	{
		if (player.name == "Koradin")
			player.send("SCATTER MINING: " +actor.name + " just got " + sName);
	}
	var randomStop = random(1,10);
	//here.echo(randomStop);
	if (randomStop < 4) {
		setSval(actor,room.vnum,"scatterMine",0);
		room.detach(20952);
	}
	return;
}
setSval(actor,20850,"mining",1);
var total_time = 10000;
while (total_time > 0)
{
	wait 7;
	if (actor.room != room || actor.position != constants.POS_STANDING || !getPick(actor))
	{
		actor.send("You stop mining.");
		act("$n stops mining.",true,actor,null,null,constants.TO_ROOM);
		setSval(actor,20850,"mining",0);
		return;
	}
	var mining_roll = random(skill_roll,hit_chance);
	var animal_roll = random(0,75);
	var result = null;
	var animal_check = false;
	if (animal_roll < scaling_skill_roll && scatterMine != 1)
	{
		result = animals[random(0,animals.length - 1)];
		animal_check = true;
	}
	else
	{
		for (i = 0; i < items.length; i++)
		{
			if ( mining_roll < items[i][2])
			{
				result = items[i];
				break;
			}
		}
	}
	if (result != null && random(0,scaling_skill_roll) == 1)
	{
		var result_vnum = result[0];
		for each (var player in getConnectedPlayers())
		{
			if (player.name == "Koradin")
				player.send("MINING: " +actor.name + " just got " + getObjProto(result_vnum).name);
		}
		if (animal_check == true)
		{
			var name = getMobProto(result[0]).name;
			var end_msg = "Suddenly " + name + " crashes through the wall!";
			var end_msg_room = "$n stops mining and backs away from the wall, pointing at something in the rocks...";
			var end_msg_actor = "You stop mining and back away when you see the wall in front of you is moving!";
		}
		else
		{
			if (random(1,scaling_skill_roll) < 3 && result[1] != null)
				result_vnum = result[1];
			if (result[4]){
				getCharCols(actor);
				var name = result[4]+getObjProto(result_vnum).name+nrm;
			}
			else
				var name = getObjProto(result_vnum).name;
			var end_msg = "Something " + result[3] + " breaks loose and tumbles down the side of the wall.";
			var end_msg_actor = "Your pick hits something that sounds a bit different, so you furiously dig in that spot!";
			var end_msg_room = "$n starts digging at the wall furiously, bits of rock and stone flying everywhere.";
		}
		//here.echo(mining_roll);
		act(end_msg_actor,true,actor,null,null,constants.TO_CHAR);
		act(end_msg_room,true,actor,null,null,constants.TO_ROOM);
		actor.mvs -= random(5,20);
		wait 7;
		if (animal_check == true)
		{
			setSval(actor,20850,"mining",0);
			room.echo(end_msg);
			room.loadMob(result[0]);
			var mobby = getMobAtRoom(room.vnum,result[0]);
			mobby.comm(result[3]);
			if (result[2] == 1)
			{
				wait 2;
				mobby.say("I'm claiming this spot! You can't dig here anymore!");
				mobby.comm("kill " + actor.name);
			}
			else if (result[2] == 4)
			{
				var time = 5;
				while (time > 0)
				{
					waitpulse 2;
					mobby.comm(dirToText(random(0,5)));
					time -= 1;
				}
			}
		}
		if (actor.room != room || actor.position != constants.POS_STANDING || !getPick(actor))
		{
			actor.send("You stop mining.");
			act("$n stops mining.",true,actor,null,null,constants.TO_ROOM);
			setSval(actor,20850,"mining",0);
			return;
		}
		if (animal_check == false)
		{
			getCharCols(actor);
			room.echo(end_msg);
			room.loadObj(result_vnum);
			actor.send("You found " + name + nrm + "!");
			if (random(1,5) ==1 && actor.level < 50)
			{
				actor.send("You receive some experience points!");
				actor.experience += random(1,1000000);
			}
			//here.echo(skill_roll);
			if (random(1,2) > 0)
			{
				var material = getObjProto(result_vnum).name.split(" ");
				var matl = material.splice(3,1);
				if (matl.toString() == "impure")
					matl = "coal";
				else
					matl = matl.toString();
					//here.echo(matl);
				awardPracs(actor,"Mining",matl,null);
			}
		}
		setSval(actor,20850,"mining",0);
		return;
	}
	if (actor.mvs < 35)
	{
		actor.send("You try to keep swinging your pick, but are too exhausted!");
		actor.send("You stop mining.");
		act("$n lowers $s pick, taking a break from mining.",true,actor,null,null,constants.TO_ROOM);
		setSval(actor,20850,"mining",0);
		return;
	}
	var message_roll = random(1,3);
	if (message_roll == 1)
		var message = "You continue to chip away at the rocky wall, hoping to stumble across something.";
	else if (message_roll == 2)
		var message = "Sweat forms on your brow as you swing away mindlessly.";
	else if (message_roll == 3)
		var message = "Your muscles ache and you groan as you repeatedly pick out a pattern in the wall.";
	actor.send(message);
	if (random(1,5) == 1) {
		if (actor.level < 50) {
			actor.send("You receive some experience points!");
			actor.experience += random(1,1000000);
		}
	}
	var randAward = Math.floor((100 - skill)/10);
	if (randAward < 1)
		randAward = 1;
	if (random(1,100) == randAward && skill < 99) {
		awardPracs(actor,"Mining",null,"randomMining");
	}
	act("$n continues to chip away at the rocky wall...",true,actor,null,null,constants.TO_ROOM);
	if (actor.race == constants.RACE_HUMAN)
		var mv_dmg = random(3,6);
	else
		var mv_dmg = random(8,12);
	actor.mvs -= mv_dmg;
	total_time -= 1;
}





};

