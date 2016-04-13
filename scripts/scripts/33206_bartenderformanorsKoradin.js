var script33206 = function(self, actor, here, args, extra) {
	var manor = getManor(actor.room);
	if (manor.ownerUserId != actor.id && !arrContains(manor.allowedUsers, actor.id))
		return;
	if (actor.race == constants.RACE_HUMAN)
		var food = 21305;
	else
		var food = 2109;
	waitpulse 1;
	if (getSval(self,33206,actor.name) > time()) {
		self.comm("tell "+actor.name+" I just gave you some! Come back later.");
		return;
	}
	actor.loadObj(food);
	setSval(self,33206,actor.name,(time()+1200));
	act("$n gives you some food.",true,self,null,actor,constants.TO_VICT);
	act("$n gives $N some food.",true,self,null,actor,constants.TO_NOTVICT);
	
	
}