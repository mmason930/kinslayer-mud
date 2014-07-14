var script10652 = function(self, actor, here, args, extra)
{
	//Jenkin 
//October 2010
//Quest: Eternal Servitude
var qName = 'Eternal Servitude';
var script = getSval(actor, 10659, "script_attached");
if ( !script && actor.questInProgress(qName) ) {
     setSval(actor, 10659, "script_attached", 1);
     actor.attach(10660);
   }

};

