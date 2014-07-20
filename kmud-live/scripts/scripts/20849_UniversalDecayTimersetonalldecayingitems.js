var script20849 = function(self, actor, here, args, extra) {
	//decay timer
	//Koradin - May 2010
	
	//Temporarily disabled - June 21, 2014
	return;
	
	var progress = getDecayProgress(self);
	if (progress == null || progress < 0)
	{
		//self.room.echo("trigger");
		setDecayTimer(self,self.decayTimer,self.decayTimerType);
		return;
	}
	if (progress > 99)
	{
		if( self.room )
			self.room.echo(getCurrentDecayMessage(self,true));
		self.deletePval("decay",true);
		self.extract();
		return;
	}
	actor = self.wornBy;
	if (!actor)
		return;
	if (!actor.fighting)
		return;
	var multiplier = 4;
	for (var _autoKey in actor.room.people) {
		var person = actor.room.people[_autoKey];
		if (person.fighting == actor)
			multiplier += 1;
	}
	multiplyDecaySpeed(self, multiplier);
	
	
}