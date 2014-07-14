var script10625 = function(self, actor, here, args, extra)
{
	var dir = extra.direction;
var driver = getSval( self, 13252, "drivenBy" );
if( driver != actor ) {
        return;
}
var driveSuccess = getSval( actor, 10625, "driveSuccess" );
if( driveSuccess == 1 ) {
        setSval( actor, 10625, "driveSuccess", 0 );
        return;//Let the actor leave.
}
_block;//Don't let them leave!
actor.send("You begin driving the cart to the " + dirToText( dir ) + ".");
actor.startTimer( random(2,4) );
var success = runTimer( actor );
if( success ) {
        setSval(actor, 10625, "driveSuccess", 1);
        actor.comm( dirToText( dir ) );
        setSval(actor, 10625, "driveSuccess", 0);
        self.moveToRoom( actor.room );
        actor.send("You pull the " + self.name + " behind you.");
}

};

