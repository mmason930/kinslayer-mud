var script20837 = function(self, actor, here, args, extra) {
	//Vines from the green stone
	//Koradin - March 2011
	var total_time = 13;
	while (total_time > 0) {
        var room = self.room;
        if (!room)
            return;
        for (var _autoKey in room.items) {
            var item = room.items[_autoKey];
            if (item) {
                if (item.vnum == 9025)
                    item.extract();
                else if (item.vnum == 9015 && item != self)
                    item.extract();
            }
        }
        var rooms = [];
        if (self.count < 6) {
            for (var _autoKey in room.neighbors) {
                var rm = room.neighbors[_autoKey];
                if (rm) {
                    if ( Math.floor(rm.vnum / 100) == Math.floor(room.vnum / 100) || random(1,100) == 1 ) {
                        var spread_chance = random(1,100);
                        
                        var char_exists = null;
                        for (var _autoKey in rm.items) {
                            var im = rm.items[_autoKey];
                            if (im.vnum == self.vnum)
                                spread_chance += 100;
                            if (im.vnum == 9025) {
                                spread_chance += 3;
                                char_exists = im;
                            }
                        }
                        rooms.push([rm,spread_chance,char_exists]);
                    }
                }
            }
            for (var _autoKey in rooms) {
                var new_room = rooms[_autoKey];
                if (new_room[1] < 5) {
                    var from_dir = dirToText(revDir(room.firstStep(new_room[0])));
                    if (from_dir == "up")
                        var from = "above";
                    else if (from_dir == "down")
                        var from = "below";
                    else
                        var from = "the " + from_dir;
                    if (room.doorIsClosed(room.firstStep(new_room[0])) && room.doorHidden(room.firstStep(new_room[0])) < 1)
                        here.echo("The thick forest of vines writhe, covering the " + room.doorName(room.firstStep(new_room[0])) + "!");
                    else if (room.doorHidden(room.firstStep(new_room[0])) > 0) {
                        here.echo("The writhing forest of vines shivers as if from some unfelt breeze...");
                    }
                    else {
                        here.echo("The deadly vines creep along the ground, spreading "+dirToText(room.firstStep(new_room[0]))+"!");
                        new_room[0].echo("A deadly forest of vines creeps in from "+from+"!");
                        new_room[0].loadObj(9015);
                        if (new_room[2] != null)
                            new_room[2].extract();
                    }
                    //here.echo("Chance to spread "+dirToText(room.firstStep(new_room[0]))+" is " + new_room[1]);
                }
            }
        }
        if (random(1,100) <= 3) {
            for (var _autoKey in room.people) {
                var person = room.people[_autoKey];
                if (getSval(person, 20838, "vines") == 2)
                    return;
            }
            room.echo("The vines wither away as fast as they grew, leaving dead vegetation everywhere.");
            room.loadObj(9025);
            self.extract();
            return;
        }
        wait 4;
        total_time -= 4;
	}
	
}