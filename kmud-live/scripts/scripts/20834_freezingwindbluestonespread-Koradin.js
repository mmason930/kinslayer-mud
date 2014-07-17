var script20834 = function(self, actor, here, args, extra) {
	//Icy wind from the blue stone
	//Koradin - March 2011
	var total_time = 13;
	while (total_time > 0)
	{
	var room = self.room;
	if (!room)
	return;
	var items = [];
	for each (var item in room.items)
	{
	if (item.vnum == 9026 && item != self)
	item.extract();
	else if (item.vnum == 9027)
	item.extract();
	else if (item != self && item.type != constants.ITEM_CONTAINER && item.vnum != 9024 && random(1,15) == 1 && item.canWear(constants.ITEM_WEAR_TAKE) && room.vnum != 19304 && room.vnum != 2032)
	items.push(item);
	}
	var chars = [];
	var itemHit = [];
	for each (var person in room.people)
	{
	if (!person.mobFlagged(constants.MOB_INVIS) && person.level < 100 && random(1,24) == 1)
	chars.push(person);
	if (!person.mobFlagged(constants.MOB_INVIS) && person.level < 100)
	itemHit.push(person);
	}
	for each(var it in items)
	{
	var target = itemHit[random(0,itemHit.length-1)];
	if (target)
	{
	if (random(0,items.length) == 1)
	{
	act("$p is picked up by the wind and tossed through the air, hitting $N!",true,null,it,target,constants.TO_NOTVICT);
	act("$p is picked up by the wind and tossed through the air, hitting you with full force!",true,null,it,target,constants.TO_VICT);
	target.send("Ouch!");
	it.moveToChar(target);
	var dmg = Math.ceil(it.weight + 3);
	if (dmg > 20)
	dmg = 20;
	target.damage(dmg);
	}
	}
	}
	for each(var per in chars)
	{
	per.send("You shiver as the icy wind slices through your clothing.");
	act("$n shivers as the wind picks up.",true,per,null,null,constants.TO_ROOM);
	if (!per.affectedBy(constants.AFF_CHILL))
	per.affect(constants.AFF_CHILL,2,0);
	if (!per.affectedBy(constants.AFF_WEAKEN))
	per.affect(constants.AFF_WEAKEN,2,0);
	}
	var rooms = [];
	for each (var rm in room.neighbors)
	{
	if (rm)
	{
	if ( Math.floor(rm.vnum / 100) == Math.floor(room.vnum / 100) || random(1,100) == 1 )
	{
	var spread_chance = random(1,100);
	var char_exists = null;
	for each(var im in rm.items)
	{
	if (im.vnum == self.vnum)
	spread_chance += 100;
	if (im.vnum == 9027)
	{
	spread_chance += 3;
	char_exists = im;
	}
	}
	rooms.push([rm,spread_chance,char_exists]);
	}
	}
	}
	for each (var new_room in rooms)
	{
	if (new_room[1] < 5)
	{
	var from_dir = dirToText(revDir(room.firstStep(new_room[0])));
	if (from_dir == "up")
	var from = "above";
	else if (from_dir == "down")
	var from = "below";
	else
	var from = "the " + from_dir;
	if (room.doorIsClosed(room.firstStep(new_room[0])) && room.doorHidden(room.firstStep(new_room[0])) < 1)
	here.echo("The icy wind rattles the " + room.doorName(room.firstStep(new_room[0])) + "!");
	else if (room.doorHidden(room.firstStep(new_room[0])) > 0)
	{
	here.echo("The wind picks up, whipping about in an icy frenzy...");
	}
	else
	{
	here.echo("The freezing wind whistles through the area, heading "+dirToText(room.firstStep(new_room[0]))+"!");
	new_room[0].echo("An icy, freezing wind blows in from "+from+"!");
	new_room[0].loadObj(9026);
	if (new_room[2] != null)
	new_room[2].extract();
	}
	//here.echo("Chance to spread "+dirToText(room.firstStep(new_room[0]))+" is " + new_room[1]);
	}
	}
	if (random(1,50) == 1)
	{
	room.echo("The freezing wind suddenly dies away, leaving an eerie calm behind.");
	room.loadObj(9027);
	self.extract();
	return;
	}
	wait 4;
	total_time -= 4;
	}
}