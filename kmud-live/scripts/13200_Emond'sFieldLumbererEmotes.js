var script13200 = function(self, actor, here, args, extra)
{
	var r = random(1,5);
if( r == 1 || r == 2 )
	here.echo( capFirstLetter(self.name) + " wipes the sweat off his forehead.");
else if( r == 3 || r == 4 )
	here.echo( capFirstLetter(self.name) + " swings his axe against the log blocking the Old Road to the south.");
else if( r == 5 )
	here.echo( capFirstLetter(self.name) + " takes a moment to sharpen the dulling blade of his axe.");

};

