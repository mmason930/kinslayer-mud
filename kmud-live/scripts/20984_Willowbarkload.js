var script20984 = function(self, actor, here, args, extra)
{
	waitpulse 12;
here.echo("You start shaving bark off the willow tree.");
here.loadObj(20983);
here.items[0].moveToChar(actor);
waitpulse 48;
here.echo("You shave one last piece of bark off the tree since your other hand is full.");

};

