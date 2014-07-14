var script209 = function(self, actor, here, args, extra)
{
	//	June 2010 - Fogel
//	Unsets svals when a horse is killed or extracted
var owner = getSval( self, 207, "owner" );
if( owner )
	setSval( owner, 207, "horse", 0 );

};

