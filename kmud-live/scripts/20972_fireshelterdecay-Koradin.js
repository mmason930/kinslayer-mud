var script20972 = function(self, actor, here, args, extra)
{
	// Fire and Shelter decay
// Koradin - March 2010
var human = constants.RACE_HUMAN;
var troll = constants.RACE_TROLLOC;
if (self.room.zoneVnum == global.arenaZoneVnum)
	return;
if (self.vnum == 20971) //human campfire
{
	var race = human;
	var timer = 20;
	var next_item = 991;
	var msg = "The campfire dies away completely, leaving only glowing coals.";
}
else if (self.vnum == 20972) //troll campfire
{
	var race = troll;
	var timer = 20;
	var next_item = 991;
	var msg = "The campfire dies away completely, leaving only glowing coals.";
}
else if (self.vnum == 20974) //human bonfire
{
	var race = human;
	var timer = 30;
	var next_item = 20971;
	var msg = "The bonfire dies down a bit as the larger logs start to crumble...";
}
else if (self.vnum == 20975) //troll bonfire
{
	var race = troll;
	var timer = 30;
	var next_item = 20972
	var msg = "The bonfire dies down a bit as the larger logs start to crumble...";
}
else if (self.vnum == 20977) //human shack
{
	var race = human;
	var timer = 45;
	var next_item = 20976;
	var msg = "The breeze finally wins over as the shack collapses in on itself.";
}
else if (self.vnum == 20978) //troll hut
{
	var race = troll;
	var timer = 45;
	var next_item = 20976;
	var msg = "The breeze finally wins over as the hut collapses in on itself.";
}
else if (self.vnum == 20979) //human tower
{
	var race = human;
	var timer = 120;
	var next_item = 20977;
	var msg = "A support beam snaps, and the top of the tower crashes to the ground, leaving the original shelter behind.";
}
else if (self.vnum == 20980) //troll tower
{
	var race = troll;
	var timer = 120;
	var next_item = 20978;
	var msg = "A support beam snaps, and the top of the tower crashes to the ground, leaving the original shelter behind.";
}
else if (self.vnum == 991)
{
	var timer = 1;
	var msg = "The final coals fade to blackened ash, which quickly blows away with the breeze.";
}
timer *= 60;
wait timer;
act(msg, true, null, self, null, constants.TO_ROOM)
if (next_item)
	self.room.loadObj(next_item);
self.extract();

};

