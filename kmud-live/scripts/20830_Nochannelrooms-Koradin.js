var script20830 = function(self, actor, here, args, extra)
{
	//No channel rooms
//Koradin - April 2010
var total_time = 12;
while (total_time > 0)
{
	var people = self.people;
	for each(var person in people)
	{
		if (person.hasSource() && !person.isBashed && random(1,5) == 1)
		{
			person.send("Your surroundings make you lose focus!");
			person.comm("release");
			act("$n seems to lose focus for a moment.",true,person,null,null,constants.TO_ROOM);
		}
	}
	wait 1;
	total_time -= 1;
}

};

