var script20981 = function(self, actor, here, args, extra) { // murash/agelmar enter script
	if(global.eventIsActive && actor.race == self.race){
		waitpulse 1;
		getCharCols(actor);
		actor.send(cyn+bld+self.name+" has a special assignment! Type JOIN EVENT to get started."+nrm);
	}
}
