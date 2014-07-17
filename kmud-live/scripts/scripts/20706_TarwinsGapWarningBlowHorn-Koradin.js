var script20706 = function(self, actor, here, args, extra) {
	//Tarwin's Gap Warning Horn
	//Koradin - converted from DG May 2010
	var vArgs = getArgList(args);
	_block;
	if (vArgs[2] || !vArgs[1])
	{
		actor.send("Blow what?");
		return;
	}
	if (str_cmp(vArgs[1],"horn"))
	{
		actor.send("Blow what?");
		return;
	}
	if (getSval(actor,20706,"horn") == true)
	{
		actor.send("Catch your breath before you blow again!");
		return;
	}
	setSval(actor,20706,"horn",true);
	actor.send("You put your mouth up to the horn and blow as hard as you can.");
	act("$n puts $s mouth up to the horn and blows with all $s might.",true,actor,null,null,constants.TO_ROOM);
	waitpulse 15;
	gecho("A great horn can be heard in the distance, warning of impending danger at Tarwin's Gap.", function(actor) { return true; } );
	wait 5;
	setSval(actor,20706,"horn",undefined);
}