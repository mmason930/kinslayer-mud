var script8083 = function(self, actor, here, args, extra) {
	_block;
	actor.send("Ok." );
	waitpulse 5;
	actor.send( "You open the Charred Door." );
	here.echoaround( actor, actor.name + " opens the Charred Door." );
	waitpulse 5;
	var room = getRoom(8025);
	actor.moveToRoom(room);
	actor.send("You exit the dump.");
	actor.comm("look");
	here.echo( actor.name + " leaves east." );
	room.echoaround( actor, actor.name + " has arrived from the west." );
	waitpulse 5;
	here.echo( "The Charred Door closes quietly." );
}