var script20899 = function(self, actor, here, args, extra) {
	for (var _autoKey in self.people) {
		var person = self.people[_autoKey];
		if (person.vnum > 0)
			person.extract();
	}
}