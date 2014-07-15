var script10694 = function(self, actor, here, args, extra) {
	var item = extra.obj;
	var args = item.name.split(" ");
	if( actor.level <= 5 && arrContains(args, "scalp") && arrContains(args, "deer") ) {
	    waitpulse 4;
	    self.comm("say Great job " + actor.name + "!");
	    actor.exp(200);
	    getCharCols(actor);
	    actor.send(bld+cyn+"You gain some experience!"+nrm);
	    item.extract();
	}
}