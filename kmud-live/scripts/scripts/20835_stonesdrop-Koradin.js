var script20835 = function(self, actor, here, args, extra) {
	//stones drop
	//Koradin - April 2010
	var room = actor.room;
	var trigger = false;
	for (var _autoKey in room.items) {
		var thing = room.items[_autoKey];
	if (thing.vnum == 9022 && self.vnum == 20836)
	var event = " raging fire";
	else if (thing.vnum == 9015 && self.vnum == 20837)
	var event = " thick forest of vines";
	else if (thing.vnum == 9026 && self.vnum == 20835)
	var event = "n icy wind blowing";
	if (event)
	{
	actor.send("There's already a" +event+" here!");
	_block;
	return;
	}
	}
	if (self.vnum == 20835)
	{
	var msg1 = "The stone vaporizes before it hits the ground!";
	var msg2 = "The area quickly grows colder...";
	var msg3 = "An unnatural, chilly breeze starts blowing past...";
	var msg4 = "The breeze strengthens into a freezing wind!";
	var end_obj = 9026;
	}
	else if (self.vnum == 20836)
	{
	var msg1 = "The stone immediately bursts into flames!";
	var msg2 = "The flame grows larger, feeding off the surrounding area...";
	var msg3 = "The breeze fans the flame into a flash fire!";
	var msg4 = "The blazing wildfire sweeps through the area, destroying everything in its path!";
	var end_obj = 9022;
	}
	else if (self.vnum == 20837)
	{
	var msg1 = "The stone immediately breaks apart and sinks into the ground!";
	var msg2 = "Suddenly a strange plant starts growing at an alarming rate...";
	var msg3 = "The plant splits, multiplying into a dense forest!";
	var msg4 = "The thick forest of vines writhes about, making travel nearly impossible!";
	var end_obj = 9015;
	}
	waitpulse 1;
	for (var _autoKey in room.items) {
		var item = room.items[_autoKey];
        if (item == self)
            trigger = true;
        }
    }
	if (trigger == false) {
        return;
    }
    room.echo(msg1);
    setTimeout(30, echoMsg, [room,msg2]));
    setTimeout(60, echoMsg, [room,msg3]));
    setTimeout(60, echoAndLoad, [room,msg4,end_obj]));
	self.extract();
    
    function echoMsg(vArgs) {
        var room = vArgs[0];
        room.echo(vArgs[1]);
    }
    
    function echoAndLoad(vArgs) {
        echoMsg(vArgs);
        var room = vArgs[0];
        room.loadObj(vArgs[2]);
    }
}