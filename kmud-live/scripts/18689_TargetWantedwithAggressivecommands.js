var script18689 = function(self, actor, here, args, extra)
{
	//Overload kill command to work with "kill wanted"
//Rhollor Jan 2010
args = getArgList(args.toLowerCase());
if(args[1] == "wanted") {
	var clans = actor.findClans();
	if(!clans.length) {
		//Can't have any wanted if not clanned...
		return 1;
	}
	//Search room for someone wanted by this person
	var people = actor.room.people;
	var target = "null";
	for(var i = 0; i < people.length; i ++) {
		for(var j = 0; j < clans.length; j++) {
			if(people[i].wantedBy(clans[j])) {
				target = people[i];
				break;
			}
		}
		if(target != "null")
			break;
	}
	if(target == "null") {
		return 1;
	}
	else {
		var buf = args[0] + " " + target.namelist.split(" ")[0];
		for(var i = 2; i < (args.length); i++) {
			buf += " " + args[i];
		}
		actor.comm(buf);
	}
}
else {
	return 1;
}
return 0;

};

