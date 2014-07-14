var script20899 = function(self, actor, here, args, extra)
{
	for each (var person in self.people) {
	if (person.vnum > 0)
		person.extract();
}

};

