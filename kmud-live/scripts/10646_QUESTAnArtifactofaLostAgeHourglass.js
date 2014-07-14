var script10646 = function(self, actor, here, args, extra)
{
	//Jenkin 
//September 2010
//Quest: An Artifact of a Lost Age
var qName = 'An Artifact of a Lost Age';
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp ) {
     _block;
     actor.send(bld+cyn+"As you tip the hourglass, grains of sand trickle downward."+nrm);
     waitpulse 10;
     actor.send(bld+cyn+"The room bends and flickers..."+nrm); 
     actor.moveToRoom(getRoom(20881));
     setSval(actor, 10646, "hourglass", 1);
}

};

