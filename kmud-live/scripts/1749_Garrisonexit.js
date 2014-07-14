var script1749 = function(self, actor, here, args, extra)
{
	_block;
actor.send("Ok." );
waitpulse 5;
actor.send( "You open the Jammed Door." );
here.echoaround( actor, actor.name + " opens the Jammed Door." );
waitpulse 5;
var room = getRoom(1738);
actor.moveToRoom(room);
actor.send("You exit the garrison.");
actor.comm("look");
here.echo( actor.name + " leaves east." );
room.echoaround( actor, actor.name + " has arrived from the west." );
waitpulse 5;
here.echo( "The Jammed Door closes quietly." );

};

