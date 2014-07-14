var script13253 = function(self, actor, here, args, extra)
{
	var dir = extra.direction;
var dragger = getSval( self, 13252, "draggedBy" );
if( dragger != actor ) {
	return;
}
var dragSuccess = getSval( actor, 13253, "dragSuccess" );
if( dragSuccess == 1 ) {
	setSval( actor, 13253, "dragSuccess", 0 );
	return;//Let the actor leave.
}
_block;//Don't let them leave!
actor.send("You begin dragging the crate to the " + dirToText( dir ) + ".");
actor.startTimer( random(3,5) );
var success = runTimer( actor );
if( success ) {
	setSval(actor, 13253, "dragSuccess", 1);
	actor.comm( dirToText( dir ) );
	setSval(actor, 13253, "dragSuccess", 0);
	self.moveToRoom( actor.room );
	actor.send("You pull " + self.name + " in along with you.");
}

};

