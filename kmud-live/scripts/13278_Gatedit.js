var script13278 = function(self, actor, here, args, extra)
{
	if( actor.getOLC() ) {
	return;
}
if( actor.level < 102 ) {
	actor.send("You can't use that!\r\n");
	return;
}
_block;
actor.setupOLC( "gatedit" );

};

