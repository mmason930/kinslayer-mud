var script9806 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList( args );
if( vArgs.length < 2 ) {
	return;
}
var eq = [];
for(var i = 0;i < constants.NUM_WEARS;++i)
{
	if( actor.eq( i ) )
		eq.push( actor.eq(i) );
}
var targetObj = getObjInListVis( actor, vArgs[1], [eq,actor.inventory,here.items] );
if( !targetObj || targetObj.name != "a rusty chain saw" )
	return;
_block;
var status = getSval(targetObj, 9804, "onoff");
if( status == "off" ) {
	actor.send("It appears to be off already.");
	return;
}
actor.send("You turn off " + targetObj.name);
here.echo( capFirstLetter(targetObj.name) + " dwindles down to a halt." );
setSval(targetObj, 9804, "onoff", "off");

};

