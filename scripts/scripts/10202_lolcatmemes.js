var script10202 = function(self, actor, here, args, extra) {
	var memes = new Array();
	memes[0] = "say im in ur mud, steelin ur lewtz";
	memes[1] = "say i can haz heron?";
	memes[2] = "say blademaster kitteh iz pwnin teh nubs";
	memes[3] = "purr";
	memes[4] = "hug " + here.people[random(0, here.people.length - 1)];
	//add more memes here
	self.comm(memes[random(0, memes.length - 1)]);
}