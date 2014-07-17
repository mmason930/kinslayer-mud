var script200 = function(self, actor, here, args, extra) {
	if( actor.level <= 5 && extra.direction == constants.UP ) {
		getCharCols(actor);
		waitpulse 1;
		actor.send(bld + red + "The skill trainers can be found in the building west of here." + nrm);
		waitpulse 1;
		actor.send(bld + red + "The best place to begin exploring is through the south gate. Head north and follow the road east and south." + nrm);
	}
}