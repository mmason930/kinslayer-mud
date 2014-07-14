var script10648 = function(self, actor, here, args, extra)
{
	///Jenkin 
//October 2010
//Quest: An Artifact of a Lost Age
var qName = 'An Artifact of a Lost Age';
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp ) {
setSval(actor, 10648, "spider", 0);
}

};

