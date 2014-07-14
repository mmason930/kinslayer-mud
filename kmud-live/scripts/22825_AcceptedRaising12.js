var script22825 = function(self, actor, here, args, extra)
{
	var targetRoom = getRoom(10575);
portalHere = here.loadObj(10600);
portalThere = targetRoom.loadObj(10600);
wait 2;
here.echo("The portal shimmers a few spans away.");
wait 3;
here.echo("The portal fades a bit, but then glows bright again.");
wait 2;
here.echo("You look between the portal and Masuri. A few simple weaves, and you could save her life.");
wait 3;
here.echo("A soft voice floats through your mind. 'The way back comes but once...'");
wait 2;
here.echo("The portal begins to fade away.");
wait 2;
here.echo("Tears stream down Masuri's face, begging you for aid.");
wait 4;
here.echo("The portal disappears, leaving you trapped forever.");
wait 1;
portalHere.extract();
wait 1;
portalThere.extract();

};

