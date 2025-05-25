var script10202 = function(self, actor, here, args, extra) {
	let memes = [
		"say im in ur mud, steelin ur lewtz",
		"say i can haz heron?",
		"say blademaster kitteh iz pwnin teh nubs",
		"purr",
		"hug " + here.people[random(0, here.people.length - 1)].name
	];

	self.comm(memes[random(0, memes.length - 1)]);
}