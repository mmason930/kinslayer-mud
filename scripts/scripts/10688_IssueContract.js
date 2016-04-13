var script10688 = function(self, actor, here, args, extra) {
	var contracts = new Array();
	contracts[0] = "say im in ur mud, steelin ur lewtz";
	contracts[1] = "say i can haz heron?";
	contracts[2] = "say blademaster kitteh iz pwnin teh nubs";
	contracts[3] = "purr";
	contracts[4] = "hug " + here.people[random(0, here.people.length - 1)];
	//add more contracts here
	self.comm(contracts[random(0, contracts.length - 1)]);
}