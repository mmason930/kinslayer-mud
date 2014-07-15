var script13283 = function(self, actor, here, args, extra) {
	function f( vArgs ) {
		for each ( var person in vArgs ) {
			person.updateJournalTask('A Lethal Art',0);
		}
	}
	setTimeout(1,f,here.people);
}