var script20848 = function(self, actor, here, args, extra) {
	//Smithing paging script
	//Koradin - May 2010
	_block;
	getCharCols(actor);
	var sval = getSval(actor,20855,"paging");
	var obj_type = getSval(actor,20855,"obj type");
	var obj_mat = getSval(actor,20855,"obj mat");
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"cancel",1))
	{
		actor.send("You stop tailoring.");
		setSval(actor,20855,"paging",0);
		setSval(actor,20855,"obj type",null);
		setSval(actor,20855,"attribute",0);
		actor.detach(20848);
		return;
	}
	if (sval == 11) // COMBO
	{
		if (strn_cmp(vArgs[0],"heavier",1))
		{
			setSval(actor,20855,"attribute",1);
			actor.detach(20848);
			actor.comm("tailor "+obj_mat+" "+obj_type);
			return;
		}
		else if (strn_cmp(vArgs[0],"lighter",1))
		{
			setSval(actor,20855,"attribute",2);
			actor.detach(20848);
			actor.comm("tailor "+obj_mat+" "+obj_type);
			return;
		}
		else
		{
			actor.send("That is not a valid command!");
	        actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop tailoring.");
	        actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
			actor.send(" ");
			getMenu(actor,11,"tailoring");
			return;
		}
	}
	else {
		actor.send("That is not a valid command!");
	    actor.send("Choose another command, or type "+cyn+"C"+nrm+" to stop tailoring.");
	    actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
		return;
	}
	
	
}