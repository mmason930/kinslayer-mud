var script1201 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	if(vArgs.length < 2 || str_cmp(vArgs[1],"coin")) {
	    actor.send("Flip what?");
	    return;
	}
}