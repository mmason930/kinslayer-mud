var script20853 = function(self, actor, here, args, extra) {
	//Disperse and recall herbalism scripts on zone reset
	//Koradin
	/**
	self.echo("HERBALISM")
	self.echo(fread("Herbalism"));
	if (fread("Herbalism"))
		var rooms = fread("Herbalism").split("~");
	for (var _autoKey in rooms) {
		var room = rooms[_autoKey];
		if (getRoom(room))
			getRoom(room).detach(20951);
	}
	fwrite("Herbalism","");
	self.echo("MINING")
	self.echo(fread("Mining"));
	if (fread("Mining"))
		var rooms2 = fread("Mining").split("~");
	for (var _autoKey in rooms2) {
		var room2 = rooms2[_autoKey];
		if (getRoom(room2))
			getRoom(room2).detach(20952);
	}
	fwrite("Mining",""); **/
	var new_rooms = [];
	var new_rooms2 = [];
	var field = constants.SECT_FIELD;
	var forest = constants.SECT_FOREST;
	var hills = constants.SECT_HILLS;
	var mount = constants.SECT_MOUNTAIN;
	var road = constants.SECT_ROAD;
	var cur_room = null;
	for (i = 0; i < 150; i++)
	{
		cur_room = getRandomRoom();
		if (cur_room.countJS(20951) < 1)
		{
			if (cur_room.sector == road || cur_room.sector == field || cur_room.sector == forest || cur_room.sector == hills || cur_room.sector == mount)
			{
				cur_room.attach(20951);
				new_rooms.push(cur_room.vnum);
				self.echo("herbalism attaching to " +cur_room.name+ " " + cur_room.vnum);
			}
		}
		else if (cur_room.countJS(20952) < 1)
		{
			if (cur_room.sector == road || cur_room.sector == field || cur_room.sector == forest || cur_room.sector == hills || cur_room.sector == mount)
			{
				cur_room.attach(20952);
				new_rooms2.push(cur_room.vnum);
				self.echo("mining attaching to " +cur_room.name+ " " + cur_room.vnum);
			}
		}
		else
			i--;
	}
	/**
	fwrite("Herbalism",new_rooms.join("~"));
	fwrite("Mining",new_rooms2.join("~")); **/
	
	
}