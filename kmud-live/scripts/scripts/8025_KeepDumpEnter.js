var script8025 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( vArgs.length < 2 || actor.race != constants.RACE_TROLLOC || vArgs[1].toLowerCase() != "dump" )
	{
		return;
	}
	_block;
	if(actor.position != constants.POS_STANDING){
	    actor.send("You're not in the correct position.");
	    return;
	}
	if(actor.affectedBy(constants.AFF_NOQUIT)) {
	    actor.send("You must collect yourself first.");
	}
	else {
	    actor.send("Ok.");
	    waitpulse 5;
	    if(actor.mount)
		{
			actor.comm("dismount");
		}
		here.echoaround( actor, actor.name + " opens a door to the west." );
		actor.send( "You open a door to the west." );
		waitpulse 5;
		actor.send("You enter the dump.");
		actor.moveToRoom(getRoom(8083));
		actor.comm("look");
		here.echo( actor.name + " leaves west." );
		waitpulse 5;
		here.echo( "The Charred Door closes quietly." );
	}
	return;
}