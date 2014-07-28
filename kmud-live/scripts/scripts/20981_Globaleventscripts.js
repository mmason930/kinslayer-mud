var script20981 = function(self, actor, here, args, extra) { // murash/agelmar enter script
	//sendKoradin("working1");
	if(global.eventIsActive){
		//sendKoradin("working");
		waitpulse 1;
		getCharCols(actor);
		actor.send(cyn+bld+"Welcome to the global Kinslayer event! Type JOIN EVENT to get started."+nrm);
	}
}
