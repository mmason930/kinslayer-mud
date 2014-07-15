var script23259 = function(self, actor, here, args, extra) {
	var arg_array = args.split(" ");
	var alias_array = self.namelist.split(" ");
	if ( (arg_array[0] == "drag") && (isName(arg_array[1], self.namelist)) )
	{
	    _block;
	    if ( getSval(self, 23259, "attached") != undefined )            /*Crate is someone else's*/
	        { actor.send("That crate is already being pulled."); }
	    else
	    {
	        if ( getSval(actor, 23259, "attached") != undefined )           /*Actor has a crate already*/
	            { actor.send("You can only pull one crate at a time!"); }
	        else
	        {
	            setSval(self, 23259, "attached", actor.name);       /*Attach crate to actor with actor's name*/
	            setSval(actor, 23259, "attached", 1);               /*Attach actor to crate with 1*/
	            actor.send("You stoop down and begin pulling " + self.name + " across the floor as you move.\n");
	            here.echoaround(actor, actor.name + " stoops down and begins pulling " + self.name + " along the floor.\n");
	        }
	    }
		return;
	}
	else if ( (arg_array[0] == "release") && (isName(arg_array[1], self.namelist)) )
	{
	    if ( (getSval(self, 23259, "attached") == actor.name) )
	    {
			_block;
	        setSval(actor, 23259, "attached", undefined);
	        setSval(self, 23259, "attached", undefined);                 /*Disassociate actor and crate*/
	        actor.send("You stop pulling " + self.name + " and leave it where it is.\n");
	        here.echoaround(actor, actor.name + " releases " + self.name + " and leaves it where it is.\n");
	    }
	    else
	    {
			_block;
			actor.send("How can you let go of something you don't have?\n");
		}
		return;
	}
}