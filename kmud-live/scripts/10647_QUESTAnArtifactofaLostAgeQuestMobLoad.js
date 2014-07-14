var script10647 = function(self, actor, here, args, extra)
{
	///Jenkin 
//October 2010
//Quest: An Artifact of a Lost Age 
var qName = 'An Artifact of a Lost Age';
if ( getSval( actor, 10646, "hourglass" ) == 1 && actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp ) {
   here.loadMob(6969);
}
setSval(actor, 10646, "hourglass", 0);

};

