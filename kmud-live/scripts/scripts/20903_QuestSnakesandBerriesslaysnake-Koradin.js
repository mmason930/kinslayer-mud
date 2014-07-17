var script20903 = function(self, actor, here, args, extra) {
	//For quest: Snakes and Berries
	//Koradin - January 2010
	function f( vArgs ) {
		vArgs[0].updateJournalTask('Snakes and Berries',1);
	}
	setTimeout(1,f,[actor]);
}