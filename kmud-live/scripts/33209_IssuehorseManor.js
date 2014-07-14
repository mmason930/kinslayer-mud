var script33209 = function(self, actor, here, args, extra)
{
	if (actor.id != getManor(actor.room).ownerUserId && !arrContains(getManor(actor.room).allowedUsers, actor.id)) {
	waitpulse 1;
	self.comm("say I don't work for you, "+actor.name+"!");
	return;
}
if (actor.race == constants.RACE_HUMAN)
	var horse = 201;
else if (actor.class == constants.CLASS_FADE || actor.class == constants.CLASS_DREADLORD)
	var horse = 9998;
else {
	waitpulse 1;
	self.comm("say What do you need a mount for?");
	return;
}
waitpulse 1;
if (getSval(self,33209,actor.name) > time()) {
	self.comm("tell "+actor.name+" I just gave you one! Come back later.");
	return;
}
setSval(self,33209,actor.name,(time()+1800));
act("$n leads a horse out to you.",true,self,null,actor,constants.TO_VICT);
act("$n leads a horse out to $N.",true,self,null,actor,constants.TO_NOTVICT);
actor.room.loadMob(horse);
setSval(actor.room.people[0],33209,"horse",(time()+1800));
actor.room.people[0].comm("follow "+actor.name);
actor.comm("group all");



};

