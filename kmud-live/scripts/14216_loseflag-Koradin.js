var script14216 = function(self, actor, here, args, extra)
{
	var actor = self.carriedBy;
if (!actor)
	return;
var vArgs = getArgList(args);
if (vArgs[2]) {
	if (!str_cmp(vArgs[1],"all") || arrContains(self.namelist.split(" "), vArgs[1])) {
		_block;
		actor.send("Drop the flag or give it away!");
		return;
	}
}
if (strn_cmp(vArgs[0],"flee",1)) {
	_block;
	actor.comm("drop ctfflag");
	actor.comm("flee");
	actor.sps = (Math.floor(actor.maxSpell*.6));
	actor.shadowPoints = (Math.floor(actor.maxShadow*.6));
	return;
}
actor.detach(14215);
function afterWait(args) {
	var actor = args[0];
	var self = args[1];
	for each (var person in actor.room.people) {
		if (person.hasItem(self.vnum,true,false,false)) {
			if (person.arenaTeam != actor.arenaTeam) {   //person given to is wrong team, return the flag
				arenaEcho(capFirstLetter(self.name)+" has been returned by "+person.name+".");
				resetFlag(self.vnum);
				return;
			}
			person.attach(14215);
			if (person.eq(constants.WEAR_WIELD))
				person.eq(constants.WEAR_WIELD).moveToChar(actor);
			person.sps = 0;
			person.shadowPoints = 0;
		}
	}
}
setTimeout(1,afterWait,[actor,self]);

};

