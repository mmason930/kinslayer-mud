var script10378 = function(self, actor, here, args, extra)
{
	_block;
actor.send("Ok." );
waitpulse 5;
actor.send( "You open the Jammed Door." );
here.echoaround( actor, actor.name + " opens the Jammed Door." );
waitpulse 5;
var room = getRoom(10377);
actor.moveToRoom(room);
actor.send("You exit the garrison.");
actor.comm("look");
here.echo( actor.name + " leaves south." );
room.echoaround( actor, actor.name + " has arrived from the north." );
waitpulse 5;
here.echo( "The Jammed Door closes quietly." );

};

