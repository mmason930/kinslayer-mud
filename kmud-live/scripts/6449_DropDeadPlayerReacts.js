var script6449 = function(self, actor, here, args, extra)
{
		_block;
	actor.qval("Drop Dead",10);
	getCharCols(actor,constants.CL_OFF);
	actor.send(grn+"You sidestep just in time and plunge the brown-garbed man's dagger into his heart."+nrm);
	echoaround(actor, actor.name+" sidesteps plunges a brown-garbed man's dagger into his heart.");
	actor.room.loadMob(9854);
	var mob = actor.getCharRoomVis("ATTACKER_GUY");
	mob.damage(1000,actor);
	actor.comm("save");
	waitpulse 3;
	actor.send(cyn+bld+"That man looked very familiar..."+nrm);
	actor.detach(6449);

};

