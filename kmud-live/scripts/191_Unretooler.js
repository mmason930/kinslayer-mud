var script191 = function(self, actor, here, args, extra)
{
	// Removes the retooling from a piece of equipment
// Written by Fogel
var obj = extra.obj;
if( actor.race != self.race )
{
	_block;
	waitpulse 3;
	self.comm( "say We don't serve your kind here!" );
	return;
}
//	Crafting base items are in the 4900-4999 range, we don't want to unretool those.
if( !obj.isRetooled() )
{
	_block;
	waitpulse 3;
	self.comm( "say That object hasn't been altered, I can't do anything with it" );
	return;
}
else
{
	_noblock;
	waitpulse 1;
	self.comm( "say Thanks, I'll fix that up for you right away!" );
	obj.setRetoolSDesc( "" );
	obj.setRetoolName( "" );
	obj.setRetoolDesc( "" );
	waitpulse 3;
	self.comm( "give " + obj.namelist.split(" ")[0] + " " + actor.name );
}

};

