var script20705 = function(self, actor, here, args, extra) {
	//Blight noises
	//Koradin - converted from DG May 2010
	var rand = random(1,3);
	if (rand == 1)
		self.zecho("The shrieking sound of death fills the area.");
	else if (rand == 2)
		self.zecho("An ear piercing howl sounds from somewhere in the distance.");
	else if (rand == 3)
		self.zecho("A rancid smell seeps in, soon becoming overwhelming.");
}