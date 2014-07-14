var script10649 = function(self, actor, here, args, extra)
{
	///Jenkin 
//October 2010
//Quest: An Artifact of a Lost Age 
_block;
var qName = 'An Artifact of a Lost Age';
var Argsv = getArgList( args );
var vArgs = args.split(" ")
var cmd = vArgs[1];
if ( Argsv.length < 2 ) {
     return;
}
else if ( cmd == "hourglass" && getSval( actor, 10646, "hourglass" ) == 1 && actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp ) {
     actor.send("The hourglass rests in the down position. It looks like it has already been flipped.");
}

};

