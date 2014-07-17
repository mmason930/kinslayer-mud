var script10607 = function(self, actor, here, args, extra) {
	if( !getSval(actor, 10607, "deadMobs") ) {
	setSval(actor, 10607, "deadMobs", []);
	}
	var deadMobsArray = getSval(actor, 10607, "deadMobs");
	setSval(actor, 10607, "deadMobs", deadMobsArray)
	deadMobsArray.push( self.vnum );
	if ( arrContains(deadMobsArray,10606) ) {
	waitpulse 4
	actor.updateJournalTask('Search and Rescue', 4);
	}
}