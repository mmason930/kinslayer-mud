var script13423 = function(self, actor, here, args, extra)
{
	var rand = random(1,5);
if( rand == 1 ) {
waitpulse 6;
    here.echo("A crazy woman rolling around in a veggie patch screams, 'mmmmm carrot soup!'");
}
else if( rand == 2 ) {
waitpulse 6;
    here.echo("A crazy woman rolling around in a veggie patch screams, 'Corn!'");
}
else if( rand == 3 ) {
waitpulse 6;
here.echo("A crazy woman rolling around in a veggie patch screams, 'Potato!'");
}
else if( rand == 4 ) {
waitpulse 6;
here.echo("A crazy woman rolling around in a veggie patch screams, 'Rotten apples feel so good!'");
}
else if( rand == 5 ) {
waitpulse 6;
here.echo("A crazy woman rolling around in a veggie patch screams, 'This is the life!'");
}

};

