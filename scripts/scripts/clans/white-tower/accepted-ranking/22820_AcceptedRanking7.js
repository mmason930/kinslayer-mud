var script22820 = function(self, actor, here, args, extra) {

	_block;

	let targetRoom = getRoom(10575);
	let portalHere = here.loadObj(10600);
	let portalThere = targetRoom.loadObj(10600);
	wait 2;

	here.echo("The portal shimmers a few spans away.");
	wait 3;

	here.echo("The portal fades a bit, but then glows bright again.");
	wait 2;

	here.echo("A soft voice floats through your mind. 'The way back comes but once...'");
	wait 2;

	here.echo("The portal begins to fade away.");
	wait 1;

	here.echo("The portal disappears, leaving you trapped forever.");
	wait 1;

	portalHere.extract();
	wait 1;

	portalThere.extract();
}