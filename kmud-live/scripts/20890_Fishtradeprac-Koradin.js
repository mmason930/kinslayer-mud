var script20890 = function(self, actor, here, args, extra)
{
	//FISH trade prac fishing
//Koradin - May 2010
_block;
if (getSval(actor,20850,"fishing") == 1)
{
actor.send("You're already fishing!");
return;
}
var skill = actor.getSkill(getSkillVnum("Fishing"));
if (skill == 0)
{
actor.send("You know nothing about fishing...");
return;
}
if (actor.level < 5)
{
actor.send("You're too inexperienced to start catching the big ones, kid!");
return;
}
if (actor.position != constants.POS_STANDING)
{
actor.send("You're not in the right position to do that right now!");
return;
}
function getRod( actor )
{
var wield_item = actor.eq(constants.WEAR_WIELD);
if (wield_item)
{
for each(var word in wield_item.namelist.split(" "))
{
if (word == "rodforfishing")
return wield_item;
}
}
return null;
}
var rod = getRod( actor );
if (!rod)
{
actor.send("You need to wield a fishing pole in order to cast your line!");
return;
}
var room = actor.room;
if (!room.isFlagged(constants.ROOM_FISHING))
{
actor.send("You can't find anywhere to cast your line here...");
return;
}
var room_count = 0;
for each (var person in room.people)
{
if (getRod( person ) && person != actor)
room_count += 1;
}
if (room_count > 3)
{
actor.send("It's too crowded here! Pick another spot!");
return;
}
var scaling_skill_roll = 15 - (skill / 10);
var hit_chance = 1000;
var skill_roll = 600;
for (i = 0; i < skill; i++)
skill_roll -= 15;
if (skill_roll < 0)
skill_roll = 0;
if (rod.vnum == 21704)
hit_chance = 1000;
else if (rod.vnum == 21705 && skill > 39)
hit_chance -= 100;
else if (rod.vnum == 20891 && skill > 69)
hit_chance -= 200;
if (skill > 69)
{
var bonus_sub = ((skill - 69) * 4);
hit_chance -= bonus_sub;
}
getCharCols(actor);
var trinketRoll = [random(1000,1015),random(900,916),random(700,715),random(800,817)];
trinketRoll = trinketRoll[random(0,3)];
var pleco = [20913,1,"glittering",cyn];
var arowana = [20913,4,"hmm",cyn];
var mackerel = [20911,11,"long, blue-silver"];
var bullhead = [20910,21,"fat-headed"];
var puffer = [20909,46,"spiky"];
var rage = [20908,66,"large spotted"];
var trinket = [trinketRoll,87,"strange",cyn];
var salmon = [20907,127,"pink"];
var trout = [20906,177,"colorful"];
var eel = [20905,237,"slimy"];
var carp = [20904,318,"common-looking"];
var perch = [20903,400,"white"];
var catfish = [20902,500,"whiskered"];
var bass = [20901,600,"small"];
var minnow = [20900,700,"tiny"];
var items = [pleco,arowana,mackerel,bullhead,puffer,rage,trinket,salmon,trout,eel,carp,perch,catfish,bass,minnow];
var fisherman = [6110,1,"emote blinks his eyes, adjusting to the light."];
var scorpion = [4204,2,"kill " + actor.name];
var beetle = [20840,3,"kill " + actor.name];
var animals = [fisherman,scorpion,beetle];
var total_items = [pleco,arowana,mackerel,bullhead,puffer,rage,trinket,salmon,trout,eel,carp,perch,catfish,bass,minnow,fisherman,scorpion,beetle];
actor.startTimer((skill_roll/100));
var success = runTimer(actor);
if (!success || random(0,50) > skill)
{
actor.send("You fumble the rod as you try to cast your line, and end up hooking your own thumb!");
act("$n's clumsy attempt to cast $s line ends with $m hooking $s own thumb!",true,actor,null,null,constants.TO_ROOM);
actor.send("Ouch!");
actor.damage(random(1,3));
return;
}
act("You cast your line into the water and settle in for the big catch...",true,actor,null,null,constants.TO_CHAR);
actor.send("You start fishing.");
act("$n casts $s fishing line far into the water...",true,actor,null,null,constants.TO_ROOM);
setSval(actor,20890,"fishing",1);
var total_time = 10000;
while (total_time > 0)
{
wait 7;
if (actor.room != room || !getRod(actor))
{
actor.send("You stop fishing, and reel your line in.");
act("$n stops fishing, reeling $s line in.",true,actor,null,null,constants.TO_ROOM);
setSval(actor,20890,"fishing",0);
return;
}
var fishing_roll = random(skill_roll,hit_chance);
var animal_roll = random(0,60);
var result = null;
var animal_check = false;
if (animal_roll < scaling_skill_roll)
{
result = animals[random(0,animals.length - 1)];
animal_check = true;
}
else
{
for (i = 0; i < items.length; i++)
{
if ( fishing_roll < items[i][1])
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
player.send("FISHING: " +actor.name + " just got " + result_vnum);
}
if (animal_check == true)
{
var name = getMobProto(result[0]).name;
var end_msg = "Suddenly " + name + " bursts into view, splashing water everywhere!";
var end_msg_room = "$n starts looking at the rippling water with a curious look on $s face...";
var end_msg_actor = "You look puzzled as the water starts rippling, but you feel no tug at the line...";
}
else
{
if (result[3])
var name = result[3]+getObjProto(result_vnum).name+nrm;
else
var name = getObjProto(result_vnum).name;
var end_msg = "A " + result[2] + " fish finally flops out of the water, landing safely on shore.";
var end_msg_actor = "A sharp tug at the line grabs your attention, and you start reeling like mad!";
var end_msg_room = "$n grabs $s pole and starts reeling like crazy!";
}
act(end_msg_actor,true,actor,null,null,constants.TO_CHAR);
act(end_msg_room,true,actor,null,null,constants.TO_ROOM);
actor.mvs -= random(5,20);
wait 7;
if (animal_check == true)
{
setSval(actor,20890,"fishing",0);
room.echo(end_msg);
room.loadMob(result[0]);
var mobby = getMobAtRoom(room.vnum,result[0]);
mobby.comm(result[2]);
if (result[1] == 1)
{
wait 2;
mobby.say("I'm claiming this spot! You can't fish here anymore!");
mobby.comm("kill " + actor.name);
}
}
if (actor.room != room || actor.position != constants.POS_STANDING || !getRod(actor))
{
actor.send("You stop fishing.");
act("$n stops fishing.",true,actor,null,null,constants.TO_ROOM);
setSval(actor,20890,"fishing",0);
return;
}
if (animal_check == false)
{
room.echo(end_msg);
room.loadObj(result_vnum);
actor.send("You caught " + name + "!");
if (random(1,5) ==1 && actor.level < 50)
{
actor.send("You receive some experience points!");
actor.experience += random(1,1000000);
}
//here.echo(skill_roll);
//AWARD PRACS HERE WHEN ITS FINISHED
}
setSval(actor,20890,"fishing",0);
return;
}
//if (actor.mvs < 35)
//{
//actor.send("You try to keep swinging your pick, but are too exhausted!");
//actor.send("You stop mining.");
//act("$n lowers $s pick, taking a break from mining.",true,actor,null,null,constants.TO_ROOM);
//setSval(actor,20850,"mining",0);
//return;
//}
var message_roll = random(1,3);
if (message_roll == 1)
var message = "You kick back and let " + getRod(actor).name + " work its magic.";
else if (message_roll == 2)
var message = "The soft lapping of the water soothes you...";
else if (message_roll == 3)
var message = "You gently wiggle your line back and forth, hoping they'll take the bait.";
actor.send(message);
if (random(1,5) ==1 && actor.level < 50)
{
actor.send("You receive some experience points!");
actor.experience += random(1,1000000);
}
act("$n continues fishing, occasionally wiggling $s pole...",true,actor,null,null,constants.TO_ROOM);
if (actor.race == constants.RACE_HUMAN)
var mv_dmg = random(3,6);
else
var mv_dmg = random(8,12);
actor.mvs -= mv_dmg;
total_time -= 1;
}

};

