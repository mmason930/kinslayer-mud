var script1738 = function(self, actor, here, args, extra) {
	const vArgs = getArgList(args);
	if( actor.race !== constants.RACE_HUMAN || vArgs.length < 2 || vArgs[1].toLowerCase() !== "garrison" )
	{
		_noblock;
		return;
	}
	_block;
	if(actor.position !== constants.POS_STANDING){
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
		actor.send("You enter the garrison.");
		actor.moveToRoom(getRoom(1749));
		actor.comm("look");
		here.echo( actor.name + " leaves west." );
		waitpulse 5;
		here.echo( "The Jammed Door closes quietly." );
	}
}