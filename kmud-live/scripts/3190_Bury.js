var script3190 = function(self, actor, here, args, extra)
{
	if( actor.name != "Galnor" && actor.name != "Alder" ) {
	return;
}
_block;
var vArgs = getArgList( args );
if( vArgs.length < 2 ) {
	actor.send("Bury what?");
	return;
}
var objectTargetString = vArgs[ 1 ];
var objectToBury = getObjInListVis( actor, objectTargetString, actor.inventory );
if( !objectToBury ) {
	actor.send("You don't seem to be carrying that.");
	return;
}
actor.send("You bury " + objectToBury.name + ".");
addDigItem( objectToBury, here );

};

