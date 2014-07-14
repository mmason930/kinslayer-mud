var script13280 = function(self, actor, here, args, extra)
{
	// Alder
// Global "achedit" command
if( actor.getOLC() ) {
	return;
}
if( actor.level < 102 ) {
	actor.send("You can't use that!\r\n");
	return;
}
_block;
actor.setupOLC( "achedit" );

};

