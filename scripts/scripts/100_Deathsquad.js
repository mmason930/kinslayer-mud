let script100 = function(self, actor, here, args, extra) {
	let argv = args.split(" ");
	let target = argv[1];
	let people = here.people;
	let victim = false;
	for(let i =0; i < people.length; ++i)
	{
	    if (people[i].name === target)
	    {
	        victim = people[i];
	        self.say("You shall die for your crimes, " + victim.name);
	        victim.damage(1000,self);
	    }
	}
}