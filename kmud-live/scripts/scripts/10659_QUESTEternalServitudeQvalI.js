var script10659 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//Quest: Eternal Servitude
	var qName = 'Eternal Servitude';
	var points = getSval(actor, 10659, "soldier_death");
	if( !points ) { 
	   points = 0;
	}
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp && here.vnum == 20624 || here.vnum == 20622 ) {
	   if ( points >= 2) {
	     getCharCols(actor);     
	     actor.send(bld+cyn+"You lay waste to the Shienaran Camp!"+nrm);
	     actor.updateJournalTask(qName,0);
	     setSval(actor, 10659, "soldier_death", 0);
	     setSval(actor, 10659, "script_attached", 1);
	     actor.attach(10660);
	   }
	   else {
	     getCharCols(actor);
	     actor.send(bld+cyn+"Focus. You feel closer to your goal with every kill."+nrm); 
	     points += 1;
	     setSval(actor, 10659, "soldier_death", points);
	   }
	}
}