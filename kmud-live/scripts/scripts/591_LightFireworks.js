var script591 = function(self, actor, here, args, extra) {
	var arg_list = args.split(" ");
	if( str_cmp(arg_list[1], "fireworks") )
	    return 1;
	actor.send("You light the fuse of " + self.name + ".");
	here.echoaround(actor, actor.name + " lights the fuse of " + self.name + ".");
	actor.loadObj(591);
	self.extract();
	return 0;
}