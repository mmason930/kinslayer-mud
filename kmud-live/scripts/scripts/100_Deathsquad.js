var script100 = function(self, actor, here, args, extra) {
	var argv = args.split(" ");
	var target = argv[1];
	var people = here.people;
	var victim = false;
	for(var i =0; i < people.length; ++i)
	{
	    if (people[i].name == target)
	    {
	        victim = people[i];
	        self.say("You shall die for your crimes, " + victim.name);
	        victim.damage(1000,self);
	    }
	}
}