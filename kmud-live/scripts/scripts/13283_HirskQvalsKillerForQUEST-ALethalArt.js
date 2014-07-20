var script13283 = function(self, actor, here, args, extra) {
	function f( vArgs ) {
		for (var _autoKey in vArgs) {
			var person = vArgs[_autoKey];
			person.updateJournalTask('A Lethal Art',0);
		}
	}
	setTimeout(1,f,here.people);
}